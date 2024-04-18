cmake_minimum_required(VERSION 3.22)

# project name - CMAKE_PROJECT_NAME
project(Vedit LANGUAGES CXX)

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_BINARY_DIR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# add all libs
add_subdirectory("src")
add_subdirectory("extern")