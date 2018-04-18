#pragma once

#include "rapid_string.h"
#include "sds.h"
#include <benchmark\benchmark.h>
#include <string>

#define CAT_STR "Hello World!"
#define CAT_STR_LEN (sizeof(CAT_STR) - 1)

inline void rs_concat(benchmark::State& state)
{
	rapid_string s = rs_empty();

	for (auto _ : state)
		rs_cat_n(&s, CAT_STR, CAT_STR_LEN);

	rs_free(&s);
}

inline void sds_concat(benchmark::State& state)
{
	sds s = sdsempty();

	for (auto _ : state)
		s = sdscatlen(s, CAT_STR, CAT_STR_LEN);

	sdsfree(s);
}

inline void std_concat(benchmark::State& state)
{
	std::string s;

	for (auto _ : state)
		s.append(CAT_STR, CAT_STR_LEN);
}
