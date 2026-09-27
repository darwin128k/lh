#include <benchmark/benchmark.h>

#include <lh/str/format/uint.h>

static void
BM_str_format_uint_1_digit(benchmark::State &state)
{
    char buf[32];
    lh_uint_t value = 7;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(lh_str_ptr_format_uint(value, buf, sizeof(buf)));
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_str_format_uint_1_digit);

static void
BM_str_format_uint_10_digits(benchmark::State &state)
{
    char buf[32];
    lh_uint_t value = 4294967295U;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(lh_str_ptr_format_uint(value, buf, sizeof(buf)));
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_str_format_uint_10_digits);
