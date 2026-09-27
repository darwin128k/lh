#include <benchmark/benchmark.h>

#include <lh/str/format/hex.h>

static void
BM_str_format_hex_1_digit(benchmark::State &state)
{
    char buf[32];
    lh_uint_t value = 0xF;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(lh_str_ptr_format_hex(value, lh_bool_false, buf, sizeof(buf)));
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_str_format_hex_1_digit);

static void
BM_str_format_hex_8_digits(benchmark::State &state)
{
    char buf[32];
    lh_uint_t value = 0xFFFFFFFFU;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(value);
        benchmark::DoNotOptimize(lh_str_ptr_format_hex(value, lh_bool_false, buf, sizeof(buf)));
        benchmark::DoNotOptimize(buf);
    }
}
BENCHMARK(BM_str_format_hex_8_digits);
