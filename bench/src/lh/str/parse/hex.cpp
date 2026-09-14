#include <benchmark/benchmark.h>

#include <lh/numeric/limits.h>
#include <lh/str/parse/hex.h>

static void
BM_str_parse_hex_2_digits(benchmark::State &state)
{
    char text[] = "ff";
    lh_uint_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(text);
        benchmark::DoNotOptimize(lh_str_ptr_parse_hex(text, sizeof(text) - 1, LH_UINT_T_MAX, &out));
    }
}
BENCHMARK(BM_str_parse_hex_2_digits);

static void
BM_str_parse_hex_8_digits(benchmark::State &state)
{
    char text[] = "ffffffff";
    lh_uint_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(text);
        benchmark::DoNotOptimize(lh_str_ptr_parse_hex(text, sizeof(text) - 1, LH_UINT_T_MAX, &out));
    }
}
BENCHMARK(BM_str_parse_hex_8_digits);
