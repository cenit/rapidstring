#ifndef CONSTRUCT_HPP_7DFD4B503BE48168
#define CONSTRUCT_HPP_7DFD4B503BE48168

#include "rapidstring.h"
#include <benchmark/benchmark.h>
#include <string>

constexpr const char *str_12{ "123456789012" };
constexpr const char *str_24{ "123456789012345678901234" };
constexpr const char *str_48{
	"123456789012345678901234567890123456789012345678"
};

inline void create_rapidstring(rapidstring *s, const char *buff, std::size_t sz)
{
	rs_init_w_n(s, buff, sz);

	/*
	 * Clang will optimize everything away if this isn't present. However, it
	 * also drastically slows down GCC.
	 */
#ifdef __clang__
	benchmark::DoNotOptimize(s);
#endif

	rs_free(s);
}

inline void rs_12_byte_construct(benchmark::State &state)
{
	rapidstring s;

	for (auto _ : state) {
		create_rapidstring(&s, str_12, 12);
	}

	benchmark::DoNotOptimize(s);
}

inline void std_12_byte_construct(benchmark::State &state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(std::string{ str_12, 12 });
}

inline void rs_24_byte_construct(benchmark::State &state)
{
	rapidstring s;

	for (auto _ : state) {
		create_rapidstring(&s, str_24, 24);
	}

	benchmark::DoNotOptimize(s);
}

inline void std_24_byte_construct(benchmark::State &state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(std::string{ str_24, 24 });
}

inline void rs_48_byte_construct(benchmark::State &state)
{
	rapidstring s;

	for (auto _ : state) {
		create_rapidstring(&s, str_48, 48);
	}

	benchmark::DoNotOptimize(s);
}

inline void std_48_byte_construct(benchmark::State &state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(std::string{ str_48, 48 });
}

#endif /* !CONSTRUCT_HPP_7DFD4B503BE48168 */
