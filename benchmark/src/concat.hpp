#ifndef CONCAT_HPP_23EF09BE616AE9A8
#define CONCAT_HPP_23EF09BE616AE9A8

#include "rapidstring.h"
#include <benchmark/benchmark.h>
#include <cstddef>
#include <string>

constexpr std::size_t count{ 100 };
constexpr const char concat_str[]{ "A fairly long string for concatenation" };
constexpr std::size_t concat_size{ sizeof(concat_str) - 1 };

/*
 * No benchmarks are done for concatenation without specifying the length as
 * this would be identicle to adding the execution time of strlen() to the
 * benchmarks bellow.
 */

inline void rs_cat(benchmark::State &state)
{
	for (auto _ : state) {
		rapidstring s;
		rs_init(&s);

		for (size_t i = 0; i < count; i++)
			rs_cat_n(&s, concat_str, concat_size);

		benchmark::DoNotOptimize(s);
		rs_free(&s);
	}
}

inline void rs_reserve_concat(benchmark::State &state)
{
	for (auto _ : state) {
		rapidstring s;
		rs_init_w_cap(&s, concat_size * count);

		/*
		 * The heap concatenation function may be used as we are sure
		 * the string will always be on the heap AND we are sure we have
		 * enough capacity.
		 */
		for (size_t i = 0; i < count; i++)
			rs_heap_cat_n(&s, concat_str, concat_size);

		benchmark::DoNotOptimize(s);
		rs_free(&s);
	}
}

inline void std_reserve_concat(benchmark::State &state)
{
	for (auto _ : state) {
		std::string s;
		s.reserve(concat_size * count);

		for (size_t i = 0; i < count; i++)
			s.append(concat_str, concat_size);

		benchmark::DoNotOptimize(s);
	}
}

inline void std_concat(benchmark::State &state)
{
	for (auto _ : state) {
		std::string s;

		for (size_t i = 0; i < count; i++)
			s.append(concat_str, concat_size);

		benchmark::DoNotOptimize(s);
	}
}

#endif /* !CONCAT_HPP_23EF09BE616AE9A8 */
