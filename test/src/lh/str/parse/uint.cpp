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

TEST(str_ptr_parse_uint_prefix, stops_at_first_non_digit)
{
    lh_uint_t value = 0;
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("8080/tcp", 8, 65535U, &value), 4u);
    EXPECT_EQ(value, 8080u);
}

TEST(str_ptr_parse_uint_prefix, whole_buffer_and_size_limit)
{
    lh_uint_t value = 0;
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("123", 3, LH_UINT_T_MAX, &value), 3u);
    EXPECT_EQ(value, 123u);
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("12345", 2, LH_UINT_T_MAX, &value), 2u);
    EXPECT_EQ(value, 12u);
}

TEST(str_ptr_parse_uint_prefix, failures_return_zero_and_keep_out)
{
    lh_uint_t value = 77;
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("x1", 2, LH_UINT_T_MAX, &value), 0u);
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("", 0, LH_UINT_T_MAX, &value), 0u);
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("007", 3, LH_UINT_T_MAX, &value), 0u);
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("300;", 4, 255U, &value), 0u);
    EXPECT_EQ(value, 77u);
}

TEST(str_ptr_parse_uint_prefix, single_zero_is_fine)
{
    lh_uint_t value = 77;
    EXPECT_EQ(lh_str_ptr_parse_uint_prefix("0x", 2, LH_UINT_T_MAX, &value), 1u);
    EXPECT_EQ(value, 0u);
}

} // namespace
