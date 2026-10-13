#include <benchmark/benchmark.h>

#include <lh/bit/endian.h>

static void
BM_bit_pack_unpack_be32(benchmark::State &state)
{
    lh_byte_t buf[4];
    lh_u32_t v = 0xA1B2C3D4u;
    for (auto _ : state)
    {
        lh_bit_pack_be32(v, buf);
        benchmark::DoNotOptimize(buf);
        v = lh_bit_unpack_be32(buf);
        benchmark::DoNotOptimize(v);
        v += 0x01010101u;
    }
}
BENCHMARK(BM_bit_pack_unpack_be32);

static void
BM_bit_pack_unpack_le32(benchmark::State &state)
{
    lh_byte_t buf[4];
    lh_u32_t v = 0xA1B2C3D4u;
    for (auto _ : state)
    {
        lh_bit_pack_le32(v, buf);
        benchmark::DoNotOptimize(buf);
        v = lh_bit_unpack_le32(buf);
        benchmark::DoNotOptimize(v);
        v += 0x01010101u;
    }
}
BENCHMARK(BM_bit_pack_unpack_le32);

static void
BM_bit_pack_unpack_be64(benchmark::State &state)
{
    lh_byte_t buf[8];
    lh_u64_t v = 0x0123456789ABCDEFull;
    for (auto _ : state)
    {
        lh_bit_pack_be64(v, buf);
        benchmark::DoNotOptimize(buf);
        v = lh_bit_unpack_be64(buf);
        benchmark::DoNotOptimize(v);
        v += 0x0101010101010101ull;
    }
}
BENCHMARK(BM_bit_pack_unpack_be64);

static void
BM_bit_pack_unpack_le16(benchmark::State &state)
{
    lh_byte_t buf[2];
    lh_u16_t v = 0xA1B2u;
    for (auto _ : state)
    {
        lh_bit_pack_le16(v, buf);
        benchmark::DoNotOptimize(buf);
        v = lh_bit_unpack_le16(buf);
        benchmark::DoNotOptimize(v);
        v = static_cast<lh_u16_t>(v + 0x0101u);
    }
}
BENCHMARK(BM_bit_pack_unpack_le16);
