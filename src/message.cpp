#include "message.h"
using namespace std;
Message::Message(const string &content, const string &role)
    : content(content), role(role) {}

string Message::getContent() const { return content; }
string Message::getRole() const { return role; }
