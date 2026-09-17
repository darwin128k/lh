#include <benchmark/benchmark.h>

#include <lh/time.h>
#include <lh/time/initializer.h>
#include <lh/util/addr.h>

static void
BM_time_parse(benchmark::State &state)
{
    const char text[] = "09:05:00";
    lh_time_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_time_parse(text, sizeof(text) - 1, lh_addr_of(out)));
    }
}
BENCHMARK(BM_time_parse);

static void
BM_time_format(benchmark::State &state)
{
    lh_time_t time = lh_time_initializer(9, 5, 0);
    char buf[LH_TIME_TEXT_MAX];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_time_format(lh_addr_of(time), buf, sizeof(buf)));
    }
}
BENCHMARK(BM_time_format);

static void
BM_time_add_second(benchmark::State &state)
{
    lh_time_t time = lh_time_initializer(9, 5, 0);
    for (auto _ : state)
    {
        lh_time_t cur = time;
        benchmark::DoNotOptimize(lh_time_add_second(lh_addr_of(cur), 1U));
        benchmark::DoNotOptimize(cur);
    }
}
BENCHMARK(BM_time_add_second);
