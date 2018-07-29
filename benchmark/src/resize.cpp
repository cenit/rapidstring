#include "rapidstring.h"
#include <benchmark/benchmark.h>
#include <cstddef>
#include <string>

constexpr std::size_t resize_count{ 1000 };

void rs_resize_test(benchmark::State &state)
{
	for (auto _ : state) {
		rapidstring s;
		rs_init(&s);
		rs_resize(&s, resize_count);
		benchmark::DoNotOptimize(s);
		rs_free(&s);
	}
}

BENCHMARK(rs_resize_test);

void std_resize(benchmark::State &state)
{
	for (auto _ : state) {
		std::string s;
		s.resize(resize_count);
		benchmark::DoNotOptimize(s);
	}
}

BENCHMARK(std_resize);
