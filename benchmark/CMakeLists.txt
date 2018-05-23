project(rapidstring_benchmark LANGUAGES CXX)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(rapidstring_benchmark
	src/main.cpp
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests" FORCE)
add_subdirectory(lib/benchmark)

target_include_directories(rapidstring_benchmark
	PRIVATE
		../include
		lib/benchmark/include
)

target_compile_features(rapidstring_benchmark PRIVATE cxx_std_11)

if (MSVC)
	target_compile_options(rapidstring_benchmark 
		PRIVATE 
			/W4
	)
elseif(AppleClang OR Clang OR GNU OR Intel)
	target_compile_options(rapidstring_benchmark 
		PRIVATE 
			-Wall
			-Wextra
			-pedantic
			-O3
			-Ofast
	)
endif()

target_link_libraries(rapidstring_benchmark
	PRIVATE
		benchmark
)
