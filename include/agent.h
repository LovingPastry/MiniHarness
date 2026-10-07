#pragma once
#include "message.h"
#include "model.h"
#include <string>
#include <vector>
using namespace std;

class AgentState
{
public:
    vector<Message> messages;
    vector<string> tasks;
    int step = 0;
    bool finished = false;
    AgentState();
    string getContent() const;
};

class Agent
{
public:
    AgentState state;
    Model model;
    void tool_call(const string &response);
    void finish_task(string &response);
    void agent_loop();
};
