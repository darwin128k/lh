#include <benchmark/benchmark.h>

#include <lh/vector.h>

namespace
{
constexpr lh_usize_t kCount = 1000;
}

static void
BM_vector_push_back_grow(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_vector_t v;
        lh_vector_init(&v, sizeof(int));
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_vector_push_back(&v, &value);
        }
        benchmark::DoNotOptimize(lh_vector_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_vector_push_back_grow);

static void
BM_vector_push_back_reserved(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_vector_t v;
        lh_vector_init(&v, sizeof(int));
        lh_vector_reserve(&v, kCount);
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_vector_push_back(&v, &value);
        }
        benchmark::DoNotOptimize(lh_vector_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_vector_push_back_reserved);

static void
BM_vector_get_ptr_random_access(benchmark::State &state)
{
    lh_vector_t v;
    lh_vector_init(&v, sizeof(int));
    lh_vector_reserve(&v, kCount);
    for (lh_usize_t n = 0; n < kCount; ++n)
    {
        int value = static_cast<int>(n);
        lh_vector_push_back(&v, &value);
    }

    lh_usize_t i = 0;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_vector_get_ptr(&v, i % kCount));
        ++i;
    }

    lh_memory_typed_allocated_clear(&v.typed);
}
BENCHMARK(BM_vector_get_ptr_random_access);
