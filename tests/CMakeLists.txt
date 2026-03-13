cmake_minimum_required(VERSION 3.20)

add_executable(fitness_tests
    test_parser.cpp
    test_filter.cpp
    test_statistics.cpp
)

target_link_libraries(fitness_tests
    PRIVATE
    fitness_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(fitness_tests)
