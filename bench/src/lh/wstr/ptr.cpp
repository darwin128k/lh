#include <benchmark/benchmark.h>

#include <lh/util/wstr/ptr.h>

#include <cwchar>

static void
BM_wstr_ptr_len(benchmark::State &state)
{
    const lh_wchar_t text[] = L"the quick brown fox jumps over the lazy dog";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_wstr_ptr_len(text));
    }
}
BENCHMARK(BM_wstr_ptr_len);

static void
BM_wstr_ptr_find(benchmark::State &state)
{
    const lh_wchar_t text[] = L"the quick brown fox jumps over the lazy dog";
    const lh_wchar_t needle[] = L"lazy";
    const lh_usize_t text_len = lh_wstr_ptr_len(text);
    const lh_usize_t needle_len = lh_wstr_ptr_len(needle);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_wstr_ptr_find(text, text_len, needle, needle_len, lh_bool_false));
    }
}
BENCHMARK(BM_wstr_ptr_find);

static void
BM_wstr_ptr_equals(benchmark::State &state)
{
    const lh_wchar_t a[] = L"the quick brown fox jumps over the lazy dog";
    const lh_wchar_t b[] = L"the quick brown fox jumps over the lazy dog";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_wstr_ptr_equals(a, b, lh_bool_false));
    }
}
BENCHMARK(BM_wstr_ptr_equals);
