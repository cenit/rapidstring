cmake_minimum_required(VERSION 3.2 VERSION 0.1.0)
project(rapidstring LANGUAGES C)

# TODO: installation?

add_subdirectory(test)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_subdirectory(benchmark)
endif()
