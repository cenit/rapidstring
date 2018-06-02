project(rapidstring_test LANGUAGES CXX)
add_executable(rapidstring_test
	src/access.cpp
	src/assign.cpp
	src/append.cpp
	src/construct.cpp
	src/main.cpp
)

# TODO: some test for ansi compliance

target_compile_features(rapidstring_test PRIVATE cxx_std_11)

# TODO: move to common function
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(rapidstring_test
		PRIVATE
			/W4
			/WX
	)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|Intel")
	target_compile_options(rapidstring_test
		PRIVATE
			-Wall
			-Wextra
			-Wpedantic
			-Werror
	)
endif()

target_include_directories(rapidstring_test
	PRIVATE
		../include
		lib/Catch2/single_include
)
OPTION(ENABLE_GCOV "Enable gcov (debug, Linux builds only)" OFF)

IF (ENABLE_GCOV AND NOT WIN32 AND NOT APPLE)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -lgcov")
ENDIF()
