cmake_minimum_required(VERSION 3.16)
project(untitled1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_executable(untitled1
        main.cpp
        structure/API/Database.cpp
        structure/API/ServerPort.cpp
        structure/FormalStructures/Item.cpp
        structure/FormalStructures/PrefixTree.cpp
        structure/FormalStructures/Stack.cpp
        structure/FormalStructures/TrieNode.cpp
        structure/logic/DockerSystem.cpp
        structure/logic/ErrorDetector.cpp
)

target_link_libraries(untitled1 PRIVATE SQLite::SQLite3)
