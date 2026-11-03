#include <benchmark/benchmark.h>

#include <lh/compress/gzip.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace
{

// Three inputs that pull the encoder apart: zeros are one long match after another,
// markup is the web host's case (short matches mixed with literals), random bytes find
// no match and end in the stored fallback.
enum InputKind
{
    kZeros = 0,
    kMarkup = 1,
    kRandom = 2,
};

const char *const kInputNames[] = {"zeros", "markup", "random"};

std::vector<unsigned char>
make_input(lh_usize_t kind, lh_usize_t n)
{
    std::vector<unsigned char> data(n);
    std::uint32_t seed = 1U;
    const char *const words[] = {"<div class=\"item\">", "lorem ",       "ipsum ", "</div>\n",
                                 "dolor sit ",           "{\"k\": 42}, "};
    lh_usize_t at = 0;
    while (kind != kZeros && at < n)
    {
        seed = seed * 1103515245U + 12345U;
        if (kind == kRandom)
        {
            data[at++] = static_cast<unsigned char>(seed >> 16);
            continue;
        }
        for (const char *w = words[(seed >> 16) % 6U]; *w != '\0' && at < n; ++w)
        {
            data[at++] = static_cast<unsigned char>(*w);
        }
    }
    return data;
}

} // namespace

static void
BM_compress_deflate_encode(benchmark::State &state)
{
    const lh_usize_t kind = static_cast<lh_usize_t>(state.range(0));
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(1));
    const std::vector<unsigned char> data = make_input(kind, n);
    lh_usize_t bound = 0;
    lh_compress_deflate_bound(n, &bound);
    std::vector<unsigned char> out(bound);
    std::unique_ptr<lh_compress_deflate_encoder_t> encoder(new lh_compress_deflate_encoder_t);
    lh_compress_deflate_encoder_init(encoder.get(), LH_COMPRESS_DEFLATE_MATCHER_CHAIN_LIMIT);
    lh_usize_t written = 0;

    for (auto _ : state)
    {
        lh_compress_deflate_encode(encoder.get(), data.data(), n, out.data(), bound, &written);
        benchmark::DoNotOptimize(out.data());
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(1));
    state.counters["ratio"] = static_cast<double>(written) / static_cast<double>(n);
    state.SetLabel(kInputNames[kind]);
}

BENCHMARK(BM_compress_deflate_encode)->ArgsProduct({{kZeros, kMarkup, kRandom}, {4096, 1 << 20}});

static void
BM_compress_deflate_decode(benchmark::State &state)
{
    const lh_usize_t kind = static_cast<lh_usize_t>(state.range(0));
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(1));
    const std::vector<unsigned char> data = make_input(kind, n);
    lh_usize_t bound = 0;
    lh_compress_deflate_bound(n, &bound);
    std::vector<unsigned char> packed(bound);
    std::unique_ptr<lh_compress_deflate_encoder_t> encoder(new lh_compress_deflate_encoder_t);
    lh_compress_deflate_encoder_init(encoder.get(), LH_COMPRESS_DEFLATE_MATCHER_CHAIN_LIMIT);
    lh_usize_t packed_size = 0;
    lh_compress_deflate_encode(encoder.get(), data.data(), n, packed.data(), bound, &packed_size);
    std::vector<unsigned char> out(n);
    lh_usize_t consumed = 0;
    lh_usize_t written = 0;

    for (auto _ : state)
    {
        lh_compress_deflate_decode(packed.data(), packed_size, out.data(), n, &consumed, &written);
        benchmark::DoNotOptimize(out.data());
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(1));
    state.SetLabel(kInputNames[kind]);
}

BENCHMARK(BM_compress_deflate_decode)->ArgsProduct({{kZeros, kMarkup, kRandom}, {4096, 1 << 20}});
