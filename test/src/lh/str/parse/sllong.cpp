#include <gtest/gtest.h>

#include <cstring>
#include <limits>

#include <lh/str/parse/sllong.h>

namespace
{

constexpr lh_sllong_t k_min = std::numeric_limits<lh_sllong_t>::min();
constexpr lh_sllong_t k_max = std::numeric_limits<lh_sllong_t>::max();

bool
parse(const char *text, lh_sllong_t *out, lh_sllong_t min = k_min, lh_sllong_t max = k_max)
{
    return lh_str_ptr_parse_sllong(text, std::strlen(text), min, max, out);
}

TEST(str_ptr_parse_sllong, parses_positive_and_negative)
{
    lh_sllong_t value = 0;
    EXPECT_TRUE(parse("15169", &value));
    EXPECT_EQ(value, 15169);
    EXPECT_TRUE(parse("-42", &value));
    EXPECT_EQ(value, -42);
}

TEST(str_ptr_parse_sllong, parses_both_ends_of_the_range)
{
    lh_sllong_t value = 0;
    EXPECT_TRUE(parse("9223372036854775807", &value));
    EXPECT_EQ(value, k_max);
    EXPECT_TRUE(parse("-9223372036854775808", &value));
    EXPECT_EQ(value, k_min);
}

TEST(str_ptr_parse_sllong, rejects_one_past_either_end)
{
    lh_sllong_t value = 7;
    EXPECT_FALSE(parse("9223372036854775808", &value));
    EXPECT_FALSE(parse("-9223372036854775809", &value));
    EXPECT_EQ(value, 7);
}

TEST(str_ptr_parse_sllong, minus_zero_is_zero)
{
    lh_sllong_t value = 7;
    EXPECT_TRUE(parse("-0", &value));
    EXPECT_EQ(value, 0);
}

TEST(str_ptr_parse_sllong, keeps_to_min_and_max)
{
    lh_sllong_t value = 7;
    EXPECT_TRUE(parse("10", &value, -10, 10));
    EXPECT_TRUE(parse("-10", &value, -10, 10));
    EXPECT_FALSE(parse("11", &value, -10, 10));
    EXPECT_FALSE(parse("-11", &value, -10, 10));
    EXPECT_FALSE(parse("-1", &value, 0, 10));
    EXPECT_FALSE(parse("1", &value, -10, -5));
    EXPECT_EQ(value, -10);
}

TEST(str_ptr_parse_sllong, rejects_malformed_text)
{
    lh_sllong_t value = 7;
    EXPECT_FALSE(lh_str_ptr_parse_sllong("", 0, k_min, k_max, &value));
    EXPECT_FALSE(parse("-", &value));
    EXPECT_FALSE(parse("+1", &value));
    EXPECT_FALSE(parse("--1", &value));
    EXPECT_FALSE(parse("-01", &value));
    EXPECT_FALSE(parse("01", &value));
    EXPECT_FALSE(parse("1e3", &value));
    EXPECT_FALSE(parse(" 1", &value));
    EXPECT_EQ(value, 7);
}

} // namespace
