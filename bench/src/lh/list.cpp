#include <benchmark/benchmark.h>

#include <lh/list.h>

#include <vector>

namespace
{

struct Item
{
    int value;
    lh_list_node_t node;
};

lh_int_t
by_value(const lh_list_node_t *a, const lh_list_node_t *b, lh_ptr /*context*/)
{
    const int va = lh_list_entry(Item, node, a)->value;
    const int vb = lh_list_entry(Item, node, b)->value;
    return va < vb ? -1 : va > vb ? 1 : 0;
}

constexpr lh_usize_t kCount = 1000;

} // namespace

static void
BM_list_push_back(benchmark::State &state)
{
    std::vector<Item> items(kCount);
    for (lh_usize_t i = 0; i < kCount; ++i)
    {
        items[i].value = static_cast<int>(i);
        lh_list_node_init(&items[i].node);
    }

    for (auto _ : state)
    {
        lh_list_t list;
        lh_list_init(&list);
        for (lh_usize_t i = 0; i < kCount; ++i)
        {
            lh_list_node_init(&items[i].node);
            lh_list_push_back(&list, &items[i].node);
        }
        benchmark::DoNotOptimize(lh_list_get_size(&list));
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_list_push_back);

static void
BM_list_push_front(benchmark::State &state)
{
    std::vector<Item> items(kCount);
    for (lh_usize_t i = 0; i < kCount; ++i)
    {
        items[i].value = static_cast<int>(i);
        lh_list_node_init(&items[i].node);
    }

    for (auto _ : state)
    {
        lh_list_t list;
        lh_list_init(&list);
        for (lh_usize_t i = 0; i < kCount; ++i)
        {
            lh_list_node_init(&items[i].node);
            lh_list_push_front(&list, &items[i].node);
        }
        benchmark::DoNotOptimize(lh_list_get_size(&list));
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_list_push_front);

static void
BM_list_traverse_forward(benchmark::State &state)
{
    std::vector<Item> items(kCount);
    lh_list_t list;
    lh_list_init(&list);
    for (lh_usize_t i = 0; i < kCount; ++i)
    {
        items[i].value = static_cast<int>(i);
        lh_list_node_init(&items[i].node);
        lh_list_push_back(&list, &items[i].node);
    }

    for (auto _ : state)
    {
        int sum = 0;
        for (lh_list_node_t *n = lh_list_get_first(&list); n; n = lh_list_get_next(&list, n))
        {
            sum += lh_list_entry(Item, node, n)->value;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_list_traverse_forward);

static void
BM_list_sort(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    std::vector<Item> items(n);

    for (auto _ : state)
    {
        state.PauseTiming();
        lh_list_t list;
        lh_list_init(&list);
        for (lh_usize_t i = 0; i < n; ++i)
        {
            items[i].value = static_cast<int>((n - i) * 2654435761u);
            lh_list_node_init(&items[i].node);
            lh_list_push_back(&list, &items[i].node);
        }
        state.ResumeTiming();

        lh_list_sort(&list, by_value, nullptr);
        benchmark::DoNotOptimize(lh_list_get_first(&list));
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_list_sort)->Arg(100)->Arg(1000)->Arg(10000);

static void
BM_list_insert_sorted(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    std::vector<Item> items(n);

    for (auto _ : state)
    {
        lh_list_t list;
        lh_list_init(&list);
        for (lh_usize_t i = 0; i < n; ++i)
        {
            items[i].value = static_cast<int>((i * 2654435761u) % 10007);
            lh_list_node_init(&items[i].node);
            lh_list_insert_sorted(&list, &items[i].node, by_value, nullptr);
        }
        benchmark::DoNotOptimize(lh_list_get_size(&list));
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_list_insert_sorted)->Arg(100)->Arg(1000);

static void
BM_list_pop_front_drain(benchmark::State &state)
{
    std::vector<Item> items(kCount);

    for (auto _ : state)
    {
        state.PauseTiming();
        lh_list_t list;
        lh_list_init(&list);
        for (lh_usize_t i = 0; i < kCount; ++i)
        {
            items[i].value = static_cast<int>(i);
            lh_list_node_init(&items[i].node);
            lh_list_push_back(&list, &items[i].node);
        }
        state.ResumeTiming();

        while (!lh_list_is_empty(&list))
        {
            benchmark::DoNotOptimize(lh_list_pop_front(&list));
        }
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * kCount);
}
BENCHMARK(BM_list_pop_front_drain);
