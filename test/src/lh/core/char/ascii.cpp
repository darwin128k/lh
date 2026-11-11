#include <gtest/gtest.h>

#include <lh/core/char/ascii.h>

TEST(char_is_ascii, every_byte_up_to_del)
{
    for (int c = 0; c <= 0x7F; ++c)
    {
        EXPECT_TRUE(lh_char_is_ascii(static_cast<lh_char_t>(c))) << c;
    }
}

TEST(char_is_ascii, no_byte_from_0x80)
{
    for (int c = 0x80; c <= 0xFF; ++c)
    {
        EXPECT_FALSE(lh_char_is_ascii(static_cast<lh_char_t>(c))) << c;
    }
}
