cmake_minimum_required(VERSION 3.10)
project(MyProject C)

set(CMAKE_C_STANDARD 11)

set(CLIENT_SRC src/client.c)
set(SERVER_SRC src/server.c)

add_executable(client ${CLIENT_SRC})

add_executable(server ${SERVER_SRC})