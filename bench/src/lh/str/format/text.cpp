#include <benchmark/benchmark.h>

#include <lh/str/format/text.h>

static void
BM_str_format_text_literal(benchmark::State &state)
{
    char buf[64];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_text(buf, sizeof(buf), "hello world"));
    }
}
BENCHMARK(BM_str_format_text_literal);

static void
BM_str_format_text_literal_long(benchmark::State &state)
{
    char buf[256];
    static const char fmt[] =
        "the quick brown fox jumps over the lazy dog, pack my box with five dozen liquor jugs";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_format_text(buf, sizeof(buf), fmt));
    }
}
BENCHMARK(BM_str_format_text_literal_long);

static void
BM_str_format_text_mixed_conversions(benchmark::State &state)
{
    char buf[64];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            lh_str_ptr_format_text(buf, sizeof(buf), "id=%u name=%s x=%x", 7U, "abc", 255U));
    }
}
BENCHMARK(BM_str_format_text_mixed_conversions);
