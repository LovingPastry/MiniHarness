#include "tool.h"
#include <fstream>
#include <sstream>
using namespace std;

Tool::Tool(
    const string &name,
    const string &description,
    ExecuteFunction execute)
    : name(name),
      description(description),
      execute(execute)
{
}

Tool ReadFileTool{
    "read_file",
    "Reads a file. Argument format: one line containing the file path.",
    [](const string &filePath)
    {
        ifstream file(filePath);

        if (!file.is_open())
        {
            return string("Error: Could not open file.");
        }

        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }};

Tool WriteFileTool{
    "write_file",
    "Write contents into a file. "
    "Argument format : First line containing the file path, "
    "file content starts on the second line.",
    [](const string &args)
    {
        int pos = args.find('\n');
        string filePath = args.substr(0, pos);
        string content = args.substr(pos + 1);

        ofstream file(filePath);
        if (!file.is_open())
        {
            return string("Error: Could not open file for writing.");
        }
        file << content;
        file.close();
        return string("File written successfully.");
    }};

unordered_map<string, Tool> tools{
    {ReadFileTool.name, ReadFileTool},
    {WriteFileTool.name, WriteFileTool}};