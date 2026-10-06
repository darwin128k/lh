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

} // namespace
