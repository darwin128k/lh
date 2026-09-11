#include <benchmark/benchmark.h>

#include <lh/memory/bounds/allocated.h>
#include <lh/memory/bounds/allocated/initializer.h>

static void
BM_memory_bounds_allocated_resize_4KB(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
        lh_memory_bounds_allocated_resize(&a, 4096);
        benchmark::DoNotOptimize(a);
        lh_memory_bounds_allocated_clear(&a);
    }
}
BENCHMARK(BM_memory_bounds_allocated_resize_4KB);

static void
BM_memory_bounds_allocated_exchange(benchmark::State &state)
{
    // Not a symmetric swap: self takes other's block and other is left empty (via
    // swap_and_clear) — re-provision both every iteration rather than exchanging the
    // same pair repeatedly, since the second call would see an already-empty `other`.
    for (auto _ : state)
    {
        lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
        lh_memory_bounds_allocated_resize(&a, 64);
        lh_memory_bounds_allocated_t b = lh_memory_bounds_allocated_initializer();
        lh_memory_bounds_allocated_resize(&b, 128);
        lh_memory_bounds_allocated_exchange(&a, &b);
        benchmark::DoNotOptimize(a);
        lh_memory_bounds_allocated_clear(&a);
        lh_memory_bounds_allocated_clear(&b);
    }
}
BENCHMARK(BM_memory_bounds_allocated_exchange);
