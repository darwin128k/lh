#include <benchmark/benchmark.h>

#include <lh/str/format/sint.h>

static void
BM_str_format_sint_1_digit(benchmark::State &state)
{
    char buf[32];
    lh_sint_t value = -7;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(lh_str_ptr_format_sint(value, buf, sizeof(buf)));
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_str_format_sint_1_digit);

static void
BM_str_format_sint_10_digits(benchmark::State &state)
{
    char buf[32];
    lh_sint_t value = -2147483647;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(lh_str_ptr_format_sint(value, buf, sizeof(buf)));
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_str_format_sint_10_digits);
