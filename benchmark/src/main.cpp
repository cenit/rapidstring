#include "concat.hpp"
#include "construct.hpp"
#include "resize.hpp"
#include <benchmark/benchmark.h>

// TODO: add fbstring to benchmarks
// TODO: add intel compiler to benchmarks

// Concatenation
BENCHMARK(rs_concat);
BENCHMARK(std_concat);

BENCHMARK(rs_reserve_concat);
BENCHMARK(std_reserve_concat);

// Construction
BENCHMARK(rs_12_byte_construct);
BENCHMARK(std_12_byte_construct);

BENCHMARK(rs_24_byte_construct);
BENCHMARK(std_24_byte_construct);

BENCHMARK(rs_48_byte_construct);
BENCHMARK(std_48_byte_construct);

// Resizing
BENCHMARK(rs_resize);
BENCHMARK(std_resize);

BENCHMARK_MAIN();