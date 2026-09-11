#include <benchmark/benchmark.h>

#include <lh/memory/typed.h>
#include <lh/memory/typed/initializer.h>

#include <vector>

static void
BM_memory_typed_get_size(benchmark::State &state)
{
    std::vector<int> buf(1024);
    lh_memory_typed_t t =
        lh_memory_typed_initializer(buf.data(), buf.data() + buf.size(), sizeof(int));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_typed_get_size(&t));
    }
}
BENCHMARK(BM_memory_typed_get_size);

static void
BM_memory_typed_get_ptr_from_begin(benchmark::State &state)
{
    std::vector<int> buf(1024);
    lh_memory_typed_t t =
        lh_memory_typed_initializer(buf.data(), buf.data() + buf.size(), sizeof(int));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_typed_get_ptr_from_begin(&t, 512));
    }
}
BENCHMARK(BM_memory_typed_get_ptr_from_begin);

static void
BM_memory_typed_is_valid_index(benchmark::State &state)
{
    std::vector<int> buf(1024);
    lh_memory_typed_t t =
        lh_memory_typed_initializer(buf.data(), buf.data() + buf.size(), sizeof(int));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_typed_is_valid_index(&t, 512));
    }
}
BENCHMARK(BM_memory_typed_is_valid_index);

static void
BM_memory_typed_set_value(benchmark::State &state)
{
    std::vector<int> buf(1024, 0);
    lh_memory_typed_t t =
        lh_memory_typed_initializer(buf.data(), buf.data() + buf.size(), sizeof(int));
    int value = 42;
    for (auto _ : state)
    {
        lh_memory_typed_set_value(&t, 512, &value);
    }
}
BENCHMARK(BM_memory_typed_set_value);

static void
BM_memory_typed_set_values_64(benchmark::State &state)
{
    std::vector<int> buf(1024, 0);
    std::vector<int> values(64, 7);
    lh_memory_typed_t t =
        lh_memory_typed_initializer(buf.data(), buf.data() + buf.size(), sizeof(int));
    for (auto _ : state)
    {
        lh_memory_typed_set_values(&t, 100, values.data(), values.size());
    }
}
BENCHMARK(BM_memory_typed_set_values_64);
