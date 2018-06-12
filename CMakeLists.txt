cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(rapidstring LANGUAGES C VERSION 0.1.0)
add_library(rapidstring INTERFACE)
target_include_directories(rapidstring INTERFACE include)

include(${CMAKE_SOURCE_DIR}/cmake/utility.cmake)

# TODO: Installation.

add_subdirectory(test)
add_subdirectory(benchmark)
