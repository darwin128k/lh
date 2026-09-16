#include <gtest/gtest.h>

#include <lh/time/minute.h>
#include <lh/time/minute/initializer.h>

namespace
{

TEST(time_minute_init, pointer_and_brace)
{
    lh_time_minute_t via_macro = lh_time_minute_initializer(10);
    lh_time_minute_t via_fn;

    lh_time_minute_init(&via_fn, 10);
    EXPECT_EQ(lh_time_minute_get(&via_macro), 10);
    EXPECT_EQ(lh_time_minute_equals(&via_macro, &via_fn), lh_bool_true);
}

TEST(time_minute_set, wraps_and_returns_hours)
{
    lh_time_minute_t minute = lh_time_minute_initializer(0);
    EXPECT_EQ(lh_time_minute_set(&minute, 125), 2U);
    EXPECT_EQ(lh_time_minute_get(&minute), 5);
}

TEST(time_minute_add, carries_into_hours)
{
    lh_time_minute_t minute = lh_time_minute_initializer(50);
    EXPECT_EQ(lh_time_minute_add(&minute, 20), 1U);
    EXPECT_EQ(lh_time_minute_get(&minute), 10);
}

TEST(time_minute_sub, borrows_hours)
{
    lh_time_minute_t minute = lh_time_minute_initializer(10);
    EXPECT_EQ(lh_time_minute_sub(&minute, 20), 1U);
    EXPECT_EQ(lh_time_minute_get(&minute), 50);
}

TEST(time_minute_compare, equals_and_is_at_least)
{
    const lh_time_minute_t a = lh_time_minute_initializer(10);
    const lh_time_minute_t b = lh_time_minute_initializer(10);
    const lh_time_minute_t c = lh_time_minute_initializer(11);

    EXPECT_EQ(lh_time_minute_equals(&a, &b), lh_bool_true);
    EXPECT_EQ(lh_time_minute_is_at_least(&c, &a), lh_bool_true);
    EXPECT_EQ(lh_time_minute_is_at_least(&a, &c), lh_bool_false);
    EXPECT_EQ(lh_time_minute_is_less(&a, &c), lh_bool_true);
    EXPECT_EQ(lh_time_minute_is_greater(&c, &a), lh_bool_true);
}

} // namespace
