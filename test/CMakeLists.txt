project(rapid_string_test LANGUAGES CXX)
add_executable(rapid_string_test
	src/concat.cpp
	src/construct.cpp
	src/copy.cpp
	src/main.cpp
)

target_compile_features(rapid_string_test PRIVATE cxx_std_11)

# TODO: move to common function
# TODO: add crazy optimization flags
if (MSVC)
	target_compile_options(rapid_string_test PRIVATE /W4)
elseif(AppleClang OR Clang OR GNU OR Intel)
	target_compile_options(rapid_string_test PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(rapid_string_test
	PRIVATE
		../include
		lib/Catch2/single_include
)

target_compile_definitions(rapid_string_test
	PRIVATE
		RS_ASSERTS
)

