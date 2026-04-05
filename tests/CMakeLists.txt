include(FetchContent)

fetchcontent_declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

fetchcontent_makeavailable(googletest)

add_executable(client_tests test_client/client_tests.cpp ../src/client/client.cpp)

target_include_directories(client_tests PRIVATE ../src/client)
target_link_libraries(client_tests common gtest gtest_main pthread)
