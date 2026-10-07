#pragma once
#include <string>
using namespace std;

class Message
{
public:
    Message() = default;
    Message(const string &content, const string &role);
    string getContent() const;
    string getRole() const;

private:
    string content;
    string role;
};
