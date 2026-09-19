#include <gtest/gtest.h>

#include <lh/util/str/ptr/dot.h>

namespace
{

TEST(str_ptr_is_dot, accepts_dot)
{
    EXPECT_TRUE(lh_str_ptr_is_dot("."));
}

TEST(str_ptr_is_dot, rejects_other_names)
{
    EXPECT_FALSE(lh_str_ptr_is_dot(""));
    EXPECT_FALSE(lh_str_ptr_is_dot(".."));
    EXPECT_FALSE(lh_str_ptr_is_dot("..."));
    EXPECT_FALSE(lh_str_ptr_is_dot(".a"));
    EXPECT_FALSE(lh_str_ptr_is_dot("a."));
    EXPECT_FALSE(lh_str_ptr_is_dot("foo"));
}

TEST(str_ptr_is_double_dot, accepts_double_dot)
{
    EXPECT_TRUE(lh_str_ptr_is_double_dot(".."));
}

TEST(str_ptr_is_double_dot, rejects_other_names)
{
    EXPECT_FALSE(lh_str_ptr_is_double_dot(""));
    EXPECT_FALSE(lh_str_ptr_is_double_dot("."));
    EXPECT_FALSE(lh_str_ptr_is_double_dot("..."));
    EXPECT_FALSE(lh_str_ptr_is_double_dot(".a"));
    EXPECT_FALSE(lh_str_ptr_is_double_dot("a."));
    EXPECT_FALSE(lh_str_ptr_is_double_dot("foo"));
}

} // namespace
