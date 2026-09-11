#include <benchmark/benchmark.h>

#include <lh/str/format/sint.h>

static void
BM_str_format_sint_1_digit(benchmark::State &state)
{
    char buf[32];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_sint(-7, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_str_format_sint_1_digit);

static void
BM_str_format_sint_10_digits(benchmark::State &state)
{
    char buf[32];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_sint(-2147483647, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_str_format_sint_10_digits);
