#include <benchmark/benchmark.h>

#include <lh/date.h>
#include <lh/date/initializer.h>
#include <lh/util/addr.h>

static void
BM_date_parse(benchmark::State &state)
{
    const char text[] = "2026/09/18";
    lh_date_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_date_parse(text, sizeof(text) - 1, lh_addr_of(out)));
    }
}
BENCHMARK(BM_date_parse);

static void
BM_date_format(benchmark::State &state)
{
    lh_date_t date = lh_date_initializer(2026, 9, 18);
    char buf[LH_DATE_TEXT_MAX];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_date_format(lh_addr_of(date), buf, sizeof(buf)));
    }
}
BENCHMARK(BM_date_format);

static void
BM_date_add_day(benchmark::State &state)
{
    lh_date_t date = lh_date_initializer(2026, 9, 18);
    for (auto _ : state)
    {
        lh_date_t cur = date;
        benchmark::DoNotOptimize(lh_date_add_day(lh_addr_of(cur), 1U));
        benchmark::DoNotOptimize(cur);
    }
}
BENCHMARK(BM_date_add_day);

static void
BM_date_add_day_month_roll(benchmark::State &state)
{
    lh_date_t date = lh_date_initializer(2026, 1, 31);
    for (auto _ : state)
    {
        lh_date_t cur = date;
        benchmark::DoNotOptimize(lh_date_add_day(lh_addr_of(cur), 1U));
        benchmark::DoNotOptimize(cur);
    }
}
BENCHMARK(BM_date_add_day_month_roll);
