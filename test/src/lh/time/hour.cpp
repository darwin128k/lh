#include <gtest/gtest.h>

#include <lh/time/hour.h>
#include <lh/time/hour/initializer.h>

namespace
{

TEST(time_hour_init, pointer_and_brace)
{
    lh_time_hour_t via_macro = lh_time_hour_initializer(9);
    lh_time_hour_t via_fn;

    lh_time_hour_init(&via_fn, 9);
    EXPECT_EQ(lh_time_hour_get(&via_macro), 9);
    EXPECT_EQ(lh_time_hour_equals(&via_macro, &via_fn), lh_bool_true);
}

TEST(time_hour_set, wraps_and_returns_days)
{
    lh_time_hour_t hour = lh_time_hour_initializer(0);
    EXPECT_EQ(lh_time_hour_set(&hour, 50), 2U);
    EXPECT_EQ(lh_time_hour_get(&hour), 2);
}

TEST(time_hour_add, carries_into_days)
{
    lh_time_hour_t hour = lh_time_hour_initializer(20);
    EXPECT_EQ(lh_time_hour_add(&hour, 6), 1U);
    EXPECT_EQ(lh_time_hour_get(&hour), 2);
}

TEST(time_hour_sub, borrows_days)
{
    lh_time_hour_t hour = lh_time_hour_initializer(2);
    EXPECT_EQ(lh_time_hour_sub(&hour, 6), 1U);
    EXPECT_EQ(lh_time_hour_get(&hour), 20);
}

TEST(time_hour_compare, equals_and_is_at_least)
{
    const lh_time_hour_t a = lh_time_hour_initializer(9);
    const lh_time_hour_t b = lh_time_hour_initializer(9);
    const lh_time_hour_t c = lh_time_hour_initializer(10);

    EXPECT_EQ(lh_time_hour_equals(&a, &b), lh_bool_true);
    EXPECT_EQ(lh_time_hour_is_at_least(&c, &a), lh_bool_true);
    EXPECT_EQ(lh_time_hour_is_at_least(&a, &c), lh_bool_false);
    EXPECT_EQ(lh_time_hour_is_less(&a, &c), lh_bool_true);
    EXPECT_EQ(lh_time_hour_is_greater(&c, &a), lh_bool_true);
}

} // namespace
