cmake_minimum_required(VERSION 3.28)
project(eksamen)

set(CMAKE_CXX_STANDARD 17)

add_executable(eksamen main.cpp include/Server.h include/Client.h src/Client.cpp src/Server.cpp
        include/Player.h
        src/Player.cpp
        include/State.h
        src/State.cpp)

target_link_libraries(eksamen PRIVATE ws2_32 mswsock)

