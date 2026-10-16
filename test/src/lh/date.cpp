#include <gtest/gtest.h>

#include <string>

#include <lh/date.h>
#include <lh/date/initializer.h>
#include <lh/test/out.h>

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
    const lh_date_t result = lh_test::out(lh_date_from_epoch_days, lh_date_days_since_epoch(&date));
    const lh_date_t before_epoch = lh_test::out(lh_date_from_epoch_days, -1);

    EXPECT_EQ(lh_date_equals(&date, &result), lh_bool_true);
    EXPECT_EQ(lh_date_get_year(&before_epoch), 1969);
    EXPECT_EQ(lh_date_get_month(&before_epoch), 12);
    EXPECT_EQ(lh_date_get_day(&before_epoch), 31);
}

/* lh_date_from_epoch_days finds the year in O(1) from the 400-year cycle. The
   reference here is the plain walk it replaced -- a year at a time, then a month at
   a time -- written out independently, and every day from 0000-01-01 to 9999-12-31
   has to come out the same and go back to the same day number. */
TEST(date_from_epoch_days, matches_a_day_by_day_walk_for_ten_thousand_years)
{
    const lh_s64_t first = -719528; /* 0000-01-01 */
    lh_s64_t days = first;
    lh_date_year_t year = 0;
    lh_date_month_t month = 1;
    lh_date_day_t day = 1;

    for (; year <= 9999; ++days)
    {
        const lh_date_t got = lh_test::out(lh_date_from_epoch_days, days);

        ASSERT_EQ(lh_date_get_year(&got), year) << "day " << days;
        ASSERT_EQ(lh_date_get_month(&got), month) << "day " << days;
        ASSERT_EQ(lh_date_get_day(&got), day) << "day " << days;
        ASSERT_EQ(lh_date_days_since_epoch(&got), days);

        if (day < lh_date_days_in_month(year, month))
        {
            ++day;
            continue;
        }
        day = 1;
        if (month < 12)
        {
            ++month;
            continue;
        }
        month = 1;
        ++year;
    }
}

TEST(date_year_from_days_from_zero, is_the_inverse_of_days_from_zero)
{
    for (lh_s64_t y = -1000; y <= 70000; ++y)
    {
        const lh_s64_t start = lh_date_year_days_from_zero(y);

        ASSERT_EQ(lh_date_year_from_days_from_zero(start), y);
        ASSERT_EQ(lh_date_year_from_days_from_zero(start - 1), y - 1);
    }
}

TEST(date_days_before_month, common_and_leap)
{
    EXPECT_EQ(lh_date_days_before_month(2026, 1), 0);
    EXPECT_EQ(lh_date_days_before_month(2026, 3), 59);
    EXPECT_EQ(lh_date_days_before_month(2024, 3), 60);
    EXPECT_EQ(lh_date_days_before_month(2024, 12), 335);
    EXPECT_EQ(lh_date_days_before_month(2026, 13), 0);
}

TEST(date_year_days, leap_and_common)
{
    EXPECT_EQ(lh_date_year_days(2024), 366);
    EXPECT_EQ(lh_date_year_days(2026), 365);
    EXPECT_EQ(lh_date_year_days(2000), 366);
    EXPECT_EQ(lh_date_year_days(1900), 365);
}

TEST(date_scalar_compare, year_month_day_index)
{
    EXPECT_EQ(lh_date_year_equals(2026, 2026), lh_bool_true);
    EXPECT_EQ(lh_date_year_is_less(2025, 2026), lh_bool_true);
    EXPECT_EQ(lh_date_year_is_greater(2026, 2025), lh_bool_true);

    EXPECT_EQ(lh_date_month_equals(9, 9), lh_bool_true);
    EXPECT_EQ(lh_date_month_is_less(8, 9), lh_bool_true);
    EXPECT_EQ(lh_date_month_is_at_least(9, 9), lh_bool_true);

    EXPECT_EQ(lh_date_day_equals(16, 16), lh_bool_true);
    EXPECT_EQ(lh_date_day_is_less(15, 16), lh_bool_true);
    EXPECT_EQ(lh_date_day_is_greater(16, 15), lh_bool_true);

    EXPECT_EQ(lh_date_month_index_equals(0, 0), lh_bool_true);
    EXPECT_EQ(lh_date_month_index_is_less(0, 11), lh_bool_true);
    EXPECT_EQ(lh_date_month_index_is_greater(11, 0), lh_bool_true);
}

} // namespace
