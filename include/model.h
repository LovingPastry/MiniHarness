#pragma once
#include <string>

class Model
{
public:
    Model(
        const std::string &apiKey = "",
        const std::string &baseUrl = "https://api.deepseek.com",
        const std::string &modelName = "deepseek-v4-pro");
    ~Model();
    std::string chat(const std::string &content);

private:
    std::string apiKey, baseUrl, modelName;
    static size_t WriteCallback(
        void *contents,
        size_t size,
        size_t count,
        void *userData);
};
