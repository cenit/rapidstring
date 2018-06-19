project(rapidstring_benchmark LANGUAGES CXX)

add_executable(rapidstring_benchmark
	src/main.cpp
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests" FORCE)
add_subdirectory(lib/benchmark)

target_link_libraries(rapidstring_benchmark PRIVATE rapidstring benchmark)
target_compile_features(rapidstring_benchmark PRIVATE cxx_std_11)
target_compile_warnings(rapidstring_benchmark)
