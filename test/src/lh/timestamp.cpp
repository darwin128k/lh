#include <gtest/gtest.h>

#include <lh/date/initializer.h>
#include <lh/datetime/initializer.h>
#include <lh/time/initializer.h>
#include <lh/timestamp.h>

namespace
{

TEST(timestamp_from_datetime, known_epoch_offset)
{
    const lh_datetime_t dt =
        lh_datetime_initializer(lh_date_initializer(1970, 1, 2), lh_time_initializer(0, 0, 1));

    EXPECT_EQ(lh_timestamp_from_datetime(&dt), 86401);
}

TEST(timestamp_to_datetime, roundtrip)
{
    const lh_datetime_t dt =
        lh_datetime_initializer(lh_date_initializer(2026, 9, 16), lh_time_initializer(9, 5, 30));
    lh_datetime_t out{};

    lh_timestamp_to_datetime(lh_timestamp_from_datetime(&dt), &out);
    EXPECT_EQ(lh_datetime_equals(&dt, &out), lh_bool_true);
}

TEST(timestamp_to_datetime, before_epoch)
{
    const lh_datetime_t dt =
        lh_datetime_initializer(lh_date_initializer(1969, 12, 31), lh_time_initializer(23, 0, 0));
    lh_datetime_t out{};

    const lh_timestamp_t ts = lh_timestamp_from_datetime(&dt);
    EXPECT_EQ(ts, -3600);

    lh_timestamp_to_datetime(ts, &out);
    EXPECT_EQ(lh_datetime_equals(&dt, &out), lh_bool_true);
}

TEST(timestamp_from_date, midnight)
{
    const lh_date_t date = lh_date_initializer(1970, 1, 1);
    EXPECT_EQ(lh_timestamp_from_date(&date), 0);
}

TEST(timestamp_from_time, seconds_since_midnight)
{
    const lh_time_t time = lh_time_initializer(1, 1, 1);
    EXPECT_EQ(lh_timestamp_from_time(&time), 3661);
}

TEST(timestamp_get_components, matches_source_fields)
{
    const lh_datetime_t dt =
        lh_datetime_initializer(lh_date_initializer(2026, 9, 16), lh_time_initializer(9, 5, 30));
    const lh_timestamp_t ts = lh_timestamp_from_datetime(&dt);

    EXPECT_EQ(lh_timestamp_get_year(ts), 2026);
    EXPECT_EQ(lh_timestamp_get_month(ts), 9);
    EXPECT_EQ(lh_timestamp_get_day(ts), 16);
    EXPECT_EQ(lh_timestamp_get_hour(ts), 9);
    EXPECT_EQ(lh_timestamp_get_minute(ts), 5);
    EXPECT_EQ(lh_timestamp_get_second(ts), 30);
}

TEST(timestamp_add_seconds, wraps_into_next_day)
{
    const lh_datetime_t dt =
        lh_datetime_initializer(lh_date_initializer(2026, 9, 16), lh_time_initializer(23, 59, 0));
    const lh_timestamp_t shifted = lh_timestamp_add_seconds(lh_timestamp_from_datetime(&dt), 120);

    EXPECT_EQ(lh_timestamp_get_day(shifted), 17);
    EXPECT_EQ(lh_timestamp_get_hour(shifted), 0);
    EXPECT_EQ(lh_timestamp_get_minute(shifted), 1);
}

TEST(timestamp_sub_seconds, borrows_from_previous_day)
{
    const lh_datetime_t dt =
        lh_datetime_initializer(lh_date_initializer(2026, 9, 16), lh_time_initializer(0, 0, 30));
    const lh_timestamp_t shifted = lh_timestamp_sub_seconds(lh_timestamp_from_datetime(&dt), 60);

    EXPECT_EQ(lh_timestamp_get_day(shifted), 15);
    EXPECT_EQ(lh_timestamp_get_hour(shifted), 23);
    EXPECT_EQ(lh_timestamp_get_minute(shifted), 59);
    EXPECT_EQ(lh_timestamp_get_second(shifted), 30);
}

TEST(timestamp_add_days, crosses_month_boundary)
{
    const lh_date_t date = lh_date_initializer(2026, 1, 31);
    const lh_timestamp_t shifted = lh_timestamp_add_days(lh_timestamp_from_date(&date), 1);

    EXPECT_EQ(lh_timestamp_get_month(shifted), 2);
    EXPECT_EQ(lh_timestamp_get_day(shifted), 1);
}

TEST(timestamp_sub_days, crosses_month_boundary)
{
    const lh_date_t date = lh_date_initializer(2026, 3, 1);
    const lh_timestamp_t shifted = lh_timestamp_sub_days(lh_timestamp_from_date(&date), 1);

    EXPECT_EQ(lh_timestamp_get_month(shifted), 2);
    EXPECT_EQ(lh_timestamp_get_day(shifted), 28);
}

TEST(timestamp_add_months, clamps_day_on_shorter_month)
{
    const lh_date_t date = lh_date_initializer(2026, 1, 31);
    const lh_timestamp_t shifted = lh_timestamp_add_months(lh_timestamp_from_date(&date), 1);

    EXPECT_EQ(lh_timestamp_get_year(shifted), 2026);
    EXPECT_EQ(lh_timestamp_get_month(shifted), 2);
    EXPECT_EQ(lh_timestamp_get_day(shifted), 28);
}

TEST(timestamp_add_months, carries_into_next_year)
{
    const lh_date_t date = lh_date_initializer(2026, 12, 15);
    const lh_timestamp_t shifted = lh_timestamp_add_months(lh_timestamp_from_date(&date), 2);

    EXPECT_EQ(lh_timestamp_get_year(shifted), 2027);
    EXPECT_EQ(lh_timestamp_get_month(shifted), 2);
    EXPECT_EQ(lh_timestamp_get_day(shifted), 15);
}

TEST(timestamp_sub_months, borrows_from_previous_year)
{
    const lh_date_t date = lh_date_initializer(2027, 1, 15);
    const lh_timestamp_t shifted = lh_timestamp_sub_months(lh_timestamp_from_date(&date), 2);

    EXPECT_EQ(lh_timestamp_get_year(shifted), 2026);
    EXPECT_EQ(lh_timestamp_get_month(shifted), 11);
    EXPECT_EQ(lh_timestamp_get_day(shifted), 15);
}

TEST(timestamp_add_years, leap_day_clamps_on_non_leap_target)
{
    const lh_date_t date = lh_date_initializer(2024, 2, 29);
    const lh_timestamp_t shifted = lh_timestamp_add_years(lh_timestamp_from_date(&date), 1);

    EXPECT_EQ(lh_timestamp_get_year(shifted), 2025);
    EXPECT_EQ(lh_timestamp_get_month(shifted), 2);
    EXPECT_EQ(lh_timestamp_get_day(shifted), 28);
}

TEST(timestamp_sub_years, plain_year_step_back)
{
    const lh_date_t date = lh_date_initializer(2026, 9, 16);
    const lh_timestamp_t shifted = lh_timestamp_sub_years(lh_timestamp_from_date(&date), 3);

    EXPECT_EQ(lh_timestamp_get_year(shifted), 2023);
    EXPECT_EQ(lh_timestamp_get_month(shifted), 9);
    EXPECT_EQ(lh_timestamp_get_day(shifted), 16);
}

TEST(timestamp_diff, seconds_between_two_points)
{
    const lh_date_t date = lh_date_initializer(2026, 9, 16);
    const lh_timestamp_t self = lh_timestamp_from_date(&date);
    const lh_timestamp_t other = lh_timestamp_sub_days(self, 1);

    EXPECT_EQ(lh_timestamp_diff(self, other), 86400);
    EXPECT_EQ(lh_timestamp_diff(other, self), -86400);
}

TEST(timestamp_compare, equals_and_ordering)
{
    const lh_date_t date = lh_date_initializer(2026, 9, 16);
    const lh_timestamp_t self = lh_timestamp_from_date(&date);
    const lh_timestamp_t later = lh_timestamp_add_seconds(self, 1);

    EXPECT_EQ(lh_timestamp_equals(self, self), lh_bool_true);
    EXPECT_EQ(lh_timestamp_equals(self, later), lh_bool_false);
    EXPECT_EQ(lh_timestamp_is_at_least(later, self), lh_bool_true);
    EXPECT_EQ(lh_timestamp_is_less(self, later), lh_bool_true);
    EXPECT_EQ(lh_timestamp_is_greater(later, self), lh_bool_true);
    EXPECT_EQ(lh_timestamp_is_less(self, self), lh_bool_false);
}

TEST(timestamp_floor_div, rounds_toward_negative_infinity)
{
    EXPECT_EQ(lh_timestamp_floor_div(7, 2), 3);
    EXPECT_EQ(lh_timestamp_floor_div(-7, 2), -4);
    EXPECT_EQ(lh_timestamp_floor_div(-6, 2), -3);
    EXPECT_EQ(lh_timestamp_floor_div(6, 2), 3);
}

TEST(timestamp_floor_mod, always_has_sign_of_divisor)
{
    EXPECT_EQ(lh_timestamp_floor_mod(7, 2), 1);
    EXPECT_EQ(lh_timestamp_floor_mod(-7, 2), 1);
    EXPECT_EQ(lh_timestamp_floor_mod(-6, 2), 0);
}

TEST(timestamp_date_add_months, updates_date_in_place)
{
    lh_date_t date = lh_date_initializer(2026, 1, 31);

    EXPECT_EQ(lh_timestamp_date_add_months(&date, 1), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 28);
}

TEST(timestamp_date_sub_months, updates_date_in_place)
{
    lh_date_t date = lh_date_initializer(2026, 3, 31);

    EXPECT_EQ(lh_timestamp_date_sub_months(&date, 1), 0U);
    EXPECT_EQ(lh_date_get_month(&date), 2);
    EXPECT_EQ(lh_date_get_day(&date), 28);
}

} // namespace
