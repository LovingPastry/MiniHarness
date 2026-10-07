cmake_minimum_required(VERSION 3.20)

project(MiniHarness LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)

add_executable(MiniHarness
    main.cpp
    src/agent.cpp
    src/message.cpp
    src/model.cpp
    src/tool.cpp
)

target_include_directories(MiniHarness
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MiniHarness
    PRIVATE
        CURL::libcurl
)
