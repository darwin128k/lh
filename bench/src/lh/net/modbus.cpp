#include <benchmark/benchmark.h>

#include <lh/net/modbus.h>

static void
BM_mb_crc16(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    lh_byte_t bytes[256];
    for (lh_usize_t i = 0; i < n && i < sizeof(bytes); ++i)
    {
        bytes[i] = static_cast<lh_byte_t>(i);
    }
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_mb_crc16(bytes, static_cast<lh_u16_t>(n)));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_mb_crc16)->Arg(8)->Arg(64)->Arg(256);

static void
BM_mb_build_read(benchmark::State &state)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            lh_mb_build_read(adu, LH_MB_ADU_MAX, 1, LH_MB_FC_READ_HOLDING, 0x0010, 3));
        benchmark::DoNotOptimize(adu);
    }
}
BENCHMARK(BM_mb_build_read);

static void
BM_mb_build_write(benchmark::State &state)
{
    lh_byte_t adu[LH_MB_ADU_MAX];
    const lh_u16_t values[8] = {0x1234, 0xABCD, 0x0001, 0xFFFF, 0x1111, 0x2222, 0x3333, 0x4444};
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_mb_build_write(adu, LH_MB_ADU_MAX, 1, 0x0020, values, 8));
        benchmark::DoNotOptimize(adu);
    }
}
BENCHMARK(BM_mb_build_write);

static void
BM_mb_plan_reads(benchmark::State &state)
{
    const lh_mb_want_t wants[] = {
        {0x0000, 10}, {0x0010, 5}, {0x0020, 20}, {0x0100, 3}, {0x0200, 15},
    };
    lh_mb_read_t reads[16];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            lh_mb_plan_reads(wants, static_cast<lh_u16_t>(sizeof(wants) / sizeof(wants[0])), reads,
                             static_cast<lh_u16_t>(sizeof(reads) / sizeof(reads[0]))));
        benchmark::DoNotOptimize(reads);
    }
}
BENCHMARK(BM_mb_plan_reads);
