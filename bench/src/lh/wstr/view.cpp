#include <benchmark/benchmark.h>

#include <lh/wstr/view.h>

static void
BM_wstr_view_init(benchmark::State &state)
{
    const lh_wchar_t text[] = L"the quick brown fox jumps over the lazy dog";
    for (auto _ : state)
    {
        lh_wstr_view_t v;
        lh_wstr_view_init(lh_addr_of(v), text);
        benchmark::DoNotOptimize(lh_wstr_view_get_size(lh_addr_of(v)));
    }
}
BENCHMARK(BM_wstr_view_init);

static void
BM_wstr_view_from_offset(benchmark::State &state)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"key=value;path=/usr/local/bin");
    lh_usize_t i = 0;
    for (auto _ : state)
    {
        lh_wstr_view_t mid = lh_wstr_view_from_offset(&v, i % 10, 4);
        benchmark::DoNotOptimize(lh_wstr_view_get_size(&mid));
        ++i;
    }
}
BENCHMARK(BM_wstr_view_from_offset);

static void
BM_wstr_view_tail(benchmark::State &state)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"key=value;path=/usr/local/bin");
    lh_usize_t i = 0;
    for (auto _ : state)
    {
        lh_wstr_view_t tail = lh_wstr_view_tail(&v, i % lh_wstr_view_get_size(&v));
        benchmark::DoNotOptimize(lh_wstr_view_get_size(&tail));
        ++i;
    }
}
BENCHMARK(BM_wstr_view_tail);

static void
BM_wstr_view_get_size(benchmark::State &state)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"the quick brown fox jumps over the lazy dog");
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_wstr_view_get_size(&v));
    }
}
BENCHMARK(BM_wstr_view_get_size);
