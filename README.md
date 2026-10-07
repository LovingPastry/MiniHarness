# MiniHarness

MiniHarness 是一个轻量级 Agent（智能体）测试框架。它提供一个命令行交互环境，用户输入任务后，程序会调用大语言模型 API，并允许模型通过内置工具读取和写入本地文件，直到任务完成。

## 项目作用

- 实现一个简单的 Agent 循环，持续接收用户输入并驱动大模型。
- 支持模型调用本地工具（目前内置 `read_file` 和 `write_file`）。
- 可用于验证 Agent 协议、工具调用流程以及本地 LLM 工程。

## 目录结构

```
MiniHarness/
├── main.cpp          # 程序入口：读取用户输入并启动 Agent 循环
├── include/
│   ├── agent.h       # Agent 与 AgentState 声明
│   ├── message.h     # 消息类声明
│   ├── model.h       # 模型接口类声明
│   └── tool.h        # 工具类声明与全局 tools 注册表
├── src/
│   ├── agent.cpp     # Agent 循环、系统提示词、工具调用解析
│   ├── message.cpp   # 消息类实现
│   ├── model.cpp     # 基于 libcurl 的模型 HTTP 调用实现
│   └── tool.cpp      # 内置工具实现与注册
└── README.md
```

## 设计思路

1. **消息与状态管理**  
   所有用户、助手和系统消息统一由 `Message` 表示，并保存在 `AgentState` 中。每次模型调用前，都会将当前完整对话历史序列化为 JSON 格式的 `input` 发送给模型。

2. **模型调用封装**  
   `Model` 类封装了基于 libcurl 的 HTTP POST 请求，默认访问 DeepSeek 的 `/responses` 接口。API Key 优先从环境变量 `MINI_HARNESS_API_KEY` 读取。

3. **工具调用协议**  
   系统提示词要求模型在需要调用工具时输出固定三行格式：

   ```
   ToolCall
   <工具名>
   <参数>
   ```

   程序解析该格式后执行对应工具，并将结果作为用户消息写回对话历史。任务完成时模型输出 `TaskFinished`。

4. **内置工具**  
   - `read_file`：读取指定文件内容  
   - `write_file`：将路径写入第一行，文件内容从第二行开始写入

5. **循环控制**  
   每个任务最多执行 `MAX_STEPS=100` 步，防止无限循环。任务结束后内部 `finished` 状态会被重置，程序返回主循环等待下一条用户输入。

## 依赖

- C++17 或更高版本
- [libcurl](https://curl.se/libcurl/)
- [nlohmann/json](https://github.com/nlohmann/json)

## 环境变量

| 变量名 | 说明 |
|--------|------|
| `MINI_HARNESS_API_KEY` | 大模型 API 密钥 |
| `MINI_HARNESS_DEBUG` | 设置后启用 HTTP 详细调试输出 |

## 构建与运行

示例编译命令：

```bash
g++ -std=c++17 main.cpp src/*.cpp -Iinclude -lcurl -o miniharness
```

运行：

```bash
./miniharness
```

启动后出现 `User:` 提示，输入任务即可。模型可以调用内置工具读写文件，并在完成后输出 `TaskFinished`。