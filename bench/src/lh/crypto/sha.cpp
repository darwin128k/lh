#include <benchmark/benchmark.h>

#include <lh/crypto/sha.h>

static void
BM_crypto_sha256_64_bytes(benchmark::State &state)
{
    char data[64];
    unsigned char out[32];
    for (int i = 0; i < 64; ++i)
    {
        data[i] = static_cast<char>(i);
    }
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(
            lh_crypto_sha_hash(lh_crypto_sha_kind_256, data, 64, out, sizeof(out)));
    }
}
BENCHMARK(BM_crypto_sha256_64_bytes);
