#include "model.h"
#include <curl/curl.h>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <string>
#include <cstdlib> // getenv
#include <iostream>

using json = nlohmann::json;

static std::string extractOutputText(const std::string &rawResponse)
{
    json root = json::parse(rawResponse);
    std::string result;

    if (!root.contains("output") || !root["output"].is_array())
        throw std::runtime_error(
            "There is no extractable text in the response");

    for (const auto &item : root["output"])
    {
        if (!item.contains("content") || !item["content"].is_array())
            continue;

        for (const auto &content : item["content"])
        {
            if (content.value("type", "") == "output_text")
            {
                result += content.value("text", "");
            }
        }
    }
    if (result.empty())
        throw std::runtime_error("Response has no extractable text");

    return result;
}

Model::Model(
    const std::string &apiKey,
    const std::string &baseUrl,
    const std::string &modelName)
    : apiKey(apiKey.empty()
                 ? (getenv("MINI_HARNESS_API_KEY")
                        ? getenv("MINI_HARNESS_API_KEY")
                        : "")
                 : apiKey),
      baseUrl(baseUrl),
      modelName(modelName)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

Model::~Model() { curl_global_cleanup(); }

size_t Model::WriteCallback(
    void *contents, size_t size, size_t count, void *userData)
{
    auto *response = static_cast<std::string *>(userData);
    const size_t total = size * count;
    response->append(static_cast<char *>(contents), total);
    return total;
}

std::string Model::chat(const std::string &content)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        throw std::runtime_error("Unable to initialize CURL");
    std::string response;
    const std::string url = baseUrl + "/responses";
    // std::string url = baseUrl + "/chat/completions";

    json jsonBody;
    jsonBody["model"] = modelName;
    jsonBody["input"] = json::parse(content);
    const std::string jsonBodyStr = jsonBody.dump();

    curl_slist *headers = nullptr;
    headers = curl_slist_append(headers,
                                ("Authorization: Bearer " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBodyStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    const bool debug = std::getenv("MINI_HARNESS_DEBUG") != nullptr;
    if (debug)
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode result = curl_easy_perform(curl);

    long httpStatus = 0;
    double totalTime = 0.0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &totalTime);

    if (debug)
    {
        std::cerr << "[HTTP] status=" << httpStatus
                  << ", total_time=" << totalTime << "s\n";
    }

    if (result != CURLE_OK)
    {
        const std::string message =
            "CURL request failed: " +
            std::string(curl_easy_strerror(result)) +
            "\nHTTP status: " + std::to_string(httpStatus) +
            "\nResponse:\n" + response;

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error(message);
    }

    if (httpStatus < 200 || httpStatus >= 300)
    {
        const std::string message =
            "HTTP request failed, status: " +
            std::to_string(httpStatus) +
            "\nResponse:\n" + response;

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error(message);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    try
    {
        return extractOutputText(response);
    }
    catch (const std::exception &error)
    {
        throw std::runtime_error(
            std::string("Failed to parse model response: ") +
            error.what() + "\nRaw response:\n" + response);
    }
}
