#include <gtest/gtest.h>

#include <lh/time/second.h>
#include <lh/time/second/initializer.h>

namespace
{

TEST(time_second_init, pointer_and_brace)
{
    lh_time_second_t via_macro = lh_time_second_initializer(10);
    lh_time_second_t via_fn;

    lh_time_second_init(&via_fn, 10);
    EXPECT_EQ(lh_time_second_get(&via_macro), 10);
    EXPECT_EQ(lh_time_second_equals(&via_macro, &via_fn), lh_bool_true);
}

TEST(time_second_set, wraps_and_returns_minutes)
{
    lh_time_second_t second = lh_time_second_initializer(0);
    EXPECT_EQ(lh_time_second_set(&second, 125), 2U);
    EXPECT_EQ(lh_time_second_get(&second), 5);
}

TEST(time_second_set, in_range_is_zero_overflow)
{
    lh_time_second_t second = lh_time_second_initializer(0);
    EXPECT_EQ(lh_time_second_set(&second, 59), 0U);
    EXPECT_EQ(lh_time_second_get(&second), 59);
}

TEST(time_second_add, carries_into_minutes)
{
    lh_time_second_t second = lh_time_second_initializer(50);
    EXPECT_EQ(lh_time_second_add(&second, 20), 1U);
    EXPECT_EQ(lh_time_second_get(&second), 10);
}

TEST(time_second_compare, equals_and_is_at_least)
{
    const lh_time_second_t a = lh_time_second_initializer(10);
    const lh_time_second_t b = lh_time_second_initializer(10);
    const lh_time_second_t c = lh_time_second_initializer(11);

    EXPECT_EQ(lh_time_second_equals(&a, &b), lh_bool_true);
    EXPECT_EQ(lh_time_second_equals(&a, &c), lh_bool_false);
    EXPECT_EQ(lh_time_second_is_at_least(&c, &a), lh_bool_true);
    EXPECT_EQ(lh_time_second_is_at_least(&a, &c), lh_bool_false);
    EXPECT_EQ(lh_time_second_is_at_least(&a, &b), lh_bool_true);
    EXPECT_EQ(lh_time_second_is_less(&a, &c), lh_bool_true);
    EXPECT_EQ(lh_time_second_is_less(&a, &b), lh_bool_false);
    EXPECT_EQ(lh_time_second_is_greater(&c, &a), lh_bool_true);
    EXPECT_EQ(lh_time_second_is_greater(&a, &c), lh_bool_false);
}

} // namespace
