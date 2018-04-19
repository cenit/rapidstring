#pragma once

#include "rapid_string.h"
#include <benchmark\benchmark.h>
#include <string>

#define CAT_COUNT 100
#define CAT_STR "A fairly long string for concatenation"
#define CAT_STR_LEN (sizeof(CAT_STR) - 1)

/*
 * No benchmarks are done for concatenation without specifying the length as
 * this would be identicle to adding the execution time of strlen() to the
 * benchmarks bellow.
 */

inline void rs_concat(benchmark::State& state)
{
	for (auto _ : state) {
		rapid_string s = rs_empty();

		for (size_t i = 0; i < CAT_COUNT; i++)
			rs_cat_n(&s, CAT_STR, CAT_STR_LEN);

		rs_free(&s);
	}
}

inline void std_concat(benchmark::State& state)
{
	for (auto _ : state) {
		std::string s;

		for (size_t i = 0; i < CAT_COUNT; i++)
			s.append(CAT_STR, CAT_STR_LEN);
	}
}
