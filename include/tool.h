#pragma once
#include <functional>
#include <string>
#include <unordered_map>

using namespace std;
using ExecuteFunction = function<string(const string &)>;

class Tool
{
public:
    string name;
    string description;
    ExecuteFunction execute;
    Tool(const string &name, const string &description, ExecuteFunction execute);
};

extern unordered_map<string, Tool> tools;