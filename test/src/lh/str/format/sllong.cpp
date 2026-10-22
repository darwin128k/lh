#include <gtest/gtest.h>

#include <limits>
#include <string>

#include <lh/str/format/sllong.h>

namespace
{

std::string
format(lh_sllong_t value)
{
    lh_char_t buf[32];
    const lh_usize_t written = lh_str_ptr_format_sllong(value, buf, sizeof(buf));
    return std::string(buf, written);
}

TEST(str_ptr_format_sllong, formats_zero_positive_and_negative)
{
    EXPECT_EQ(format(0), "0");
    EXPECT_EQ(format(15169), "15169");
    EXPECT_EQ(format(-42), "-42");
}

TEST(str_ptr_format_sllong, formats_both_ends_of_the_range)
{
    EXPECT_EQ(format(std::numeric_limits<lh_sllong_t>::max()), "9223372036854775807");
    EXPECT_EQ(format(std::numeric_limits<lh_sllong_t>::min()), "-9223372036854775808");
}

TEST(str_ptr_format_sllong, writes_nothing_when_it_does_not_fit)
{
    lh_char_t buf[3];
    EXPECT_EQ(lh_str_ptr_format_sllong(-123, buf, sizeof(buf)), 0U);
    EXPECT_EQ(lh_str_ptr_format_sllong(-12, buf, sizeof(buf)), 3U);
    EXPECT_EQ(lh_str_ptr_format_sllong(-1, buf, 0U), 0U);
}

} // namespace
