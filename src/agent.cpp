#include "agent.h"
#include "tool.h"
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#define MAX_STEPS 100

inline Message BuildSystemPrompt()
{
    string prompt = R"(You are an Agent.

Task protocol:

- Read the user's task carefully.
- Use a tool when necessary.
- To call a tool, output exactly three lines:
  ToolCall
  <tool_name>
  <arguments>

- After the task is fully completed, output exactly:
  TaskFinished

Available tools:
)";

    for (const auto &[name, tool] : tools)
    {
        prompt += "\n- " + name + "\n";
        prompt += "  " + tool.description + "\n";
    }

    prompt += R"(
Strict rules:

- The tool name must exactly match one of the available tools.
- The argument must follow the format described by the tool.
- Do not use Markdown fences around ToolCall.
- Do not output extra text around ToolCall.
)";

    return Message(prompt, "system");
}

AgentState::AgentState() : messages{BuildSystemPrompt()} {}

string AgentState::getContent() const
{
    json content = json::array();
    for (int i = 0; i < messages.size(); ++i)
    {
        content.push_back(
            {{"role", messages[i].getRole()},
             {"content", messages[i].getContent()}});
    }
    return content.dump();
}

void Agent::tool_call(const string &response)
{
    if (!response.starts_with("ToolCall"))
        return;
    istringstream iss(response);
    string tag, name, args;
    if (getline(iss, tag) && tag == "ToolCall" && getline(iss, name) && getline(iss, args, '\0'))
    {
        auto it = tools.find(name);
        state.messages.emplace_back(
            "ToolResult:\n" + (it != tools.end() ? it->second.execute(args) : "Tool not found"),
            "user");
    }
    else
        state.messages.emplace_back(
            "ToolResult:\nTool Call Error, please check ToolCall format", "user");
}

void Agent::finish_task(string &response)
{
    if (response.ends_with("TaskFinished"))
        state.finished = true;
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
}
void Agent::agent_loop()
{
    while (!state.finished && state.step < MAX_STEPS)
    {
        string content = state.getContent();
        // cout << "Content: " << content << endl;
        string response = model.chat(content);
        cout << "Assistant: " << response << endl;
        state.messages.emplace_back(response, "assistant");
        finish_task(response);
        tool_call(response);
        ++state.step;
    }
}
