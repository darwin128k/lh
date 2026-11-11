#include <gtest/gtest.h>

#include <lh/core/char/word.h>

namespace
{

TEST(char_is_word, accepts_letters_digits_and_underscore)
{
    EXPECT_TRUE(lh_char_is_word('a'));
    EXPECT_TRUE(lh_char_is_word('Z'));
    EXPECT_TRUE(lh_char_is_word('0'));
    EXPECT_TRUE(lh_char_is_word('9'));
    EXPECT_TRUE(lh_char_is_word(lh_char_map_underscore));
}

TEST(char_is_word, rejects_other_chars)
{
    EXPECT_FALSE(lh_char_is_word(lh_char_map_nul));
    EXPECT_FALSE(lh_char_is_word('-'));
    EXPECT_FALSE(lh_char_is_word('.'));
    EXPECT_FALSE(lh_char_is_word('{'));
    EXPECT_FALSE(lh_char_is_word(' '));
    EXPECT_FALSE(lh_char_is_word(static_cast<lh_char_t>(0xC3)));
}

} // namespace
