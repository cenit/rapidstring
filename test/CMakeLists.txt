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
# TODO: add crazy optimization flags
if (MSVC)
	target_compile_options(rapidstring_test
		PRIVATE
			/Wall
			/WX
			/Zc
	)
elseif(AppleClang OR Clang OR GNU OR Intel)
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
