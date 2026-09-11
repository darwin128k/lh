#include <benchmark/benchmark.h>

#include <lh/runtime/allocator.h>

static void
BM_runtime_allocator_alloc_free_64B(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_ptr p = lh_runtime_allocator_alloc(64);
        benchmark::DoNotOptimize(p);
        lh_runtime_allocator_free(p);
    }
}
BENCHMARK(BM_runtime_allocator_alloc_free_64B);

static void
BM_runtime_allocator_realloc_grow(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_ptr p = lh_runtime_allocator_alloc(64);
        p = lh_runtime_allocator_realloc(p, 64, 4096);
        benchmark::DoNotOptimize(p);
        lh_runtime_allocator_free(p);
    }
}
BENCHMARK(BM_runtime_allocator_realloc_grow);
