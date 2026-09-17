#include <benchmark/benchmark.h>

#include <lh/date/initializer.h>
#include <lh/datetime.h>
#include <lh/datetime/initializer.h>
#include <lh/time/initializer.h>
#include <lh/util/addr.h>

static void
BM_datetime_parse(benchmark::State &state)
{
    const char text[] = "2026/09/18 09:05:00";
    lh_datetime_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_datetime_parse(text, sizeof(text) - 1, lh_addr_of(out)));
    }
}
BENCHMARK(BM_datetime_parse);

static void
BM_datetime_format(benchmark::State &state)
{
    lh_datetime_t dt = lh_datetime_initializer(lh_date_initializer(2026, 9, 18),
                                               lh_time_initializer(9, 5, 0));
    char buf[LH_DATETIME_TEXT_MAX];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_datetime_format(lh_addr_of(dt), buf, sizeof(buf)));
    }
}
BENCHMARK(BM_datetime_format);
