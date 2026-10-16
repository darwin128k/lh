#include <benchmark/benchmark.h>

#include <lh/crypto/rijndael.h>
#include <lh/util/addr.h>

#include <cstring>

namespace
{

void
fill_bytes(lh_byte_t *dst, lh_usize_t n, lh_byte_t seed)
{
    for (lh_usize_t i = 0; i < n; ++i)
    {
        dst[i] = static_cast<lh_byte_t>(seed + static_cast<lh_byte_t>(i));
    }
}

} // namespace

static void
BM_crypto_rijndael_aes128_ecb_encrypt(benchmark::State &state)
{
    lh_byte_t key[16];
    lh_byte_t pt[16];
    lh_byte_t ct[16];
    lh_crypto_rijndael_t aes;

    fill_bytes(key, 16, 0x00);
    fill_bytes(pt, 16, 0xA0);
    lh_crypto_rijndael_init(lh_addr_of(aes), key, 16, lh_null, 0, 16, lh_crypto_rijndael_mode_ecb);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(pt);
        benchmark::DoNotOptimize(lh_crypto_rijndael_encrypt_block(lh_addr_of(aes), pt, ct));
        benchmark::DoNotOptimize(ct);
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * 16);
}
BENCHMARK(BM_crypto_rijndael_aes128_ecb_encrypt);

static void
BM_crypto_rijndael_aes128_ecb_decrypt(benchmark::State &state)
{
    lh_byte_t key[16];
    lh_byte_t pt[16];
    lh_byte_t ct[16];
    lh_byte_t back[16];
    lh_crypto_rijndael_t aes;

    fill_bytes(key, 16, 0x00);
    fill_bytes(pt, 16, 0xA0);
    lh_crypto_rijndael_init(lh_addr_of(aes), key, 16, lh_null, 0, 16, lh_crypto_rijndael_mode_ecb);
    lh_crypto_rijndael_encrypt_block(lh_addr_of(aes), pt, ct);
    lh_crypto_rijndael_init(lh_addr_of(aes), key, 16, lh_null, 0, 16, lh_crypto_rijndael_mode_ecb);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(ct);
        benchmark::DoNotOptimize(lh_crypto_rijndael_decrypt_block(lh_addr_of(aes), ct, back));
        benchmark::DoNotOptimize(back);
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * 16);
}
BENCHMARK(BM_crypto_rijndael_aes128_ecb_decrypt);

static void
BM_crypto_rijndael_aes256_ecb_encrypt(benchmark::State &state)
{
    lh_byte_t key[32];
    lh_byte_t pt[16];
    lh_byte_t ct[16];
    lh_crypto_rijndael_t aes;

    fill_bytes(key, 32, 0x01);
    fill_bytes(pt, 16, 0xB0);
    lh_crypto_rijndael_init(lh_addr_of(aes), key, 32, lh_null, 0, 16, lh_crypto_rijndael_mode_ecb);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(pt);
        benchmark::DoNotOptimize(lh_crypto_rijndael_encrypt_block(lh_addr_of(aes), pt, ct));
        benchmark::DoNotOptimize(ct);
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * 16);
}
BENCHMARK(BM_crypto_rijndael_aes256_ecb_encrypt);

static void
BM_crypto_rijndael_aes128_cbc_encrypt(benchmark::State &state)
{
    lh_byte_t key[16];
    lh_byte_t iv[16];
    lh_byte_t pt[16];
    lh_byte_t ct[16];
    lh_crypto_rijndael_t aes;

    fill_bytes(key, 16, 0x2B);
    fill_bytes(iv, 16, 0x00);
    fill_bytes(pt, 16, 0x6B);
    lh_crypto_rijndael_init(lh_addr_of(aes), key, 16, iv, 16, 16, lh_crypto_rijndael_mode_cbc);

    for (auto _ : state)
    {
        /* CBC mutates IV chain state — re-init each iteration for a stable block cost. */
        state.PauseTiming();
        lh_crypto_rijndael_init(lh_addr_of(aes), key, 16, iv, 16, 16, lh_crypto_rijndael_mode_cbc);
        state.ResumeTiming();
        benchmark::DoNotOptimize(pt);
        benchmark::DoNotOptimize(lh_crypto_rijndael_encrypt_block(lh_addr_of(aes), pt, ct));
        benchmark::DoNotOptimize(ct);
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * 16);
}
BENCHMARK(BM_crypto_rijndael_aes128_cbc_encrypt);

static void
BM_crypto_rijndael_aes128_ecb_init(benchmark::State &state)
{
    lh_byte_t key[16];
    lh_crypto_rijndael_t aes;

    fill_bytes(key, 16, 0x00);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(key);
        benchmark::DoNotOptimize(
            lh_crypto_rijndael_init(lh_addr_of(aes), key, 16, lh_null, 0, 16, lh_crypto_rijndael_mode_ecb));
        benchmark::DoNotOptimize(aes);
    }
}
BENCHMARK(BM_crypto_rijndael_aes128_ecb_init);

static void
BM_crypto_rijndael_aes256_ecb_init(benchmark::State &state)
{
    lh_byte_t key[32];
    lh_crypto_rijndael_t aes;

    fill_bytes(key, 32, 0x01);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(key);
        benchmark::DoNotOptimize(
            lh_crypto_rijndael_init(lh_addr_of(aes), key, 32, lh_null, 0, 16, lh_crypto_rijndael_mode_ecb));
        benchmark::DoNotOptimize(aes);
    }
}
BENCHMARK(BM_crypto_rijndael_aes256_ecb_init);
