cmake_minimum_required(VERSION 3.14)
project(mini-rpc VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# On Windows, automatically export all symbols from DLLs
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

include_directories(
    "${CMAKE_SOURCE_DIR}/public"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/client"
    "${CMAKE_SOURCE_DIR}/src/server"
)

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

if(MINGW)
    link_libraries(c++print)
endif()

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

file(GLOB GLOB_TESTS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
    "${CMAKE_SOURCE_DIR}/tests/*.cxx"
    "${CMAKE_SOURCE_DIR}/tests/*.cc")

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

if(GLOB_TESTS)
    add_executable(tests ${GLOB_TESTS})
    target_include_directories(tests PRIVATE
        "${CMAKE_SOURCE_DIR}/public"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/client"
        "${CMAKE_SOURCE_DIR}/src/server"
    )
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
    if(TARGET ${PROJECT_ID}_objs)
        target_link_libraries(tests PRIVATE ${PROJECT_ID}_objs)
    endif()
    catch_discover_tests(tests)
endif()