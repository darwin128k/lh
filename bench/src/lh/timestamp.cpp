#include <benchmark/benchmark.h>

#include <lh/timestamp.h>

/* Times around 2026-10-15 12:34:56 UTC, in steps of 17 days and an hour that wrap
   fifty years either side: every call sees a date a program would really ask about,
   not one thousands of years out. */
static const lh_timestamp_t base = 1791981296;
static const lh_timestamp_t step = 86400 * 17 + 3727;
static const lh_timestamp_t span = 86400LL * 365 * 100;

static lh_timestamp_t
next(lh_timestamp_t t)
{
    t += step;
    return t > base + span / 2 ? base - span / 2 : t;
}

static void
BM_timestamp_to_datetime(benchmark::State &state)
{
    lh_timestamp_t t = base;
    lh_datetime_t out;

    for (auto _ : state)
    {
        lh_timestamp_to_datetime(t, &out);
        benchmark::DoNotOptimize(out);
        t = next(t);
    }
}
BENCHMARK(BM_timestamp_to_datetime);

static void
BM_timestamp_from_datetime(benchmark::State &state)
{
    lh_datetime_t dt;
    lh_timestamp_t t = base;

    lh_timestamp_to_datetime(base, &dt);
    for (auto _ : state)
    {
        t = lh_timestamp_from_datetime(&dt);
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_timestamp_from_datetime);

static void
BM_timestamp_get_date(benchmark::State &state)
{
    lh_timestamp_t t = base;

    for (auto _ : state)
    {
        lh_date_t d = lh_timestamp_get_date(t);
        benchmark::DoNotOptimize(d);
        t = next(t);
    }
}
BENCHMARK(BM_timestamp_get_date);

static void
BM_timestamp_get_fields(benchmark::State &state)
{
    lh_timestamp_t t = base;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_timestamp_get_year(t));
        benchmark::DoNotOptimize(lh_timestamp_get_month(t));
        benchmark::DoNotOptimize(lh_timestamp_get_day(t));
        benchmark::DoNotOptimize(lh_timestamp_get_hour(t));
        benchmark::DoNotOptimize(lh_timestamp_get_minute(t));
        benchmark::DoNotOptimize(lh_timestamp_get_second(t));
        t = next(t);
    }
}
BENCHMARK(BM_timestamp_get_fields);

static void
BM_timestamp_add_months(benchmark::State &state)
{
    lh_timestamp_t t = base;

    for (auto _ : state)
    {
        t = lh_timestamp_add_months(t, 1u);
        benchmark::DoNotOptimize(t);
        if (t > base + 86400LL * 365 * 200)
        {
            t = base;
        }
    }
}
BENCHMARK(BM_timestamp_add_months);

static void
BM_timestamp_add_years(benchmark::State &state)
{
    lh_timestamp_t t = base;

    for (auto _ : state)
    {
        t = lh_timestamp_add_years(t, 1u);
        benchmark::DoNotOptimize(t);
        if (t > base + 86400LL * 365 * 2000)
        {
            t = base;
        }
    }
}
BENCHMARK(BM_timestamp_add_years);
