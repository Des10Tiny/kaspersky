add_library(common common/socket_fd.cpp)
target_include_directories(common PUBLIC common)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -O1")

add_executable(client
    client/main.cpp
    client/client.cpp
)

target_link_libraries(client PRIVATE common)
target_include_directories(client PRIVATE client)

add_executable(server
    server/main.cpp
    server/server.cpp
    server/scanner.cpp
    server/shared_stats.cpp
)
target_link_libraries(server PRIVATE common)
target_include_directories(server PRIVATE server)
