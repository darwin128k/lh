#include <gtest/gtest.h>

#include <lh/str/parse/text.h>

namespace
{

TEST(str_ptr_parse_text, parses_unsigned_triple)
{
    lh_uint_t a = 0;
    lh_uint_t b = 0;
    lh_uint_t c = 0;
    ASSERT_TRUE(lh_str_ptr_parse_text("1.2.3", 5, "%u.%u.%u", &a, &b, &c));
    EXPECT_EQ(a, 1U);
    EXPECT_EQ(b, 2U);
    EXPECT_EQ(c, 3U);
}

TEST(str_ptr_parse_text, rejects_leading_zeros_on_uint)
{
    lh_uint_t a = 9;
    lh_uint_t b = 9;
    lh_uint_t c = 9;
    EXPECT_FALSE(lh_str_ptr_parse_text("01.2.3", 6, "%u.%u.%u", &a, &b, &c));
}

TEST(str_ptr_parse_text, rejects_leftover_input)
{
    lh_uint_t a = 0;
    lh_uint_t b = 0;
    lh_uint_t c = 0;
    EXPECT_FALSE(lh_str_ptr_parse_text("1.2.3.4", 7, "%u.%u.%u", &a, &b, &c));
}

TEST(str_ptr_parse_text, rejects_short_input)
{
    lh_uint_t a = 0;
    lh_uint_t b = 0;
    lh_uint_t c = 0;
    EXPECT_FALSE(lh_str_ptr_parse_text("1.2", 3, "%u.%u.%u", &a, &b, &c));
}

TEST(str_ptr_parse_text, matches_percent_literal)
{
    lh_uint_t value = 0;
    ASSERT_TRUE(lh_str_ptr_parse_text("100%", 4, "%u%%", &value));
    EXPECT_EQ(value, 100U);
}

TEST(str_ptr_parse_text, parses_char)
{
    lh_char_t ch = 0;
    ASSERT_TRUE(lh_str_ptr_parse_text("x", 1, "%c", &ch));
    EXPECT_EQ(ch, 'x');
}

TEST(str_ptr_parse_text, parses_signed)
{
    lh_sint_t value = 0;
    ASSERT_TRUE(lh_str_ptr_parse_text("-42", 3, "%d", &value));
    EXPECT_EQ(value, -42);
}

TEST(str_ptr_parse_text, parses_hex)
{
    lh_uint_t value = 0;
    ASSERT_TRUE(lh_str_ptr_parse_text("aB", 2, "%x", &value));
    EXPECT_EQ(value, 0xABU);
}

TEST(str_ptr_parse_text, rejects_unknown_conversion)
{
    lh_uint_t value = 0;
    EXPECT_FALSE(lh_str_ptr_parse_text("1", 1, "%q", &value));
}

} // namespace
