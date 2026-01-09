cmake_minimum_required(VERSION 3.10)
project(SnakeGame C)

# Použiť C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Threads (pre multithread servera/klienta)
find_package(Threads REQUIRED)

# --- Server ---
add_executable(server
        server.c
        snake.c
        world.c
)
target_link_libraries(server PRIVATE Threads::Threads)

# --- Client ---
add_executable(client
        client.c
        snake.c
        world.c
)
target_link_libraries(client PRIVATE Threads::Threads)
