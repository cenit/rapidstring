project(rapidstring_test LANGUAGES CXX)
add_executable(rapidstring_test
	src/access.cpp
	src/assign.cpp
	src/append.cpp
	src/construct.cpp
	src/iterator.cpp
	src/main.cpp
)

target_compile_features(rapidstring_test PRIVATE cxx_std_11)

# TODO: move to common function
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
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
			-fprofile-arcs
			-ftest-coverage
			-Werror
	)
	target_link_libraries(rapidstring_test
		PRIVATE
			--coverage
	)
endif()

target_include_directories(rapidstring_test
	PRIVATE
		../include
		lib/Catch2/single_include
)
