#include <gtest/gtest.h>

#include <lh/char/digit.h>

namespace
{

TEST(char_is_digit, accepts_decimal_digits)
{
    for (lh_char_t c = '0'; c <= '9'; c++)
    {
        EXPECT_TRUE(lh_char_is_digit(c));
    }
}

TEST(char_is_digit, rejects_non_digits)
{
    EXPECT_FALSE(lh_char_is_digit('a'));
    EXPECT_FALSE(lh_char_is_digit('/'));
    EXPECT_FALSE(lh_char_is_digit(':'));
    EXPECT_FALSE(lh_char_is_digit(' '));
}

TEST(char_to_digit, converts_all_digits)
{
    EXPECT_EQ(lh_char_to_digit('0'), 0);
    EXPECT_EQ(lh_char_to_digit('5'), 5);
    EXPECT_EQ(lh_char_to_digit('9'), 9);
}

TEST(char_from_digit, converts_all_values)
{
    EXPECT_EQ(lh_char_from_digit(0), '0');
    EXPECT_EQ(lh_char_from_digit(5), '5');
    EXPECT_EQ(lh_char_from_digit(9), '9');
}

TEST(char_digit_accumulate, builds_value_left_to_right)
{
    lh_uint_t value = 0;
    ASSERT_TRUE(lh_char_digit_accumulate(&value, 1));
    ASSERT_TRUE(lh_char_digit_accumulate(&value, 2));
    ASSERT_TRUE(lh_char_digit_accumulate(&value, 3));
    EXPECT_EQ(value, 123U);
}

TEST(char_digit_accumulate, rejects_overflow)
{
    lh_uint_t value = 4294967295U; /* LH_U32_T_MAX */
    EXPECT_FALSE(lh_char_digit_accumulate(&value, 1));
    EXPECT_EQ(value, 4294967295U); /* unchanged on overflow */
}

TEST(char_digit_extract, pulls_digits_right_to_left)
{
    lh_uint_t value = 123U;
    EXPECT_EQ(lh_char_digit_extract(&value, 10U), 3U);
    EXPECT_EQ(value, 12U);
    EXPECT_EQ(lh_char_digit_extract(&value, 10U), 2U);
    EXPECT_EQ(value, 1U);
    EXPECT_EQ(lh_char_digit_extract(&value, 10U), 1U);
    EXPECT_EQ(value, 0U);
}

TEST(char_digit_extract, is_inverse_of_accumulate)
{
    lh_uint_t value = 0;
    lh_char_digit_accumulate(&value, 7);
    lh_char_digit_accumulate(&value, 8);
    EXPECT_EQ(lh_char_digit_extract(&value, 10U), 8U);
    EXPECT_EQ(lh_char_digit_extract(&value, 10U), 7U);
}

TEST(char_digit, roundtrips)
{
    for (lh_uchar_t d = 0; d <= 9; d++)
    {
        EXPECT_EQ(lh_char_to_digit(lh_char_from_digit(d)), d);
    }
}

} // namespace
