#ifndef RESIZE_HPP_0339C33297A54DF5
#define RESIZE_HPP_0339C33297A54DF5

#include "rapid_string.h"
#include <benchmark/benchmark.h>
#include <cstddef>
#include <string>

constexpr const std::size_t resize_count{ 1000 };

inline void rs_resize(benchmark::State& state)
{
	for (auto _ : state) {
		rapid_string s;
		rs_init(&s);
		rs_resize(&s, resize_count);
		benchmark::DoNotOptimize(s);
		rs_free(&s);
	}
}

inline void std_resize(benchmark::State& state)
{
	for (auto _ : state) {
		std::string s;
		s.resize(resize_count);
		benchmark::DoNotOptimize(s);
	}
}

#endif // !RESIZE_HPP_0339C33297A54DF5
