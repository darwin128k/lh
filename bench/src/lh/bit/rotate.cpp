#include <benchmark/benchmark.h>

#include <lh/bit/rotate.h>

static void
BM_bit_rotate_left_u32(benchmark::State &state)
{
    lh_u32_t x = 0x12345678u;
    lh_usize_t r = 0;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        x = lh_bit_rotate_left_u32(x, (r++ & 31u) + 1u);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bit_rotate_left_u32);

static void
BM_bit_rotate_right_u32(benchmark::State &state)
{
    lh_u32_t x = 0x12345678u;
    lh_usize_t r = 0;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        x = lh_bit_rotate_right_u32(x, (r++ & 31u) + 1u);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bit_rotate_right_u32);

static void
BM_bit_rotate_left_u64(benchmark::State &state)
{
    lh_u64_t x = 0x0123456789ABCDEFull;
    lh_usize_t r = 0;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        x = lh_bit_rotate_left_u64(x, (r++ & 63u) + 1u);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bit_rotate_left_u64);

static void
BM_bit_rotate_right_u64(benchmark::State &state)
{
    lh_u64_t x = 0x0123456789ABCDEFull;
    lh_usize_t r = 0;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        x = lh_bit_rotate_right_u64(x, (r++ & 63u) + 1u);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bit_rotate_right_u64);
