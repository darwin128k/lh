#include <gtest/gtest.h>

#include <lh/core/net/modbus/schedule.h>

#include <vector>

/* -- The schedule: what is due, and when -------------------------------------------
   Three requests over five wants, as lh_mb_plan_reads makes them: wants 0-1 (a
   temperature at 1 s, a setting at 60 s), want 2 alone, wants 3-4. */
namespace
{

const lh_mb_want_t kWants[] = {{10, 1}, {11, 1}, {100, 1}, {200, 2}, {203, 1}};
const lh_u64_t kStart = 1000000U;

struct scheduled
{
    lh_mb_schedule_t schedule;
    std::vector<lh_mb_read_t> reads;

    scheduled()
    {
        reads.resize(5);
        reads.resize(lh_mb_plan_reads(kWants, 5, reads.data(), 5));
        lh_mb_schedule_init(&schedule);
        EXPECT_TRUE(
            lh_mb_schedule_try_assign(&schedule, reads.data(), (lh_u16_t)reads.size(), 5, kStart));
        lh_mb_schedule_set_want_period_us(&schedule, 0, 1000000U);
        lh_mb_schedule_set_want_period_us(&schedule, 1, 60000000U);
        lh_mb_schedule_set_want_period_us(&schedule, 2, 5000000U);
        lh_mb_schedule_set_want_period_us(&schedule, 3, 30000000U);
        lh_mb_schedule_set_want_period_us(&schedule, 4, 0U); /* keeps the default */
    }

    ~scheduled()
    {
        lh_mb_schedule_deinit(&schedule);
    }

    /* Attempt everything due at @p now, in the schedule's order. */
    std::vector<lh_u16_t>
    run(lh_u64_t now)
    {
        std::vector<lh_u16_t> sent;
        for (lh_u16_t i;
             (i = lh_mb_schedule_find_due(&schedule, now)) < lh_mb_schedule_get_count(&schedule);)
        {
            sent.push_back(i);
            lh_mb_schedule_mark_done(&schedule, i, now);
        }
        return sent;
    }
};

} // namespace

TEST(lh_mb_schedule, a_request_is_polled_at_the_shortest_period_of_its_wants)
{
    scheduled s;

    ASSERT_EQ(lh_mb_schedule_get_count(&s.schedule), 3);
    EXPECT_EQ(lh_mb_schedule_get_read_period_us(&s.schedule, 0), 1000000U)
        << "the 60 s setting rides with the 1 s temperature";
    EXPECT_EQ(lh_mb_schedule_get_read_period_us(&s.schedule, 1), 5000000U);
    EXPECT_EQ(lh_mb_schedule_get_read_period_us(&s.schedule, 2), LH_MB_SCHEDULE_PERIOD_US)
        << "30 s and the 10 s default: 10 s";
}

TEST(lh_mb_schedule, everything_is_due_at_the_start_and_then_at_its_period)
{
    scheduled s;

    EXPECT_EQ(s.run(kStart).size(), 3U);
    EXPECT_TRUE(s.run(kStart + 999999U).empty());
    EXPECT_EQ(s.run(kStart + 1000000U), std::vector<lh_u16_t>{0});
    EXPECT_EQ(lh_mb_schedule_get_next_due_us(&s.schedule), kStart + 2000000U);
    EXPECT_EQ(s.run(kStart + 5000000U), (std::vector<lh_u16_t>{0, 1}))
        << "the most overdue first: request 0 has waited since +2 s, request 1 since +5 s";
}

TEST(lh_mb_schedule, ten_seconds_of_polling_ask_each_request_as_often_as_it_needs)
{
    scheduled s;
    std::vector<int> asked(3, 0);

    for (lh_u64_t t = kStart; t < kStart + 10000000U; t += 100000U)
    {
        for (lh_u16_t i : s.run(t))
        {
            ++asked[i];
        }
    }
    EXPECT_EQ(asked[0], 10) << "every second";
    EXPECT_EQ(asked[1], 2) << "every five seconds";
    EXPECT_EQ(asked[2], 1) << "once in ten seconds";
}

TEST(lh_mb_schedule, a_watched_want_is_fresh_at_once_and_then_fast)
{
    scheduled s;

    s.run(kStart);
    lh_mb_schedule_watch(&s.schedule, 3, kStart + 100000U);
    EXPECT_EQ(s.run(kStart + 100000U), std::vector<lh_u16_t>{2}) << "due at once, not in 10 s";
    EXPECT_EQ(lh_mb_schedule_get_read_period_us(&s.schedule, 2), LH_MB_SCHEDULE_FAST_US);
    EXPECT_EQ(s.run(kStart + 100000U + LH_MB_SCHEDULE_FAST_US), std::vector<lh_u16_t>{2});
}

TEST(lh_mb_schedule, watches_are_counted_and_the_last_unwatch_slows_it_down)
{
    scheduled s;

    lh_mb_schedule_watch(&s.schedule, 2, kStart);
    lh_mb_schedule_watch(&s.schedule, 2, kStart);
    lh_mb_schedule_unwatch(&s.schedule, 2);
    EXPECT_EQ(lh_mb_schedule_get_want_period_us(&s.schedule, 2), LH_MB_SCHEDULE_FAST_US)
        << "another screen still shows it";
    lh_mb_schedule_unwatch(&s.schedule, 2);
    EXPECT_EQ(lh_mb_schedule_get_want_period_us(&s.schedule, 2), 5000000U);
    lh_mb_schedule_unwatch(&s.schedule, 2);
    EXPECT_EQ(lh_mb_schedule_get_want_period_us(&s.schedule, 2), 5000000U) << "not below zero";
}

TEST(lh_mb_schedule, a_watch_does_not_slow_a_want_that_is_faster_already)
{
    scheduled s;

    lh_mb_schedule_set_fast(&s.schedule, 2000000U);
    lh_mb_schedule_watch(&s.schedule, 0, kStart);
    EXPECT_EQ(lh_mb_schedule_get_want_period_us(&s.schedule, 0), 1000000U);
}

TEST(lh_mb_schedule, a_dead_device_is_asked_at_the_period_and_not_as_fast_as_possible)
{
    scheduled s;
    int attempts = 0;

    /* Every attempt fails; the master marks it done all the same. */
    for (lh_u64_t t = kStart; t < kStart + 3000000U; t += 1000U)
    {
        for (lh_u16_t i : s.run(t))
        {
            attempts += i == 0 ? 1 : 0;
        }
    }
    EXPECT_EQ(attempts, 3) << "three seconds, three attempts of the 1 s request";
}

TEST(lh_mb_schedule, finds_the_request_that_carries_a_want)
{
    scheduled s;

    EXPECT_EQ(lh_mb_schedule_find_read(&s.schedule, 0), 0);
    EXPECT_EQ(lh_mb_schedule_find_read(&s.schedule, 1), 0);
    EXPECT_EQ(lh_mb_schedule_find_read(&s.schedule, 2), 1);
    EXPECT_EQ(lh_mb_schedule_find_read(&s.schedule, 4), 2);
    EXPECT_EQ(lh_mb_schedule_find_read(&s.schedule, 5), 3) << "no such want";
}

TEST(lh_mb_schedule, an_overloaded_line_stretches_every_period_alike)
{
    scheduled s;

    s.run(kStart);
    /* 6 s later nothing has been asked: request 0 (1 s) is 5 periods late, request 1
       (5 s) is 0.2 of a period late, request 2 (10 s) is not due. By absolute lateness
       request 0 and 1 tie at nothing useful; by lateness for the period, request 0 is
       far more behind and goes first. */
    EXPECT_EQ(lh_mb_schedule_find_due(&s.schedule, kStart + 6000000U), 0);
    EXPECT_EQ(lh_mb_schedule_get_lateness(&s.schedule, 0, kStart + 6000000U), 5U * 1024U);
    EXPECT_EQ(lh_mb_schedule_get_lateness(&s.schedule, 1, kStart + 6000000U), 1024U / 5U);
    EXPECT_EQ(lh_mb_schedule_get_lateness(&s.schedule, 2, kStart + 6000000U), 0U);
}

TEST(lh_mb_schedule, a_slow_request_long_overdue_does_not_starve_a_fast_one)
{
    scheduled s;

    s.run(kStart);
    /* Request 2 (10 s) is 12 s late, request 0 (1 s) is 2 s late: the slow one is
       later in seconds, the fast one is later for what it is -- 2 periods against 1.2. */
    lh_mb_schedule_mark_done(&s.schedule, 0, kStart + 19000000U); /* due at +20 s */
    lh_mb_schedule_mark_done(&s.schedule, 1, kStart + 30000000U); /* not due */
    EXPECT_EQ(lh_mb_schedule_find_due(&s.schedule, kStart + 22000000U), 0);
}

TEST(lh_mb_schedule, what_is_on_a_screen_goes_before_what_is_later)
{
    scheduled s;

    s.run(kStart);
    lh_mb_schedule_watch(&s.schedule, 3, kStart + 5000000U); /* request 2, due at once */
    EXPECT_TRUE(lh_mb_schedule_is_read_watched(&s.schedule, 2));
    EXPECT_FALSE(lh_mb_schedule_is_read_watched(&s.schedule, 0));
    /* Request 0 is four periods late; the watched one goes first all the same. */
    EXPECT_EQ(lh_mb_schedule_find_due(&s.schedule, kStart + 5000000U), 2);
}
