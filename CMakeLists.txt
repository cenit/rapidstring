cmake_minimum_required(VERSION 3.2 VERSION 0.1.0)
project(rapidstring LANGUAGES C)

# TODO: installation?

add_subdirectory(test)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_subdirectory(benchmark)
endif()

OPTION(ENABLE_GCOV "Enable gcov (debug, Linux builds only)" OFF)

IF (ENABLE_GCOV AND NOT WIN32 AND NOT APPLE)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -lgcov")
ENDIF()
