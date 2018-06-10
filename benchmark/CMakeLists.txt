project(rapidstring_benchmark LANGUAGES CXX)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(rapidstring_benchmark
	src/main.cpp
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests" FORCE)
add_subdirectory(lib/benchmark)

target_compile_features(rapidstring_benchmark PRIVATE cxx_std_11)
target_compile_warnings(rapidstring_benchmark)
target_include_directories(rapidstring_benchmark
	PRIVATE
		../include
		lib/benchmark/include
)
target_link_libraries(rapidstring_benchmark
	PRIVATE
		benchmark
)
