#include <benchmark/benchmark.h>

#include <lh/str/format/bytes.h>

static void
BM_str_format_bytes_hex_32(benchmark::State &state)
{
    unsigned char data[32];
    char out[64];
    for (int i = 0; i < 32; ++i)
    {
        data[i] = static_cast<unsigned char>(i);
    }
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(
            lh_str_ptr_format_bytes_hex(data, 32, 0, out, sizeof(out)));
    }
}
BENCHMARK(BM_str_format_bytes_hex_32);
