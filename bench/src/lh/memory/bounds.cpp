#include <benchmark/benchmark.h>

#include <lh/memory/bounds.h>

#include <vector>

static void
BM_memory_bounds_init_by_size(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_bounds_t b;
    unsigned char *data = buf.data();
    lh_usize_t size = buf.size();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(size);
        lh_memory_bounds_init_by_size(&b, data, size);
        benchmark::DoNotOptimize(b);
    }
}
BENCHMARK(BM_memory_bounds_init_by_size);

static void
BM_memory_bounds_get_size(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_bounds_t b = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), buf.data(); _lh_tmp; }), buf.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(lh_memory_bounds_get_size(&b));
    }
}
BENCHMARK(BM_memory_bounds_get_size);

static void
BM_memory_bounds_get_ptr_from_begin(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_bounds_t b = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), buf.data(); _lh_tmp; }), buf.size());
    lh_usize_t index = 2048;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(index);
        benchmark::DoNotOptimize(lh_memory_bounds_get_ptr_from_begin(&b, index));
    }
}
BENCHMARK(BM_memory_bounds_get_ptr_from_begin);

static void
BM_memory_bounds_is_valid(benchmark::State &state)
{
    std::vector<unsigned char> buf(4096);
    lh_memory_bounds_t b = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), buf.data(); _lh_tmp; }), buf.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(lh_memory_bounds_is_valid(&b));
    }
}
BENCHMARK(BM_memory_bounds_is_valid);

static void
BM_memory_bounds_copy_4KB(benchmark::State &state)
{
    std::vector<unsigned char> src(4096, 0x5A);
    std::vector<unsigned char> dst(4096, 0);
    lh_memory_bounds_t s = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), src.data(); _lh_tmp; }), src.size());
    lh_memory_bounds_t d = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), dst.data(); _lh_tmp; }), dst.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_bounds_copy(&d, &s));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * 4096);
}
BENCHMARK(BM_memory_bounds_copy_4KB);

static void
BM_memory_bounds_compare_4KB_equal(benchmark::State &state)
{
    std::vector<unsigned char> a(4096, 0x5A);
    std::vector<unsigned char> b(4096, 0x5A);
    lh_memory_bounds_t ba = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), a.data(); _lh_tmp; }), a.size());
    lh_memory_bounds_t bb = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), b.data(); _lh_tmp; }), b.size());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_bounds_compare(&ba, &bb));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * 4096);
}
BENCHMARK(BM_memory_bounds_compare_4KB_equal);

static void
BM_memory_bounds_find_byte(benchmark::State &state)
{
    std::vector<unsigned char> haystack(4096, 0x00);
    haystack.back() = 0xFF;
    unsigned char needle = 0xFF;
    lh_memory_bounds_t hb = ({ lh_memory_bounds_t _lh_tmp; lh_memory_bounds_init_by_size(lh_addr_of(_lh_tmp), haystack.data(); _lh_tmp; }), haystack.size());
    lh_memory_bounds_t nb;

    lh_memory_bounds_init_by_size(lh_addr_of(nb), &needle, 1);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_bounds_find(&hb, &nb));
    }
}
BENCHMARK(BM_memory_bounds_find_byte);
