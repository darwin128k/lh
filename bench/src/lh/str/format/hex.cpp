#include <benchmark/benchmark.h>

#include <lh/str/format/hex.h>

static void
BM_str_format_hex_1_digit(benchmark::State &state)
{
    char buf[32];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_hex(0xF, lh_bool_false, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_str_format_hex_1_digit);

static void
BM_str_format_hex_8_digits(benchmark::State &state)
{
    char buf[32];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_hex(0xFFFFFFFFU, lh_bool_false, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_str_format_hex_8_digits);
