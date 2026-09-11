#include <benchmark/benchmark.h>

#include <lh/memory/std.h>

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
BENCHMARK(BM_memory_std_copy)->Arg(64)->Arg(4096)->Arg(1024 * 1024);

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
BENCHMARK(BM_memory_std_set)->Arg(4096);

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
BENCHMARK(BM_memory_std_compare_equal)->Arg(4096);

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
