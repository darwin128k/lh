#include <gtest/gtest.h>

#include <lh/char/map.h>
#include <lh/char/slash.h>

namespace
{

TEST(char_is_slash, accepts_solidus)
{
    EXPECT_TRUE(lh_char_is_slash(lh_char_map_slash));
}

TEST(char_is_slash, rejects_backslash_and_other)
{
    EXPECT_FALSE(lh_char_is_slash(lh_char_map_backslash));
    EXPECT_FALSE(lh_char_is_slash(lh_char_map_period));
    EXPECT_FALSE(lh_char_is_slash('a'));
}

TEST(char_is_backslash, accepts_reverse_solidus)
{
    EXPECT_TRUE(lh_char_is_backslash(lh_char_map_backslash));
}

TEST(char_is_backslash, rejects_slash_and_other)
{
    EXPECT_FALSE(lh_char_is_backslash(lh_char_map_slash));
    EXPECT_FALSE(lh_char_is_backslash(lh_char_map_period));
    EXPECT_FALSE(lh_char_is_backslash('a'));
}

} // namespace
