/*
 * Logical timers: arming one, and the group handler that a pump calls every tick
 * with the argument's number of timers in it.
 */

#include <benchmark/benchmark.h>

#include <lh/timer.h>

#include <vector>

static lh_void
on_timer(lh_timer_t *self, lh_ptr context)
{
    (void)self;
    ++*static_cast<unsigned *>(context);
}

static void
BM_timer_start_stop(benchmark::State &state)
{
    lh_timer_group_t group;
    lh_timer_t timer;
    unsigned fired = 0;
    lh_tick_t now = 0;

    lh_timer_group_init(&group);
    lh_timer_init(&timer);
    for (auto _ : state)
    {
        lh_timer_start(&group, &timer, 20u, lh_bool_true, 0u, on_timer, &fired, now++);
        lh_timer_stop(&timer);
    }
    lh_timer_deinit(&timer);
    lh_timer_group_deinit(&group);
}
BENCHMARK(BM_timer_start_stop);

/** The handler at a tick where every repeating timer (periods 1..N ms) is checked. */
static void
BM_timer_group_handler(benchmark::State &state)
{
    const std::size_t n = static_cast<std::size_t>(state.range(0));
    std::vector<lh_timer_t> timers(n);
    lh_timer_group_t group;
    unsigned fired = 0;
    lh_tick_t now = 0;

    lh_timer_group_init(&group);
    for (std::size_t i = 0; i < n; ++i)
    {
        lh_timer_init(&timers[i]);
        lh_timer_start(&group, &timers[i], static_cast<lh_tick_t>(1u + i), lh_bool_true,
                       static_cast<lh_u8_t>(i % 4u), on_timer, &fired, 0u);
    }
    for (auto _ : state)
    {
        lh_timer_group_handler(&group, ++now);
        benchmark::DoNotOptimize(fired);
    }
    for (std::size_t i = 0; i < n; ++i)
    {
        lh_timer_deinit(&timers[i]);
    }
    lh_timer_group_deinit(&group);
    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(n));
}
BENCHMARK(BM_timer_group_handler)->Arg(1)->Arg(16)->Arg(256);

static void
BM_timer_group_until_next(benchmark::State &state)
{
    std::vector<lh_timer_t> timers(64);
    lh_timer_group_t group;
    unsigned fired = 0;

    lh_timer_group_init(&group);
    for (std::size_t i = 0; i < timers.size(); ++i)
    {
        lh_timer_init(&timers[i]);
        lh_timer_start(&group, &timers[i], static_cast<lh_tick_t>(10u + i * 7u), lh_bool_true, 0u, on_timer,
                       &fired, 0u);
    }
    lh_tick_t now = 0;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_timer_group_until_next(&group, now));
        now = (now + 1u) % 10u;
    }
    for (auto &t : timers)
    {
        lh_timer_deinit(&t);
    }
    lh_timer_group_deinit(&group);
}
BENCHMARK(BM_timer_group_until_next);
