#include <gtest/gtest.h>

#include <lh/math/point.h>
#include <lh/math/size.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_size, is_empty_for_zero_or_negative)
{
    const lh_math_size_t zero = lh_math_size_make_empty();
    const lh_math_size_t neg = lh_math_size_make(-1, 4);
    const lh_math_size_t ok = lh_math_size_make(2, 3);
    EXPECT_TRUE(lh_math_size_is_empty(lh_addr_of(zero)));
    EXPECT_TRUE(lh_math_size_is_empty(lh_addr_of(neg)));
    EXPECT_FALSE(lh_math_size_is_empty(lh_addr_of(ok)));
}

TEST(math_size, from_extent_is_delta)
{
    const lh_math_point_t min = lh_math_point_make(1, 2);
    const lh_math_point_t max = lh_math_point_make(6, 5);
    const lh_math_size_t size = lh_math_size_from_extent(lh_addr_of(min), lh_addr_of(max));
    EXPECT_EQ(lh_math_size_get_width(lh_addr_of(size)), 5);
    EXPECT_EQ(lh_math_size_get_height(lh_addr_of(size)), 3);
}

TEST(math_size, inset_shrinks_both_sides)
{
    const lh_math_size_t size = lh_math_size_make(10, 8);
    const lh_math_size_t inner = lh_math_size_inset(lh_addr_of(size), 1, 2);
    EXPECT_EQ(lh_math_size_get_width(lh_addr_of(inner)), 8);
    EXPECT_EQ(lh_math_size_get_height(lh_addr_of(inner)), 4);
}

} // namespace
