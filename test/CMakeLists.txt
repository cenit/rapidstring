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

if (RS_COVERAGE)
	message(WARNING ${CMAKE_BUILD_TYPE})
	target_compile_definitions(rapidstring_test
		PRIVATE
			RS_NOINLINE
	)
	target_compile_options(rapidstring_test
		PRIVATE
			-fprofile-arcs
			-ftest-coverage
			-Wno-unused-function
	)
	target_link_libraries(rapidstring_test
		PRIVATE
			-lgcov
	)
endif()
