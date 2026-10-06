#include <gtest/gtest.h>

#include <lh/math/fpoint.h>
#include <lh/math/fsize.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_fsize, is_empty_and_from_extent)
{
    const lh_math_fsize_t zero = lh_math_fsize_make_empty();
    const lh_math_fsize_t neg = lh_math_fsize_make(-1.f, 4.f);
    EXPECT_TRUE(lh_math_fsize_is_empty(lh_addr_of(zero)));
    EXPECT_TRUE(lh_math_fsize_is_empty(lh_addr_of(neg)));

    const lh_math_fpoint_t min = lh_math_fpoint_make(1.f, 2.f);
    const lh_math_fpoint_t max = lh_math_fpoint_make(6.f, 5.f);
    const lh_math_fsize_t size = lh_math_fsize_from_extent(lh_addr_of(min), lh_addr_of(max));
    EXPECT_EQ(lh_math_fsize_get_width(lh_addr_of(size)), 5.f);
    EXPECT_EQ(lh_math_fsize_get_height(lh_addr_of(size)), 3.f);
}

TEST(math_fsize, inset_shrinks_both_sides)
{
    const lh_math_fsize_t size = lh_math_fsize_make(10.f, 8.f);
    const lh_math_fsize_t inner = lh_math_fsize_inset(lh_addr_of(size), 1.f, 2.f);
    EXPECT_EQ(lh_math_fsize_get_width(lh_addr_of(inner)), 8.f);
    EXPECT_EQ(lh_math_fsize_get_height(lh_addr_of(inner)), 4.f);
}

} /* namespace */
