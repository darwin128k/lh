#include <gtest/gtest.h>

#include <lh/os/datetime.h>

namespace
{

TEST(os_datetime_now, fills_local_calendar)
{
    lh_datetime_t a{};
    lh_datetime_t b{};
    lh_char_t text[LH_DATETIME_TEXT_MAX];
    lh_datetime_t parsed{};
    lh_usize_t n;

    ASSERT_EQ(lh_os_datetime_now(&a), lh_bool_true);
    ASSERT_EQ(lh_os_datetime_now(&b), lh_bool_true);
    EXPECT_EQ(lh_datetime_is_at_least(&b, &a), lh_bool_true);
    EXPECT_GE(lh_date_get_year(&a.date), 2020);
    EXPECT_GE(lh_date_get_month(&a.date), LH_DATE_MONTH_MIN);
    EXPECT_LE(lh_date_get_month(&a.date), LH_DATE_MONTH_MAX);
    EXPECT_GE(lh_date_get_day(&a.date), LH_DATE_DAY_MIN);
    EXPECT_LE(lh_time_get_hour(&a.time), LH_TIME_HOUR_MAX);
    EXPECT_LE(lh_time_get_minute(&a.time), LH_TIME_MINUTE_MAX);
    EXPECT_LE(lh_time_get_second(&a.time), LH_TIME_SECOND_MAX);

    n = lh_datetime_format(&a, text, sizeof(text));
    ASSERT_NE(n, 0U);
    EXPECT_EQ(lh_datetime_parse(text, n, &parsed), lh_bool_true);
    EXPECT_EQ(lh_datetime_equals(&parsed, &a), lh_bool_true);
}

} // namespace
