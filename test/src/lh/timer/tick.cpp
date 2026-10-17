#include <gtest/gtest.h>

#include <lh/timer/tick.h>

namespace
{

TEST(tick_compare, suite)
{
    EXPECT_EQ(lh_tick_equals(10, 10), lh_bool_true);
    EXPECT_EQ(lh_tick_equals(10, 11), lh_bool_false);
    EXPECT_EQ(lh_tick_is_at_least(10, 10), lh_bool_true);
    EXPECT_EQ(lh_tick_is_at_least(11, 10), lh_bool_true);
    EXPECT_EQ(lh_tick_is_at_least(9, 10), lh_bool_false);
    EXPECT_EQ(lh_tick_is_less(9, 10), lh_bool_true);
    EXPECT_EQ(lh_tick_is_less(10, 10), lh_bool_false);
    EXPECT_EQ(lh_tick_is_greater(11, 10), lh_bool_true);
    EXPECT_EQ(lh_tick_is_greater(10, 11), lh_bool_false);
}

TEST(tick_elapsed, counts_across_the_wrap)
{
    EXPECT_EQ(lh_tick_elapsed(0xFFFFFFF0U, 0x10U), 0x20U);
    EXPECT_EQ(lh_tick_elapsed(100U, 150U), 50U);
    EXPECT_EQ(lh_tick_elapsed(7U, 7U), 0U);
    EXPECT_EQ(lh_tick_elapsed(0U, 0xFFFFFFFFU), 0xFFFFFFFFU);
}

TEST(tick_is_after, reads_the_distance_as_signed)
{
    EXPECT_EQ(lh_tick_is_after(0x10U, 0xFFFFFFF0U), lh_bool_true);
    EXPECT_EQ(lh_tick_is_after(0xFFFFFFF0U, 0x10U), lh_bool_false);
    EXPECT_EQ(lh_tick_is_after(11U, 10U), lh_bool_true);
    EXPECT_EQ(lh_tick_is_after(10U, 11U), lh_bool_false);
    EXPECT_EQ(lh_tick_is_after(5U, 5U), lh_bool_false);
}

} // namespace
