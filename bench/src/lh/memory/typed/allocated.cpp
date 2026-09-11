#include <benchmark/benchmark.h>

#include <lh/memory/typed.h>
#include <lh/memory/typed/allocated.h>

static void
BM_memory_typed_allocated_resize_1000_ints(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_memory_typed_allocated_t t;
        lh_memory_typed_init_empty(&t, sizeof(int));
        lh_memory_typed_allocated_resize(&t, 1000);
        benchmark::DoNotOptimize(t);
        lh_memory_typed_allocated_clear(&t);
    }
}
BENCHMARK(BM_memory_typed_allocated_resize_1000_ints);
