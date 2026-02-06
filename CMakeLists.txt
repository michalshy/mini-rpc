cmake_minimum_required(VERSION 3.14)
project(mini-rpc VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

# create a sample test file if none exists
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/tests")
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/tests")
endif()

add_executable(tests_run
    tests/test_sample.cpp
)

target_link_libraries(tests_run PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

catch_discover_tests(tests_run)