#include <gtest/gtest.h>

#include <lh/http/char.h>

#include <cstring>

namespace
{

TEST(http_is_token_char, letters_digits_and_the_rfc_symbols)
{
    for (int c = 0; c < 256; ++c)
    {
        const bool expected = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                              (c >= '0' && c <= '9') ||
                              (c != 0 && std::strchr("!#$%&'*+-.^_`|~", c) != nullptr);
        EXPECT_EQ(lh_http_is_token_char(static_cast<lh_char_t>(c)), expected) << c;
    }
}

TEST(http_is_target_char, visible_ascii_only)
{
    for (int c = 0; c < 256; ++c)
    {
        EXPECT_EQ(lh_http_is_target_char(static_cast<lh_char_t>(c)), c >= 0x21 && c <= 0x7e)
            << c;
    }
}

TEST(http_is_field_char, tab_space_visible_and_non_ascii)
{
    for (int c = 0; c < 256; ++c)
    {
        EXPECT_EQ(lh_http_is_field_char(static_cast<lh_char_t>(c)),
                  c == '\t' || (c >= 0x20 && c != 0x7f))
            << c;
    }
}

} // namespace
