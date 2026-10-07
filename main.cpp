#include <iostream>
#include <string>
#include <windows.h>
#include "message.h"
#include "model.h"
#include "tool.h"
#include "agent.h"
using namespace std;

#define MAX_MESSAGES 1000
#define MAX_STEPS 100

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    Agent agent;
    while (1)
    {
        agent.state.finished = false;
        string user_input;
        cout << "User: ";
        getline(cin, user_input);
        agent.state.messages.push_back(Message(user_input, "user"));
        agent.agent_loop();
    }

    return 0;
}
