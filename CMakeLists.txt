cmake_minimum_required(VERSION 3.16)
project(CMakeEventSystem LANGUAGES CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# dependancies
include(FetchContent)
include(GoogleTest)

# gtest - requires cmake 3.13 or later
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

FetchContent_Declare(
  LogLib
    GIT_REPOSITORY https://github.com/AlexToulan/log.git
    GIT_TAG v1.0.1)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(LogLib)

# Source and test files
file(GLOB_RECURSE src_files "src/*.cpp" "src/*.hpp")
file(GLOB_RECURSE test_files "test/src/*.cpp" "test/src/*.hpp")

# Executable for testing
add_executable(testAll ${CMAKE_SOURCE_DIR}/test/main_testall.cpp ${src_files} ${test_files})
target_include_directories(testAll
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/test/
)
target_link_libraries(testAll PRIVATE GTest::gtest_main LogLib)
target_compile_features(testAll PRIVATE cxx_std_20)

gtest_discover_tests(testAll DISCOVERY_TIMEOUT 600)
