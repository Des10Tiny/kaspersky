include(FetchContent)

fetchcontent_declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

fetchcontent_makeavailable(googletest)

add_executable(client_tests
    test_client/client_tests.cpp
    ../src/client/client.cpp
)
target_include_directories(client_tests PRIVATE ../src/client)
target_link_libraries(client_tests common gtest gtest_main pthread)

add_executable(scanner_tests
    test_server/scanner_tests.cpp
    ../src/server/scanner.cpp
)
target_include_directories(scanner_tests PRIVATE ../src/server)
target_link_libraries(scanner_tests gtest gtest_main pthread)


add_executable(shared_stats_tests
    test_server/shared_stats_tests.cpp
    ../src/server/shared_stats.cpp
)
target_include_directories(shared_stats_tests PRIVATE ../src/server)
target_link_libraries(shared_stats_tests gtest gtest_main pthread)

add_test(NAME ClientTests COMMAND client_tests)
add_test(NAME ScannerTests COMMAND scanner_tests)
add_test(NAME SharedStatsTests COMMAND shared_stats_tests)
