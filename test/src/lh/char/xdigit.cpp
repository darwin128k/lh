#include <gtest/gtest.h>

#include <lh/char/xdigit.h>

namespace
{

TEST(char_is_xdigit, accepts_hex_digits)
{
    for (lh_char_t c = '0'; c <= '9'; c++)
    {
        EXPECT_TRUE(lh_char_is_xdigit(c));
    }
    for (lh_char_t c = 'a'; c <= 'f'; c++)
    {
        EXPECT_TRUE(lh_char_is_xdigit(c));
    }
    for (lh_char_t c = 'A'; c <= 'F'; c++)
    {
        EXPECT_TRUE(lh_char_is_xdigit(c));
    }
}

TEST(char_is_xdigit, rejects_non_digits)
{
    EXPECT_FALSE(lh_char_is_xdigit('g'));
    EXPECT_FALSE(lh_char_is_xdigit('G'));
    EXPECT_FALSE(lh_char_is_xdigit('/'));
    EXPECT_FALSE(lh_char_is_xdigit(' '));
}

TEST(char_to_xdigit, converts_mixed_case)
{
    EXPECT_EQ(lh_char_to_xdigit('0'), 0);
    EXPECT_EQ(lh_char_to_xdigit('9'), 9);
    EXPECT_EQ(lh_char_to_xdigit('a'), 10);
    EXPECT_EQ(lh_char_to_xdigit('f'), 15);
    EXPECT_EQ(lh_char_to_xdigit('A'), 10);
    EXPECT_EQ(lh_char_to_xdigit('F'), 15);
}

TEST(char_from_xdigit, converts_both_cases)
{
    EXPECT_EQ(lh_char_from_xdigit(0, lh_bool_false), '0');
    EXPECT_EQ(lh_char_from_xdigit(10, lh_bool_false), 'a');
    EXPECT_EQ(lh_char_from_xdigit(15, lh_bool_true), 'F');
}

TEST(char_xdigit_accumulate, builds_value_left_to_right)
{
    lh_uint_t value = 0;
    ASSERT_TRUE(lh_char_xdigit_accumulate(&value, 0xA));
    ASSERT_TRUE(lh_char_xdigit_accumulate(&value, 0xB));
    EXPECT_EQ(value, 0xABU);
}

TEST(char_xdigit_accumulate, rejects_overflow)
{
    lh_uint_t value = 4294967295U;
    EXPECT_FALSE(lh_char_xdigit_accumulate(&value, 1));
    EXPECT_EQ(value, 4294967295U);
}

TEST(char_is_radix_digit, digits_below_the_base_only)
{
    EXPECT_TRUE(lh_char_is_radix_digit('1', 2U));
    EXPECT_FALSE(lh_char_is_radix_digit('2', 2U));
    EXPECT_TRUE(lh_char_is_radix_digit('9', 10U));
    EXPECT_FALSE(lh_char_is_radix_digit('a', 10U));
    EXPECT_TRUE(lh_char_is_radix_digit('F', 16U));
    EXPECT_FALSE(lh_char_is_radix_digit('g', 16U));
    EXPECT_FALSE(lh_char_is_radix_digit(' ', 16U));
}

TEST(char_digit_accumulate_radix, builds_value_in_base)
{
    lh_uint_t value = 0;
    ASSERT_TRUE(lh_char_digit_accumulate_radix(&value, 1, 8U));
    ASSERT_TRUE(lh_char_digit_accumulate_radix(&value, 7, 8U));
    EXPECT_EQ(value, 017U);
    value = 4294967295U;
    EXPECT_FALSE(lh_char_digit_accumulate_radix(&value, 0, 2U));
    EXPECT_EQ(value, 4294967295U);
}

TEST(char_xdigit, roundtrips)
{
    for (lh_byte_t d = 0; d <= 15; d++)
    {
        EXPECT_EQ(lh_char_to_xdigit(lh_char_from_xdigit(d, lh_bool_false)), d);
        EXPECT_EQ(lh_char_to_xdigit(lh_char_from_xdigit(d, lh_bool_true)), d);
    }
}

} // namespace
