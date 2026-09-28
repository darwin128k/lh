#include <benchmark/benchmark.h>

#include <lh/memory.h>
#include <lh/memory/std.h>

#include <algorithm>
#include <cstring>
#include <vector>

namespace
{

struct Buffers
{
    std::vector<unsigned char> src;
    std::vector<unsigned char> dst;

    explicit Buffers(lh_usize_t size) : src(size, 0x5A), dst(size, 0)
    {
    }
};

} // namespace

static void
BM_memory_std_copy(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_std_copy(bufs.dst.data(), bufs.src.data(), bufs.src.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_copy)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1024)
    ->Arg(2048)
    ->Arg(4096)
    ->Arg(8192)
    ->Arg(65536)
    ->Arg(262144)
    ->Arg(1024 * 1024)
    ->Arg(2 * 1024 * 1024)
    ->Arg(4 * 1024 * 1024)
    ->Arg(8 * 1024 * 1024)
    ->Arg(16 * 1024 * 1024);

static void
BM_memory_std_rcopy(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_std_rcopy(bufs.dst.data(), bufs.src.data(), bufs.src.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_rcopy)->Arg(64)->Arg(4096)->Arg(65536)->Arg(1024 * 1024);

static void
BM_memory_std_copy_rev(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_std_copy_rev(bufs.dst.data(), bufs.src.data(), bufs.src.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_copy_rev)->Arg(64)->Arg(4096)->Arg(65536)->Arg(1024 * 1024);

// Comparison baseline: the platform CRT's own memcpy, interleaved with the run above so
// both see the same CPU boost/power state at each size.
static void
BM_crt_memcpy(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(std::memcpy(bufs.dst.data(), bufs.src.data(), bufs.src.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_crt_memcpy)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1024)
    ->Arg(2048)
    ->Arg(4096)
    ->Arg(8192)
    ->Arg(65536)
    ->Arg(262144)
    ->Arg(1024 * 1024)
    ->Arg(2 * 1024 * 1024)
    ->Arg(4 * 1024 * 1024)
    ->Arg(8 * 1024 * 1024)
    ->Arg(16 * 1024 * 1024);

static void
BM_memory_std_set(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_std_set(bufs.dst.data(), 0x33, bufs.dst.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_set)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1024)
    ->Arg(2048)
    ->Arg(4096)
    ->Arg(8192)
    ->Arg(65536)
    ->Arg(262144)
    ->Arg(1024 * 1024)
    ->Arg(2 * 1024 * 1024)
    ->Arg(4 * 1024 * 1024)
    ->Arg(8 * 1024 * 1024)
    ->Arg(16 * 1024 * 1024);

// Comparison baseline: the platform CRT's own memset, interleaved with the run above so
// both see the same CPU boost/power state at each size.
static void
BM_crt_memset(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(std::memset(bufs.dst.data(), 0x33, bufs.dst.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_crt_memset)
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1024)
    ->Arg(2048)
    ->Arg(4096)
    ->Arg(8192)
    ->Arg(65536)
    ->Arg(262144)
    ->Arg(1024 * 1024)
    ->Arg(2 * 1024 * 1024)
    ->Arg(4 * 1024 * 1024)
    ->Arg(8 * 1024 * 1024)
    ->Arg(16 * 1024 * 1024);

static void
BM_memory_std_compare_equal(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    bufs.dst = bufs.src; // worst case: no mismatch, every byte compared
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_std_compare(bufs.dst.data(), bufs.src.data(), bufs.dst.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_compare_equal)->Arg(4096)->Arg(65536)->Arg(1024 * 1024);

// Comparison baseline: the platform CRT's own memcmp.
static void
BM_crt_memcmp(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    bufs.dst = bufs.src;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(std::memcmp(bufs.dst.data(), bufs.src.data(), bufs.dst.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_crt_memcmp)->Arg(4096)->Arg(65536)->Arg(1024 * 1024);

static void
BM_memory_std_compare_mismatch_at_start(benchmark::State &state)
{
    // dst starts at 0, src at 0x5A: differs at byte 0 — best case, not representative
    // of a full scan; see BM_memory_std_compare_equal for the worst case.
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_std_compare(bufs.dst.data(), bufs.src.data(), bufs.dst.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_compare_mismatch_at_start)->Arg(4096);

static void
BM_memory_std_rcompare_equal(benchmark::State &state)
{
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    bufs.dst = bufs.src; // worst case: no mismatch, every byte compared
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            lh_memory_std_rcompare(bufs.dst.data(), bufs.src.data(), bufs.dst.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_rcompare_equal)->Arg(4096);

static void
BM_memory_std_rcompare_mismatch_at_end(benchmark::State &state)
{
    // dst starts at 0, src at 0x5A: differs at the last byte — best case for a
    // reverse scan, not representative of a full scan; see the _equal case above.
    Buffers bufs(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            lh_memory_std_rcompare(bufs.dst.data(), bufs.src.data(), bufs.dst.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_rcompare_mismatch_at_end)->Arg(4096);

// Substring search over English-like text, 8-byte needle at the very end: the
// whole haystack is scanned. Letters repeat often enough that the needle's first
// byte alone would pass about every 20th position.
static void
BM_memory_std_find(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    const char words[] = "the quick brown fox jumps over the lazy dog ";
    std::vector<unsigned char> hay(n);
    for (lh_usize_t i = 0; i < n; ++i)
    {
        hay[i] = static_cast<unsigned char>(words[i % (sizeof(words) - 1)]);
    }
    const unsigned char needle[] = {'l', 'a', 'z', 'y', '_', 'c', 'a', 't'};
    std::copy(needle, needle + sizeof(needle), hay.end() - sizeof(needle));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_std_find(hay.data(), n, needle, sizeof(needle)));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_std_find)->Arg(64)->Arg(1024)->Arg(65536);

// The pre-SIMD equivalent through the public find: same data, same needle.
static void
BM_memory_find_substring(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    const char words[] = "the quick brown fox jumps over the lazy dog ";
    std::vector<unsigned char> hay(n);
    for (lh_usize_t i = 0; i < n; ++i)
    {
        hay[i] = static_cast<unsigned char>(words[i % (sizeof(words) - 1)]);
    }
    const unsigned char needle[] = {'l', 'a', 'z', 'y', '_', 'c', 'a', 't'};
    std::copy(needle, needle + sizeof(needle), hay.end() - sizeof(needle));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_find(hay.data(), n, needle, sizeof(needle)));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_find_substring)->Arg(64)->Arg(1024)->Arg(65536);

// Pattern fill: range(0) bytes of destination, range(1)-byte pattern (2 = a wide
// character, as lh_wstr_ptr fills it).
static void
BM_memory_set_pattern(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    const lh_usize_t m = static_cast<lh_usize_t>(state.range(1));
    std::vector<unsigned char> dst(n);
    const unsigned char pat[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_set_pattern(dst.data(), n, pat, m));
        benchmark::DoNotOptimize(dst.data());
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_set_pattern)->ArgsProduct({{64, 4096}, {2, 4, 8}});
