#ifndef CONSTRUCT_HPP_7DFD4B503BE48168
#define CONSTRUCT_HPP_7DFD4B503BE48168

#include "rapidstring.h"
#include <benchmark/benchmark.h>
#include <string>

#define STR_12 ("123456789012")
#define STR_24 ("123456789012345678901234")
#define STR_48 ("123456789012345678901234567890123456789012345678")

inline void rs_12_byte_construct(benchmark::State &state)
{
	rapidstring s;

	for (auto _ : state) {
		rs_init_w_n(&s, STR_12, 12);

#ifdef __clang__
		benchmark::DoNotOptimize(s);
#endif

		rs_free(&s);
	}

	benchmark::DoNotOptimize(s);
}

inline void std_12_byte_construct(benchmark::State &state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(std::string{ STR_12, 12 });
}

inline void rs_24_byte_construct(benchmark::State &state)
{
	rapidstring s;

	for (auto _ : state) {
		rs_init_w_n(&s, STR_24, 24);

#ifdef __clang__
		benchmark::DoNotOptimize(s);
#endif

		rs_free(&s);
	}

	benchmark::DoNotOptimize(s);
}

inline void std_24_byte_construct(benchmark::State &state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(std::string{ STR_24, 24 });
}

inline void rs_48_byte_construct(benchmark::State &state)
{
	rapidstring s;

	for (auto _ : state) {
		rs_init_w_n(&s, STR_48, 48);

#ifdef __clang__
		benchmark::DoNotOptimize(s);
#endif

		rs_free(&s);
	}

	benchmark::DoNotOptimize(s);
}

inline void std_48_byte_construct(benchmark::State &state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(std::string{ STR_48, 48 });
}

#endif // !CONSTRUCT_HPP_7DFD4B503BE48168
