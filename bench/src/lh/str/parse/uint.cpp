#include <benchmark/benchmark.h>

#include <lh/numeric/limits.h>
#include <lh/str/parse/uint.h>

static void
BM_str_parse_uint_1_digit(benchmark::State &state)
{
    char text[] = "7";
    lh_uint_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(text);
        benchmark::DoNotOptimize(lh_str_ptr_parse_uint(text, sizeof(text) - 1, LH_UINT_T_MAX, &out));
    }
}
BENCHMARK(BM_str_parse_uint_1_digit);

static void
BM_str_parse_uint_10_digits(benchmark::State &state)
{
    char text[] = "4294967295";
    lh_uint_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(text);
        benchmark::DoNotOptimize(lh_str_ptr_parse_uint(text, sizeof(text) - 1, LH_UINT_T_MAX, &out));
    }
}
BENCHMARK(BM_str_parse_uint_10_digits);
