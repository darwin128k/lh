#include <gtest/gtest.h>

#include <lh/core/util/char/case/map.h>

namespace
{

lh_char_t
byte(int v)
{
    return static_cast<lh_char_t>(static_cast<unsigned char>(v));
}

TEST(char_to_lower, maps_ascii_letters_and_leaves_the_rest)
{
    for (int c = 0; c < 128; ++c)
    {
        const int want = (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
        EXPECT_EQ(lh_char_to_lower(byte(c)), byte(want)) << c;
    }
}

TEST(char_to_upper, maps_ascii_letters_and_leaves_the_rest)
{
    for (int c = 0; c < 128; ++c)
    {
        const int want = (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
        EXPECT_EQ(lh_char_to_upper(byte(c)), byte(want)) << c;
    }
}

TEST(char_to_lower, maps_latin1_capitals)
{
    EXPECT_EQ(lh_char_to_lower(byte(0xC0)), byte(0xE0)); /* A WITH GRAVE */
    EXPECT_EQ(lh_char_to_lower(byte(0xDE)), byte(0xFE)); /* THORN */
    EXPECT_EQ(lh_char_to_lower(byte(0xD7)), byte(0xD7)); /* MULTIPLICATION SIGN */
    EXPECT_EQ(lh_char_to_lower(byte(0xE0)), byte(0xE0)); /* already lower */
    EXPECT_EQ(lh_char_to_lower(byte(0x80)), byte(0x80));
}

TEST(char_to_upper, maps_latin1_smalls_that_stay_in_one_byte)
{
    EXPECT_EQ(lh_char_to_upper(byte(0xE0)), byte(0xC0));
    EXPECT_EQ(lh_char_to_upper(byte(0xFE)), byte(0xDE));
    EXPECT_EQ(lh_char_to_upper(byte(0xF7)), byte(0xF7)); /* DIVISION SIGN */
    EXPECT_EQ(lh_char_to_upper(byte(0xFF)), byte(0xFF)); /* Y WITH DIAERESIS: U+0178 */
    EXPECT_EQ(lh_char_to_upper(byte(0xB5)), byte(0xB5)); /* MICRO SIGN: U+039C */
}

TEST(char_to_lower, nonascii_agrees_with_the_inline_form)
{
    for (int c = 128; c < 256; ++c)
    {
        EXPECT_EQ(lh_char_to_lower(byte(c)), lh_char_to_lower_nonascii(byte(c))) << c;
        EXPECT_EQ(lh_char_to_upper(byte(c)), lh_char_to_upper_nonascii(byte(c))) << c;
    }
}

} // namespace
