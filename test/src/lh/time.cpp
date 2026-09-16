#include <gtest/gtest.h>

#include <string>

#include <lh/time.h>
#include <lh/time/initializer.h>

namespace
{

TEST(time_parse, padded)
{
    lh_time_t t{};
    ASSERT_EQ(lh_time_parse("09:05:00", 8, &t), lh_bool_true);
    EXPECT_EQ(lh_time_get_hour(&t), 9);
    EXPECT_EQ(lh_time_get_minute(&t), 5);
    EXPECT_EQ(lh_time_get_second(&t), 0);
    EXPECT_EQ(lh_time_parse("24:00:00", 8, &t), lh_bool_false);
}

TEST(time_format, zero_pads)
{
    const lh_time_t t = lh_time_initializer(9, 5, 0);
    lh_char_t buf[LH_TIME_TEXT_MAX];
    const lh_usize_t n = lh_time_format(&t, buf, sizeof(buf));
    EXPECT_EQ(n, 8U);
    EXPECT_EQ(std::string(buf, n), "09:05:00");
}

TEST(time_compare, less_greater)
{
    const lh_time_t earlier = lh_time_initializer(9, 5, 0);
    const lh_time_t later = lh_time_initializer(9, 5, 1);

    EXPECT_EQ(lh_time_is_less(&earlier, &later), lh_bool_true);
    EXPECT_EQ(lh_time_is_greater(&later, &earlier), lh_bool_true);
    EXPECT_EQ(lh_time_is_less(&earlier, &earlier), lh_bool_false);
    EXPECT_EQ(lh_time_is_at_least(&later, &earlier), lh_bool_true);
}

TEST(time_add, wraps_into_days)
{
    lh_time_t time = lh_time_initializer(23, 50, 40);
    const lh_time_t addend = lh_time_initializer(1, 20, 30);

    EXPECT_EQ(lh_time_add(&time, &addend), 1U);
    EXPECT_EQ(lh_time_get_hour(&time), 1);
    EXPECT_EQ(lh_time_get_minute(&time), 11);
    EXPECT_EQ(lh_time_get_second(&time), 10);
}

TEST(time_sub, borrows_days)
{
    lh_time_t time = lh_time_initializer(1, 11, 10);
    const lh_time_t subtrahend = lh_time_initializer(1, 20, 30);

    EXPECT_EQ(lh_time_sub(&time, &subtrahend), 1U);
    EXPECT_EQ(lh_time_get_hour(&time), 23);
    EXPECT_EQ(lh_time_get_minute(&time), 50);
    EXPECT_EQ(lh_time_get_second(&time), 40);
}

TEST(time_add_hour, wraps_into_days)
{
    lh_time_t time = lh_time_initializer(20, 0, 0);

    EXPECT_EQ(lh_time_add_hour(&time, 6), 1U);
    EXPECT_EQ(lh_time_get_hour(&time), 2);
}

} // namespace
