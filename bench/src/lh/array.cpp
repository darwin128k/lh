#include <benchmark/benchmark.h>

#include <lh/array.h>

namespace
{
constexpr lh_usize_t kCount = 1000;
}

static void
BM_array_push_back_grow(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_array_t v;
        lh_array_init(&v, sizeof(int));
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_array_push_back(&v, &value);
        }
        benchmark::DoNotOptimize(lh_array_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_array_push_back_grow);

static void
BM_array_push_back_reserved(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_array_t v;
        lh_array_init(&v, sizeof(int));
        lh_array_reserve(&v, kCount);
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_array_push_back(&v, &value);
        }
        benchmark::DoNotOptimize(lh_array_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_array_push_back_reserved);

static void
BM_array_get_ptr_random_access(benchmark::State &state)
{
    lh_array_t v;
    lh_array_init(&v, sizeof(int));
    lh_array_reserve(&v, kCount);
    for (lh_usize_t n = 0; n < kCount; ++n)
    {
        int value = static_cast<int>(n);
        lh_array_push_back(&v, &value);
    }

    lh_usize_t i = 0;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_array_get_ptr(&v, i % kCount));
        ++i;
    }

    lh_memory_typed_allocated_clear(&v.typed);
}
BENCHMARK(BM_array_get_ptr_random_access);

namespace
{

lh_int_t
int_cmp(const lh_ptr a, const lh_ptr b, lh_ptr /*context*/)
{
    const int va = *static_cast<const int *>(a);
    const int vb = *static_cast<const int *>(b);
    return va < vb ? -1 : va > vb ? 1 : 0;
}

} // namespace

static void
BM_array_insert_front(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_array_t v;
        lh_array_init(&v, sizeof(int));
        lh_array_reserve(&v, kCount);
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_array_insert(&v, 0, &value);
        }
        benchmark::DoNotOptimize(lh_array_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_array_insert_front);

static void
BM_array_erase_front(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        lh_array_t v;
        lh_array_init(&v, sizeof(int));
        lh_array_reserve(&v, kCount);
        for (lh_usize_t n = 0; n < kCount; ++n)
        {
            int value = static_cast<int>(n);
            lh_array_push_back(&v, &value);
        }
        state.ResumeTiming();

        while (lh_array_get_size(&v) > 0)
        {
            lh_array_erase(&v, 0, nullptr);
        }
        benchmark::DoNotOptimize(lh_array_get_size(&v));
        lh_memory_typed_allocated_clear(&v.typed);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_array_erase_front);

static void
BM_array_sort(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    for (auto _ : state)
    {
        state.PauseTiming();
        lh_array_t v;
        lh_array_init(&v, sizeof(int));
        lh_array_reserve(&v, n);
        for (lh_usize_t i = 0; i < n; ++i)
        {
            int value = static_cast<int>((n - i) * 2654435761u);
            lh_array_push_back(&v, &value);
        }
        state.ResumeTiming();

        lh_array_sort(&v, int_cmp, nullptr);
        benchmark::DoNotOptimize(lh_array_get_ptr(&v, 0));
        lh_memory_typed_allocated_clear(&v.typed);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_array_sort)->Arg(100)->Arg(1000)->Arg(10000);
