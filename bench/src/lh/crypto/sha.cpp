#include <benchmark/benchmark.h>

#include <lh/crypto/sha.h>

#include <vector>

namespace
{

struct ShaCase
{
    lh_crypto_sha_kind_t kind;
    lh_usize_t digest_size;
    const char *name;
};

const ShaCase kKinds[] = {
    {lh_crypto_sha_kind_1, LH_CRYPTO_SHA1_DIGEST_SIZE, "sha1"},
    {lh_crypto_sha_kind_224, LH_CRYPTO_SHA224_DIGEST_SIZE, "sha224"},
    {lh_crypto_sha_kind_256, LH_CRYPTO_SHA256_DIGEST_SIZE, "sha256"},
    {lh_crypto_sha_kind_384, LH_CRYPTO_SHA384_DIGEST_SIZE, "sha384"},
    {lh_crypto_sha_kind_512, LH_CRYPTO_SHA512_DIGEST_SIZE, "sha512"},
};

} // namespace

static void
BM_crypto_sha_hash(benchmark::State &state)
{
    const ShaCase &kind = kKinds[static_cast<lh_usize_t>(state.range(0))];
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(1));
    std::vector<unsigned char> data(n);
    for (lh_usize_t i = 0; i < n; ++i)
    {
        data[i] = static_cast<unsigned char>(i);
    }
    unsigned char out[LH_CRYPTO_SHA_DIGEST_SIZE_MAX];

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(data.data());
        benchmark::DoNotOptimize(
            lh_crypto_sha_hash(kind.kind, data.data(), n, out, kind.digest_size));
        benchmark::DoNotOptimize(out);
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(1));
    state.SetLabel(kind.name);
}
BENCHMARK(BM_crypto_sha_hash)
    ->ArgsProduct({{0, 1, 2, 3, 4}, {0, 64, 1024, 4096, 65536, 1024 * 1024}});

static void
BM_crypto_sha_update_finish(benchmark::State &state)
{
    const ShaCase &kind = kKinds[static_cast<lh_usize_t>(state.range(0))];
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(1));
    std::vector<unsigned char> data(n);
    for (lh_usize_t i = 0; i < n; ++i)
    {
        data[i] = static_cast<unsigned char>(i * 3);
    }
    unsigned char out[LH_CRYPTO_SHA_DIGEST_SIZE_MAX];
    lh_crypto_sha_t ctx;

    for (auto _ : state)
    {
        lh_crypto_sha_init(&ctx, kind.kind);
        lh_crypto_sha_update(&ctx, data.data(), n);
        benchmark::DoNotOptimize(lh_crypto_sha_finish(&ctx, out, kind.digest_size));
        benchmark::DoNotOptimize(out);
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(1));
    state.SetLabel(kind.name);
}
BENCHMARK(BM_crypto_sha_update_finish)->ArgsProduct({{0, 2, 4}, {64, 4096, 65536}});
