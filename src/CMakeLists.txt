add_library(common common/socket_fd.cpp)
target_include_directories(common PUBLIC common)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -O1")

add_executable(client
    client/main.cpp
    client/client.cpp
)
# add_executable(server server/main.cpp server/server.cpp)
# add_executable(statistics statistics/main.cpp)

target_link_libraries(client PRIVATE common)
target_include_directories(client PRIVATE client)
