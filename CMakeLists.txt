cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(rapidstring LANGUAGES C VERSION 0.1.0)

# TODO: installation?

add_subdirectory(test)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_subdirectory(benchmark)
endif()
