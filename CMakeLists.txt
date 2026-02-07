cmake_minimum_required(VERSION 3.14)
project(mini-rpc VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

# Create a shared library from sources in src/ and public/
file(GLOB_RECURSE SRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cxx"
    "${CMAKE_SOURCE_DIR}/src/*.cc"
)

file(GLOB_RECURSE PUB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/public/*.cpp"
    "${CMAKE_SOURCE_DIR}/public/*.cxx"
    "${CMAKE_SOURCE_DIR}/public/*.cc"
)

# Create glob for tests and examples
file(GLOB_RECURSE GLOB_EXAMPLES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/examples/*.cpp"
    "${CMAKE_SOURCE_DIR}/examples/*.cxx"
    "${CMAKE_SOURCE_DIR}/examples/*.cc"
)

file(GLOB_RECURSE GLOB_TESTS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
    "${CMAKE_SOURCE_DIR}/tests/*.cxx"
    "${CMAKE_SOURCE_DIR}/tests/*.cc"
)

set(LIB_SOURCES ${SRC_SOURCES} ${PUB_SOURCES})

# normalize project name to a target-friendly identifier
string(REPLACE "-" "_" PROJECT_ID ${PROJECT_NAME})

if(LIB_SOURCES)
    # Create an OBJECT library for internal access
    add_library(${PROJECT_ID}_objs OBJECT ${LIB_SOURCES})
    target_include_directories(${PROJECT_ID}_objs PUBLIC
        "${CMAKE_SOURCE_DIR}/public"
        "${CMAKE_SOURCE_DIR}/src"
    )

    # Create shared library from object library
    add_library(${PROJECT_ID} SHARED $<TARGET_OBJECTS:${PROJECT_ID}_objs>)
    target_include_directories(${PROJECT_ID} PUBLIC "${CMAKE_SOURCE_DIR}/public")
else()
    message(STATUS "No sources found in src/ or public/, skipping ${PROJECT_ID} library")
endif()

# Build each example source file as a separate executable
file(GLOB GLOB_EXAMPLES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/examples/*.cpp"
    "${CMAKE_SOURCE_DIR}/examples/*.cxx"
    "${CMAKE_SOURCE_DIR}/examples/*.cc")

foreach(example_src IN LISTS GLOB_EXAMPLES)
    get_filename_component(example_name ${example_src} NAME_WE)
    add_executable(${example_name} ${example_src})
    target_include_directories(${example_name} PRIVATE "${CMAKE_SOURCE_DIR}/public")
    if(TARGET ${PROJECT_ID})
        target_link_libraries(${example_name} PRIVATE ${PROJECT_ID})
    endif()
endforeach()

# Ensure tests dir exists
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/tests")
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/tests")
endif()

# Build each test source file as a separate executable and register with CTest
file(GLOB GLOB_TESTS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
    "${CMAKE_SOURCE_DIR}/tests/*.cxx"
    "${CMAKE_SOURCE_DIR}/tests/*.cc")

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

foreach(test_src IN LISTS GLOB_TESTS)
    get_filename_component(test_name ${test_src} NAME_WE)
    set(test_target "${test_name}_exe")
    add_executable(${test_target} ${test_src})
    
    # Tests get access to both public/ and src/ directories
    target_include_directories(${test_target} PRIVATE
        "${CMAKE_SOURCE_DIR}/public"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/rpc"
        "${CMAKE_SOURCE_DIR}/src/protocol"
        "${CMAKE_SOURCE_DIR}/src/transport"
    )
    
    target_link_libraries(${test_target} PRIVATE Catch2::Catch2WithMain)
    
    # Link against the object library for full access to internals
    if(TARGET ${PROJECT_ID}_objs)
        target_link_libraries(${test_target} PRIVATE ${PROJECT_ID}_objs)
    endif()
    
    catch_discover_tests(${test_target})
endforeach()