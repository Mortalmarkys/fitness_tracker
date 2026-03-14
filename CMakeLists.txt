cmake_minimum_required(VERSION 3.20)
project(FitnessTracker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(fitness_lib STATIC
        src/CsvParser.cpp
        src/Filter.cpp
        src/Statistics.cpp
        src/Exporter.cpp
)
target_include_directories(fitness_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(fitness_tracker src/main.cpp)
target_link_libraries(fitness_tracker PRIVATE fitness_lib)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)
