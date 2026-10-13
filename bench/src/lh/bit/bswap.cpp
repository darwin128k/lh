#include <benchmark/benchmark.h>

#include <lh/bit/bswap.h>

static void
BM_bit_bswap_u16(benchmark::State &state)
{
    lh_u16_t x = 0x1234u;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        x = lh_bit_bswap_u16(x);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bit_bswap_u16);

static void
BM_bit_bswap_u32(benchmark::State &state)
{
    lh_u32_t x = 0x12345678u;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        x = lh_bit_bswap_u32(x);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bit_bswap_u32);

static void
BM_bit_bswap_u64(benchmark::State &state)
{
    lh_u64_t x = 0x0123456789ABCDEFull;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        x = lh_bit_bswap_u64(x);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bit_bswap_u64);
