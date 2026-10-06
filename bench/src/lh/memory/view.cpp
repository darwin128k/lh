#include <benchmark/benchmark.h>

#include <lh/memory/view.h>

#include <vector>

static void
BM_memory_view_get_size(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_view_t v = ({ lh_memory_view_t _lh_tmp; lh_memory_view_init_by_size(lh_addr_of(_lh_tmp), buf.data(); _lh_tmp; }), buf.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v);
        benchmark::DoNotOptimize(lh_memory_view_get_size(&v));
    }
}
BENCHMARK(BM_memory_view_get_size);

static void
BM_memory_view_get_ptr_from_begin(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_view_t v = ({ lh_memory_view_t _lh_tmp; lh_memory_view_init_by_size(lh_addr_of(_lh_tmp), buf.data(); _lh_tmp; }), buf.size());
    lh_usize_t index = 2048;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v);
        benchmark::DoNotOptimize(index);
        benchmark::DoNotOptimize(lh_memory_view_get_ptr_from_begin(&v, index));
    }
}
BENCHMARK(BM_memory_view_get_ptr_from_begin);

static void
BM_memory_view_compare_4KB_equal(benchmark::State &state)
{
    std::vector<unsigned char> a(4096, 0x5A);
    std::vector<unsigned char> b(4096, 0x5A);
    lh_memory_view_t va = ({ lh_memory_view_t _lh_tmp; lh_memory_view_init_by_size(lh_addr_of(_lh_tmp), a.data(); _lh_tmp; }), a.size());
    lh_memory_view_t vb = ({ lh_memory_view_t _lh_tmp; lh_memory_view_init_by_size(lh_addr_of(_lh_tmp), b.data(); _lh_tmp; }), b.size());
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
    lh_memory_view_t hv = ({ lh_memory_view_t _lh_tmp; lh_memory_view_init_by_size(lh_addr_of(_lh_tmp), haystack.data(); _lh_tmp; }), haystack.size());
    lh_memory_view_t nv;

    lh_memory_view_init_by_size(lh_addr_of(nv), &needle, 1);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_view_find(&hv, &nv));
    }
}
BENCHMARK(BM_memory_view_find_byte);
