#include <benchmark/benchmark.h>

#include <lh/bit/scan.h>

static void
BM_bit_scan_forward_u32(benchmark::State &state)
{
    lh_u32_t x = 0x00A50000u;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(lh_bit_scan_forward_u32(x));
        x = (x << 1) | (x >> 31);
    }
}
BENCHMARK(BM_bit_scan_forward_u32);

static void
BM_bit_scan_reverse_u32(benchmark::State &state)
{
    lh_u32_t x = 0x00A50000u;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(lh_bit_scan_reverse_u32(x));
        x = (x << 1) | (x >> 31);
    }
}
BENCHMARK(BM_bit_scan_reverse_u32);

static void
BM_bit_scan_forward_u64(benchmark::State &state)
{
    lh_u64_t x = 0x0000A50000000000ull;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(lh_bit_scan_forward_u64(x));
        x = (x << 1) | (x >> 63);
    }
}
BENCHMARK(BM_bit_scan_forward_u64);

static void
BM_bit_scan_reverse_u64(benchmark::State &state)
{
    lh_u64_t x = 0x0000A50000000000ull;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(lh_bit_scan_reverse_u64(x));
        x = (x << 1) | (x >> 63);
    }
}
BENCHMARK(BM_bit_scan_reverse_u64);

static void
BM_bit_scan_forward_u16(benchmark::State &state)
{
    lh_u16_t x = 0x0A50u;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(lh_bit_scan_forward_u16(x));
        x = static_cast<lh_u16_t>((x << 1) | (x >> 15));
    }
}
BENCHMARK(BM_bit_scan_forward_u16);

static void
BM_bit_scan_reverse_u8(benchmark::State &state)
{
    lh_u8_t x = 0xA5u;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(lh_bit_scan_reverse_u8(x));
        x = static_cast<lh_u8_t>((x << 1) | (x >> 7));
    }
}
BENCHMARK(BM_bit_scan_reverse_u8);
