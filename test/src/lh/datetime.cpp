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

} // namespace
