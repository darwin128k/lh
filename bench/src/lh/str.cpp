#include <benchmark/benchmark.h>

#include <lh/str.h>

namespace
{
constexpr lh_usize_t kCount = 1000;
}

static void
BM_str_append_grow(benchmark::State &state)
{
    const char chunk[] = "abcdefghij";
    for (auto _ : state)
    {
        lh_str_t s;
        lh_str_init(&s);
        for (lh_usize_t i = 0; i < kCount; ++i)
        {
            lh_str_append(&s, chunk, sizeof(chunk) - 1);
        }
        benchmark::DoNotOptimize(lh_str_get_size(&s));
        lh_str_deinit(&s);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_str_append_grow);

static void
BM_str_push_back_grow(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_str_t s;
        lh_str_init(&s);
        for (lh_usize_t i = 0; i < kCount; ++i)
        {
            lh_str_push_back(&s, 'x');
        }
        benchmark::DoNotOptimize(lh_str_get_size(&s));
        lh_str_deinit(&s);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_str_push_back_grow);

static void
BM_str_reserve_append(benchmark::State &state)
{
    const char chunk[] = "abcdefghij";
    for (auto _ : state)
    {
        lh_str_t s;
        lh_str_init(&s);
        lh_str_reserve(&s, kCount * (sizeof(chunk) - 1));
        for (lh_usize_t i = 0; i < kCount; ++i)
        {
            lh_str_append(&s, chunk, sizeof(chunk) - 1);
        }
        benchmark::DoNotOptimize(lh_str_get_size(&s));
        lh_str_deinit(&s);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_str_reserve_append);

static void
BM_str_clear_and_reuse(benchmark::State &state)
{
    lh_str_t s;
    lh_str_init(&s);
    lh_str_reserve(&s, 256);
    for (auto _ : state)
    {
        lh_str_clear(&s);
        lh_str_append(&s, "hello world", 11);
        benchmark::DoNotOptimize(lh_str_get_data(&s));
    }
    lh_str_deinit(&s);
}
BENCHMARK(BM_str_clear_and_reuse);
