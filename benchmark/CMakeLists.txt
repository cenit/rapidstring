project(rapid_string_benchmark LANGUAGES CXX)
add_executable(rapid_string_benchmark
	main.cpp
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
add_subdirectory(lib/benchmark)

target_include_directories(rapid_string_benchmark
	PRIVATE
		../include
		lib/benchmark/include
)

target_compile_features(rapid_string_benchmark PRIVATE cxx_std_11)

if (MSVC)
	target_compile_options(rapid_string_benchmark PRIVATE /W4)
elseif(AppleClang OR Clang OR GNU OR Intel)
	target_compile_options(rapid_string_benchmark PRIVATE -Wall -Wextra -pedantic)
endif()

target_link_libraries(rapid_string_benchmark
	PRIVATE
		benchmark
)
