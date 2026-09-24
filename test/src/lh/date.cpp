#include <gtest/gtest.h>

#include <string>

#include <lh/date.h>
#include <lh/date/initializer.h>
#include <lh/date/month/index.h>

namespace
{

TEST(date_parse, padded_and_leap)
{
    lh_date_t date{};
    ASSERT_EQ(lh_date_parse("2026/09/16", 10, &date), lh_bool_true);
    EXPECT_EQ(lh_date_get_year(&date), 2026);
    EXPECT_EQ(lh_date_get_month(&date), 9);
    EXPECT_EQ(lh_date_get_day(&date), 16);

    ASSERT_EQ(lh_date_parse("2024/02/29", 10, &date), lh_bool_true);
    EXPECT_EQ(lh_date_parse("2023/02/29", 10, &date), lh_bool_false);
}

TEST(date_format, zero_pads_month_day)
{
    const lh_date_t date = lh_date_initializer(2026, 9, 16);
    lh_char_t buf[LH_DATE_TEXT_MAX];
    const lh_usize_t n = lh_date_format(&date, buf, sizeof(buf));
    EXPECT_EQ(n, 10U);
    EXPECT_EQ(std::string(buf, n), "2026/09/16");
}

TEST(date_is_at_least, later_month)
{
    const lh_date_t a = lh_date_initializer(2026, 10, 1);
    const lh_date_t b = lh_date_initializer(2026, 9, 30);
    EXPECT_EQ(lh_date_is_at_least(&a, &b), lh_bool_true);
    EXPECT_EQ(lh_date_equals(&a, &b), lh_bool_false);
}

TEST(date_compare, less_greater)
{
    const lh_date_t earlier = lh_date_initializer(2026, 9, 16);
    const lh_date_t later = lh_date_initializer(2026, 9, 17);

    EXPECT_EQ(lh_date_is_less(&earlier, &later), lh_bool_true);
    EXPECT_EQ(lh_date_is_greater(&later, &earlier), lh_bool_true);
    EXPECT_EQ(lh_date_is_less(&earlier, &earlier), lh_bool_false);
    EXPECT_EQ(lh_date_is_at_least(&later, &earlier), lh_bool_true);
}

TEST(date_add, days_and_month_clamp)
{
    lh_date_t date = lh_date_initializer(2026, 1, 31);
    const lh_date_t one_day = lh_date_initializer(0, 0, 1);
    const lh_date_t one_month = lh_date_initializer(0, 1, 0);

    EXPECT_EQ(lh_date_add(&date, &one_day), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 1);

    date = lh_date_initializer(2026, 1, 31);
    EXPECT_EQ(lh_date_add(&date, &one_month), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 28);
}

TEST(date_sub, previous_month)
{
    lh_date_t date = lh_date_initializer(2026, 3, 1);
    const lh_date_t one_day = lh_date_initializer(0, 0, 1);

    EXPECT_EQ(lh_date_sub(&date, &one_day), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 28);
}

TEST(date_add_day, last_of_month)
{
    lh_date_t date = lh_date_initializer(2026, 1, 31);

    EXPECT_EQ(lh_date_add_day(&date, 1), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 1);
}

TEST(date_month_index, wrap_years)
{
    lh_date_month_index_t index = LH_DATE_MONTH_INDEX_JANUARY;

    EXPECT_EQ(lh_date_month_index_add(&index, 13), 1U);
    EXPECT_EQ(index, LH_DATE_MONTH_INDEX_FEBRUARY);
}

TEST(date_add_month, updates_date_in_place)
{
    lh_date_t date = lh_date_initializer(2026, 1, 31);

    EXPECT_EQ(lh_date_add_month(&date, 1), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 28);
}

TEST(date_sub_month, updates_date_in_place)
{
    lh_date_t date = lh_date_initializer(2026, 3, 31);

    EXPECT_EQ(lh_date_sub_month(&date, 1), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 28);
}

TEST(date_days_since_epoch, known_offsets)
{
    const lh_date_t epoch = lh_date_initializer(1970, 1, 1);
    const lh_date_t after = lh_date_initializer(1970, 1, 2);
    const lh_date_t before = lh_date_initializer(1969, 12, 31);

    EXPECT_EQ(lh_date_days_since_epoch(&epoch), 0);
    EXPECT_EQ(lh_date_days_since_epoch(&after), 1);
    EXPECT_EQ(lh_date_days_since_epoch(&before), -1);
}

TEST(date_from_epoch_days, roundtrip_before_and_after_epoch)
{
    const lh_date_t date = lh_date_initializer(2026, 9, 16);
    const lh_date_t result = lh_date_from_epoch_days(lh_date_days_since_epoch(&date));
    const lh_date_t before_epoch = lh_date_from_epoch_days(-1);

    EXPECT_EQ(lh_date_equals(&date, &result), lh_bool_true);
    EXPECT_EQ(lh_date_get_year(&before_epoch), 1969);
    EXPECT_EQ(lh_date_get_month(&before_epoch), 12);
    EXPECT_EQ(lh_date_get_day(&before_epoch), 31);
}

TEST(date_year_days, leap_and_common)
{
    EXPECT_EQ(lh_date_year_days(2024), 366);
    EXPECT_EQ(lh_date_year_days(2026), 365);
    EXPECT_EQ(lh_date_year_days(2000), 366);
    EXPECT_EQ(lh_date_year_days(1900), 365);
}

} // namespace
