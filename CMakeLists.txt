cmake_minimum_required(VERSION 3.10)
project(SnakeGame C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Threads REQUIRED)

add_executable(server
        server.c
        world.c
)
target_link_libraries(server PRIVATE Threads::Threads)

add_executable(client
        client.c
        world.c
        world.h
        client.h
        server.h
)
target_link_libraries(client PRIVATE Threads::Threads)

add_custom_target(build_all DEPENDS server client)
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/client
)
