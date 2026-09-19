#include <gtest/gtest.h>

#include <lh/char/letter.h>

namespace
{

TEST(char_is_letter, accepts_ascii_letters)
{
    for (lh_char_t c = 'A'; c <= 'Z'; c++)
    {
        EXPECT_TRUE(lh_char_is_letter(c));
    }
    for (lh_char_t c = 'a'; c <= 'z'; c++)
    {
        EXPECT_TRUE(lh_char_is_letter(c));
    }
}

TEST(char_is_letter, rejects_non_letters)
{
    EXPECT_FALSE(lh_char_is_letter('0'));
    EXPECT_FALSE(lh_char_is_letter('9'));
    EXPECT_FALSE(lh_char_is_letter('@'));
    EXPECT_FALSE(lh_char_is_letter('['));
    EXPECT_FALSE(lh_char_is_letter('`'));
    EXPECT_FALSE(lh_char_is_letter('{'));
    EXPECT_FALSE(lh_char_is_letter(' '));
    EXPECT_FALSE(lh_char_is_letter(':'));
}

} // namespace
