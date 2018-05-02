#ifndef CONCAT_HPP_23EF09BE616AE9A8
#define CONCAT_HPP_23EF09BE616AE9A8

#include "rapid_string.h"
#include <benchmark/benchmark.h>
#include <string>

#define CAT_COUNT (100)
#define CAT_STR ("A fairly long string for concatenation")
#define CAT_STR_LEN (sizeof(CAT_STR) - 1)

/*
 * No benchmarks are done for concatenation without specifying the length as
 * this would be identicle to adding the execution time of strlen() to the
 * benchmarks bellow.
 */

inline void rs_concat(benchmark::State& state)
{
	for (auto _ : state) {
		rapid_string s;
		rs_init(&s);

		for (size_t i = 0; i < CAT_COUNT; i++)
			rs_append_n(&s, CAT_STR, CAT_STR_LEN);

		benchmark::DoNotOptimize(s);
		rs_free(&s);
	}
}

inline void rs_reserve_concat(benchmark::State& state) 
{
	for (auto _ : state) {
		rapid_string s;
		rs_init_w_cap(&s, CAT_STR_LEN * CAT_COUNT);

		/* 
		 * The heap concatenation function may be used as we are sure
		 * the string will always be on the heap AND we are sure we have
		 * enough capacity.
		 */
		for (size_t i = 0; i < CAT_COUNT; i++)
			rs_heap_append_n(&s, CAT_STR, CAT_STR_LEN);

		benchmark::DoNotOptimize(s);
		rs_free(&s);
	}
}

inline void std_reserve_concat(benchmark::State& state)
{
	for (auto _ : state) {
		std::string s;
		s.reserve(CAT_STR_LEN * CAT_COUNT);

		for (size_t i = 0; i < CAT_COUNT; i++)
			s.append(CAT_STR, CAT_STR_LEN);

		benchmark::DoNotOptimize(s);
	}
}

inline void std_concat(benchmark::State& state)
{
	for (auto _ : state) {
		std::string s;

		for (size_t i = 0; i < CAT_COUNT; i++)
			s.append(CAT_STR, CAT_STR_LEN);

		benchmark::DoNotOptimize(s);
	}
}

#endif // !CONCAT_HPP_23EF09BE616AE9A8
