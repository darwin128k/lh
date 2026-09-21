#include <gtest/gtest.h>

#include <string>

#include <lh/date/initializer.h>
#include <lh/datetime.h>
#include <lh/datetime/initializer.h>
#include <lh/time/initializer.h>

namespace
{

TEST(datetime_roundtrip, parse_format)
{
    lh_datetime_t dt{};
    ASSERT_EQ(lh_datetime_parse("2026/09/16 09:05:00", 19, &dt), lh_bool_true);
    EXPECT_EQ(lh_date_get_year(&dt.date), 2026);
    EXPECT_EQ(lh_time_get_hour(&dt.time), 9);

    lh_char_t buf[LH_DATETIME_TEXT_MAX];
    const lh_usize_t n = lh_datetime_format(&dt, buf, sizeof(buf));
    EXPECT_EQ(std::string(buf, n), "2026/09/16 09:05:00");
}

TEST(datetime_is_at_least, later_time_same_date)
{
    const lh_datetime_t a = lh_datetime_initializer(lh_date_initializer(2026, 9, 16),
                                                    lh_time_initializer(10, 0, 0));
    const lh_datetime_t b = lh_datetime_initializer(lh_date_initializer(2026, 9, 16),
                                                    lh_time_initializer(9, 0, 0));
    EXPECT_EQ(lh_datetime_is_at_least(&a, &b), lh_bool_true);
}

TEST(datetime_compare, less_greater)
{
    const lh_datetime_t earlier = lh_datetime_initializer(lh_date_initializer(2026, 9, 16),
                                                          lh_time_initializer(23, 0, 0));
    const lh_datetime_t later = lh_datetime_initializer(lh_date_initializer(2026, 9, 17),
                                                        lh_time_initializer(0, 0, 0));

    EXPECT_EQ(lh_datetime_is_less(&earlier, &later), lh_bool_true);
    EXPECT_EQ(lh_datetime_is_greater(&later, &earlier), lh_bool_true);
    EXPECT_EQ(lh_datetime_is_less(&earlier, &earlier), lh_bool_false);
    EXPECT_EQ(lh_datetime_is_at_least(&later, &earlier), lh_bool_true);
}

TEST(datetime_add, time_overflow_into_date)
{
    lh_datetime_t dt = lh_datetime_initializer(lh_date_initializer(2026, 9, 16),
                                               lh_time_initializer(23, 0, 0));
    const lh_datetime_t addend = lh_datetime_initializer(lh_date_initializer(0, 0, 0),
                                                         lh_time_initializer(2, 0, 0));

    EXPECT_EQ(lh_datetime_add(&dt, &addend), 0U);
    EXPECT_EQ(lh_date_get_day(&dt.date), 17);
    EXPECT_EQ(lh_time_get_hour(&dt.time), 1);
}

TEST(datetime_add, day_carry_applies_before_month_duration)
{
    // Order matters here: day-carry first lands on 31 Jan, +1 month then clamps
    // to 28 Feb. Applying the month duration first would clamp 30 Jan to 28 Feb
    // right away, and the day-carry would then push it to 1 Mar instead - a
    // different (wrong) answer. This pins the day-carry-first order.
    lh_datetime_t dt = lh_datetime_initializer(lh_date_initializer(2026, 1, 30),
                                               lh_time_initializer(23, 59, 0));
    const lh_datetime_t addend =
        lh_datetime_initializer(lh_date_initializer(0, 1, 0), lh_time_initializer(0, 2, 0));

    EXPECT_EQ(lh_datetime_add(&dt, &addend), 0U);
    EXPECT_EQ(lh_date_get_month(&dt.date), 2);
    EXPECT_EQ(lh_date_get_day(&dt.date), 28);
    EXPECT_EQ(lh_time_get_hour(&dt.time), 0);
    EXPECT_EQ(lh_time_get_minute(&dt.time), 1);
}

TEST(datetime_sub, time_borrow_from_date)
{
    lh_datetime_t dt = lh_datetime_initializer(lh_date_initializer(2026, 9, 17),
                                               lh_time_initializer(1, 0, 0));
    const lh_datetime_t subtrahend = lh_datetime_initializer(lh_date_initializer(0, 0, 0),
                                                             lh_time_initializer(2, 0, 0));

    EXPECT_EQ(lh_datetime_sub(&dt, &subtrahend), 0U);
    EXPECT_EQ(lh_date_get_day(&dt.date), 16);
    EXPECT_EQ(lh_time_get_hour(&dt.time), 23);
}

TEST(datetime_sub, day_borrow_applies_after_month_duration)
{
    // Mirror of datetime_add's order-sensitive case: the month duration must
    // land BEFORE the day-borrow from the time-of-day (31 Mar -> -1 month ->
    // clamped to 28 Feb -> -1 day (borrow) -> 27 Feb). Borrowing the day
    // first would give 30 Mar -> -1 month -> clamped to 28 Feb instead - a
    // different (wrong) answer.
    lh_datetime_t dt = lh_datetime_initializer(lh_date_initializer(2026, 3, 31),
                                               lh_time_initializer(0, 0, 30));
    const lh_datetime_t subtrahend =
        lh_datetime_initializer(lh_date_initializer(0, 1, 0), lh_time_initializer(0, 1, 0));

    EXPECT_EQ(lh_datetime_sub(&dt, &subtrahend), 0U);
    EXPECT_EQ(lh_date_get_month(&dt.date), 2);
    EXPECT_EQ(lh_date_get_day(&dt.date), 27);
    EXPECT_EQ(lh_time_get_hour(&dt.time), 23);
    EXPECT_EQ(lh_time_get_minute(&dt.time), 59);
    EXPECT_EQ(lh_time_get_second(&dt.time), 30);
}

} // namespace
