#pragma once

#include "rapid_string.h"
#include "sds.h"
#include <cstddef>
#include <benchmark\benchmark.h>
#include <string>

#define RESIZE_COUNT 100000

inline void rs_resize(benchmark::State& state)
{
	for (auto _ : state) {
		auto s = rs_empty();
		rs_resize(&s, RESIZE_COUNT);
	}
}

inline void std_resize(benchmark::State& state)
{
	for (auto _ : state) {
		std::string s;
		s.resize(RESIZE_COUNT);
	}
}
