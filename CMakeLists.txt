cmake_minimum_required(VERSION 3.16)
project(NetGrep)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(COMMAND ln -sf ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()

option(BUILD_TESTS "Build all unit tests" ON)
add_subdirectory(src)

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(BUILD_TESTS OFF)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT error)

    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()

endif()



if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


