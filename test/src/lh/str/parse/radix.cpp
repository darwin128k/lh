#include <gtest/gtest.h>

#include <lh/numeric/limits.h>
#include <lh/str/parse/radix.h>

namespace
{

TEST(str_ptr_count_radix_digits, stops_at_first_non_digit_of_base)
{
    EXPECT_EQ(lh_str_ptr_count_radix_digits("1012", 4, 2U), 3U);
    EXPECT_EQ(lh_str_ptr_count_radix_digits("09af;", 5, 10U), 2U);
    EXPECT_EQ(lh_str_ptr_count_radix_digits("09af;", 5, 16U), 4U);
    EXPECT_EQ(lh_str_ptr_count_radix_digits("x", 1, 16U), 0U);
    EXPECT_EQ(lh_str_ptr_count_radix_digits("ff", 1, 16U), 1U);
}

TEST(str_ptr_parse_radix, parses_in_each_base)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_radix("101", 3, 2U, LH_UINT_T_MAX, &value));
    EXPECT_EQ(value, 5U);
    EXPECT_TRUE(lh_str_ptr_parse_radix("777", 3, 8U, LH_UINT_T_MAX, &value));
    EXPECT_EQ(value, 511U);
    EXPECT_TRUE(lh_str_ptr_parse_radix("0042", 4, 10U, LH_UINT_T_MAX, &value));
    EXPECT_EQ(value, 42U);
    EXPECT_TRUE(lh_str_ptr_parse_radix("FfFf", 4, 16U, LH_UINT_T_MAX, &value));
    EXPECT_EQ(value, 0xFFFFU);
}

TEST(str_ptr_parse_radix, rejects_digits_above_base_and_leaves_out)
{
    lh_uint_t value = 7U;
    EXPECT_FALSE(lh_str_ptr_parse_radix("12", 2, 2U, LH_UINT_T_MAX, &value));
    EXPECT_FALSE(lh_str_ptr_parse_radix("1a", 2, 10U, LH_UINT_T_MAX, &value));
    EXPECT_FALSE(lh_str_ptr_parse_radix("", 0, 10U, LH_UINT_T_MAX, &value));
    EXPECT_EQ(value, 7U);
}

TEST(str_ptr_parse_radix, rejects_above_max_and_overflow)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_radix("256", 3, 10U, 255U, &value));
    EXPECT_FALSE(lh_str_ptr_parse_radix("100000000", 9, 16U, LH_UINT_T_MAX, &value));
    EXPECT_TRUE(lh_str_ptr_parse_radix("ffffffff", 8, 16U, LH_UINT_T_MAX, &value));
    EXPECT_EQ(value, LH_UINT_T_MAX);
}

} // namespace
