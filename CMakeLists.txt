cmake_minimum_required(VERSION 3.10)

project(Snake C)

set(CMAKE_C_STANDARD 11)

find_package(Threads REQUIRED)


# server
add_executable(server server.c)
target_link_libraries(server PRIVATE Threads::Threads)

# client
add_executable(client client.c)
target_link_libraries(client PRIVATE Threads::Threads)