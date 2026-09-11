#include <benchmark/benchmark.h>

#include <lh/str/view.h>

static void
BM_str_view_find_char(benchmark::State &state)
{
    const char text[] = "the quick brown fox jumps over the lazy dog";
    lh_str_view_t v;
    lh_str_view_init(&v, text);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_view_find_char(&v, 'z'));
    }
}
BENCHMARK(BM_str_view_find_char);

static void
BM_str_view_find(benchmark::State &state)
{
    const char text[] = "the quick brown fox jumps over the lazy dog";
    const char needle_text[] = "lazy";
    lh_str_view_t haystack;
    lh_str_view_t needle;
    lh_str_view_init(&haystack, text);
    lh_str_view_init(&needle, needle_text);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_view_find(&haystack, &needle, lh_bool_false));
    }
}
BENCHMARK(BM_str_view_find);

static void
BM_str_view_equals(benchmark::State &state)
{
    const char text[] = "the quick brown fox jumps over the lazy dog";
    lh_str_view_t a;
    lh_str_view_t b;
    lh_str_view_init(&a, text);
    lh_str_view_init(&b, text);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_view_equals(&a, &b, lh_bool_false));
    }
}
BENCHMARK(BM_str_view_equals);

static void
BM_str_view_compare(benchmark::State &state)
{
    const char text[] = "the quick brown fox jumps over the lazy dog";
    lh_str_view_t a;
    lh_str_view_t b;
    lh_str_view_init(&a, text);
    lh_str_view_init(&b, text);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_view_compare(&a, &b, lh_bool_false));
    }
}
BENCHMARK(BM_str_view_compare);
