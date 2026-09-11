#include <benchmark/benchmark.h>

#include <lh/str/format/uint.h>

static void
BM_str_format_uint_1_digit(benchmark::State &state)
{
    char buf[32];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_uint(7, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_str_format_uint_1_digit);

static void
BM_str_format_uint_10_digits(benchmark::State &state)
{
    char buf[32];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_uint(4294967295U, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_str_format_uint_10_digits);
