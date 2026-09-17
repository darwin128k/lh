#include <gtest/gtest.h>

#include <lh/str/parse/uint.h>

namespace
{

TEST(str_ptr_parse_uint, parses_single_digit)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_uint("7", 1, 255U, &value));
    EXPECT_EQ(value, 7U);
}

TEST(str_ptr_parse_uint, parses_multi_digit)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_uint("255", 3, 255U, &value));
    EXPECT_EQ(value, 255U);
}

TEST(str_ptr_parse_uint, parses_lone_zero)
{
    lh_uint_t value = 123U;
    EXPECT_TRUE(lh_str_ptr_parse_uint("0", 1, 255U, &value));
    EXPECT_EQ(value, 0U);
}

TEST(str_ptr_parse_uint, rejects_empty_input)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_uint("", 0, 255U, &value));
}

TEST(str_ptr_parse_uint, rejects_leading_zero)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_uint("080", 3, 255U, &value));
}

TEST(str_ptr_parse_uint_digits, allows_leading_zero)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_uint_digits("09", 2, 255U, &value));
    EXPECT_EQ(value, 9U);
}

TEST(str_ptr_parse_uint, rejects_value_above_max)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_uint("256", 3, 255U, &value));
}

TEST(str_ptr_parse_uint, rejects_non_digit)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_uint("12a", 3, 255U, &value));
}

TEST(str_ptr_parse_uint, rejects_overflow_of_underlying_type)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_uint("99999999999999999999", 20, 255U, &value));
}

TEST(str_ptr_parse_uint, accepts_full_uint_range_with_max_as_type_limit)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_uint("4294967295", 10, 4294967295U, &value));
    EXPECT_EQ(value, 4294967295U);
}

} // namespace
