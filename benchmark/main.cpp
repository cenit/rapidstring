#include "concat.hpp"
#include "construct.hpp"
#include "resize.hpp"
#include "rpmalloc.h"
#include <benchmark/benchmark.h>

// Concatenation
BENCHMARK(rs_concat);
BENCHMARK(std_concat);

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

int main(int argc, char *argv[])
{
	benchmark::Initialize(&argc, argv); 

	if (benchmark::ReportUnrecognizedArguments(argc, argv))
		return 1;

	auto rc = rpmalloc_initialize();

	if (rc)
		return rc;

	benchmark::RunSpecifiedBenchmarks();

	rpmalloc_finalize();
}
