#include <gtest/gtest.h>

#include <lh/char/dot.h>
#include <lh/char/map.h>

namespace
{

TEST(char_is_dot, accepts_period)
{
    EXPECT_TRUE(lh_char_is_dot(lh_char_map_period));
}

TEST(char_is_dot, rejects_other_chars)
{
    EXPECT_FALSE(lh_char_is_dot(lh_char_map_nul));
    EXPECT_FALSE(lh_char_is_dot('a'));
    EXPECT_FALSE(lh_char_is_dot('/'));
    EXPECT_FALSE(lh_char_is_dot('\\'));
}

} // namespace
