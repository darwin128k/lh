#include <gtest/gtest.h>

#include <string>

#include <lh/date.h>
#include <lh/date/initializer.h>

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

} // namespace
