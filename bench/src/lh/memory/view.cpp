#include <benchmark/benchmark.h>

#include <lh/memory/view.h>

#include <vector>

static void
BM_memory_view_get_size(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_view_t v = lh_memory_view_make_by_size(buf.data(), buf.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_view_get_size(&v));
    }
}
BENCHMARK(BM_memory_view_get_size);

static void
BM_memory_view_get_ptr_from_begin(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_view_t v = lh_memory_view_make_by_size(buf.data(), buf.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_view_get_ptr_from_begin(&v, 2048));
    }
}
BENCHMARK(BM_memory_view_get_ptr_from_begin);

static void
BM_memory_view_compare_4KB_equal(benchmark::State &state)
{
    std::vector<unsigned char> a(4096, 0x5A);
    std::vector<unsigned char> b(4096, 0x5A);
    lh_memory_view_t va = lh_memory_view_make_by_size(a.data(), a.size());
    lh_memory_view_t vb = lh_memory_view_make_by_size(b.data(), b.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_view_compare(&va, &vb));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * 4096);
}
BENCHMARK(BM_memory_view_compare_4KB_equal);

static void
BM_memory_view_find_byte(benchmark::State &state)
{
    std::vector<unsigned char> haystack(4096, 0x00);
    haystack.back() = 0xFF;
    unsigned char needle = 0xFF;
    lh_memory_view_t hv = lh_memory_view_make_by_size(haystack.data(), haystack.size());
    lh_memory_view_t nv = lh_memory_view_make_by_size(&needle, 1);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_view_find(&hv, &nv));
    }
}
BENCHMARK(BM_memory_view_find_byte);
