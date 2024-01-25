cmake_minimum_required(VERSION 3.26)
project(cpplox_bytecode)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BINARY_NAME cpplox_bytecode)
set(LIBRARY_NAME cpplox_lib)

include(FetchContent)
# gtest
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)
add_subdirectory(src)