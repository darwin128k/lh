#include <gtest/gtest.h>

#include <cstring>

#include <lh/str/parse/ullong.h>

namespace
{

bool
parse(const char *text, lh_ullong_t max, lh_ullong_t *out)
{
    return lh_str_ptr_parse_ullong(text, std::strlen(text), max, out);
}

TEST(str_ptr_parse_ullong, parses_past_32_bits)
{
    lh_ullong_t value = 0U;
    EXPECT_TRUE(parse("4294967296", ~0ULL, &value));
    EXPECT_EQ(value, 4294967296ULL);
}

TEST(str_ptr_parse_ullong, parses_the_largest_value)
{
    lh_ullong_t value = 0U;
    EXPECT_TRUE(parse("18446744073709551615", ~0ULL, &value));
    EXPECT_EQ(value, ~0ULL);
}

TEST(str_ptr_parse_ullong, rejects_one_past_the_largest_value)
{
    lh_ullong_t value = 7U;
    EXPECT_FALSE(parse("18446744073709551616", ~0ULL, &value));
    EXPECT_FALSE(parse("99999999999999999999", ~0ULL, &value));
    EXPECT_EQ(value, 7U);
}

TEST(str_ptr_parse_ullong, rejects_above_max)
{
    lh_ullong_t value = 7U;
    EXPECT_FALSE(parse("256", 255U, &value));
    EXPECT_EQ(value, 7U);
}

TEST(str_ptr_parse_ullong, parses_lone_zero_but_not_padding)
{
    lh_ullong_t value = 7U;
    EXPECT_TRUE(parse("0", ~0ULL, &value));
    EXPECT_EQ(value, 0U);
    EXPECT_FALSE(parse("007", ~0ULL, &value));
}

TEST(str_ptr_parse_ullong, digits_accepts_padding)
{
    lh_ullong_t value = 0U;
    EXPECT_TRUE(lh_str_ptr_parse_ullong_digits("007", 3, ~0ULL, &value));
    EXPECT_EQ(value, 7U);
}

TEST(str_ptr_parse_ullong, rejects_empty_sign_and_trailing_text)
{
    lh_ullong_t value = 7U;
    EXPECT_FALSE(lh_str_ptr_parse_ullong("", 0, ~0ULL, &value));
    EXPECT_FALSE(parse("-1", ~0ULL, &value));
    EXPECT_FALSE(parse("+1", ~0ULL, &value));
    EXPECT_FALSE(parse("12x", ~0ULL, &value));
    EXPECT_FALSE(parse("1.0", ~0ULL, &value));
    EXPECT_EQ(value, 7U);
}

} // namespace
