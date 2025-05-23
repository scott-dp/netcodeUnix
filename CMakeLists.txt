cmake_minimum_required(VERSION 3.28)
project(linuxserver)

set(CMAKE_CXX_STANDARD 17)

add_executable(linuxserver main.cpp include/Server.h src/Server.cpp
        include/Game/Player.h
        src/Game/Player.cpp
        include/Game/State.h
        src/Game/State.cpp
        src/worker_threads/Workers.cpp
        include/worker_threads/Workers.h
)

target_link_libraries(linuxserver pthread)

