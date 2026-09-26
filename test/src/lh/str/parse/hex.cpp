#include <gtest/gtest.h>

#include <lh/str/parse/hex.h>

namespace
{

TEST(str_ptr_parse_hex, parses_single_digit)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_hex("a", 1, 255U, &value));
    EXPECT_EQ(value, 10U);
}

TEST(str_ptr_parse_hex, parses_mixed_case_and_leading_zeros)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_hex("00FF", 4, 255U, &value));
    EXPECT_EQ(value, 255U);
}

TEST(str_ptr_parse_hex, parses_uppercase)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_hex("DEAD", 4, 0xFFFFU, &value));
    EXPECT_EQ(value, 0xDEADU);
}

TEST(str_ptr_parse_hex, rejects_empty_input)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_hex("", 0, 255U, &value));
}

TEST(str_ptr_parse_hex, rejects_value_above_max)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_hex("100", 3, 255U, &value));
}

TEST(str_ptr_parse_hex, rejects_non_digit)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_hex("12g", 3, 255U, &value));
}

TEST(str_ptr_parse_hex, rejects_overflow_of_underlying_type)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_hex("100000000", 9, 0xFFFFFFFFU, &value));
}

TEST(str_ptr_parse_hex, accepts_full_uint_range)
{
    lh_uint_t value = 0;
    EXPECT_TRUE(lh_str_ptr_parse_hex("FFFFFFFF", 8, 0xFFFFFFFFU, &value));
    EXPECT_EQ(value, 0xFFFFFFFFU);
}

TEST(str_ptr_parse_hex_prefix, stops_at_first_non_hex_digit)
{
    lh_uint_t value = 0;
    EXPECT_EQ(lh_str_ptr_parse_hex_prefix("ff;", 3, LH_UINT_T_MAX, &value), 2u);
    EXPECT_EQ(value, 255u);
}

TEST(str_ptr_parse_hex_prefix, leading_zeros_and_mixed_case)
{
    lh_uint_t value = 0;
    EXPECT_EQ(lh_str_ptr_parse_hex_prefix("00Ab", 4, LH_UINT_T_MAX, &value), 4u);
    EXPECT_EQ(value, 0xABu);
}

TEST(str_ptr_parse_hex_prefix, failures_return_zero_and_keep_out)
{
    lh_uint_t value = 77;
    EXPECT_EQ(lh_str_ptr_parse_hex_prefix("g1", 2, LH_UINT_T_MAX, &value), 0u);
    EXPECT_EQ(lh_str_ptr_parse_hex_prefix("100", 3, 0xFFU, &value), 0u);
    EXPECT_EQ(value, 77u);
}

} // namespace
