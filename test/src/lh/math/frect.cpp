#include <gtest/gtest.h>
#include <type_traits>

#include <lh/math/frect.h>
#include <lh/math/rect.h>
#include <lh/math/scalar.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_frect, components_are_the_scalar)
{
    const lh_math_frect_t r = lh_math_frect_make(1, 2, 3, 4);
    using component = decltype(lh_math_frect_get_x(lh_addr_of(r)));
    EXPECT_TRUE((std::is_same<component, lh_math_scalar_t>::value));
}

TEST(math_frect, holds_origin_and_size)
{
    const lh_math_frect_t r = lh_math_frect_make(10, 20, 30, 40);
    EXPECT_EQ(lh_math_frect_get_x(lh_addr_of(r)), 10);
    EXPECT_EQ(lh_math_frect_get_y(lh_addr_of(r)), 20);
    EXPECT_EQ(lh_math_frect_get_size_width(lh_addr_of(r)), 30);
    EXPECT_EQ(lh_math_frect_get_size_height(lh_addr_of(r)), 40);
    EXPECT_FALSE(lh_math_frect_is_empty(lh_addr_of(r)));
}

TEST(math_frect, zero_size_is_empty)
{
    const lh_math_frect_t r = lh_math_frect_make_empty();
    EXPECT_TRUE(lh_math_frect_is_empty(lh_addr_of(r)));
    EXPECT_EQ(lh_math_frect_get_width(lh_addr_of(r)), 0);
    EXPECT_EQ(lh_math_frect_get_height(lh_addr_of(r)), 0);
}

TEST(math_frect, negative_extent_is_empty)
{
    const lh_math_frect_t r = lh_math_frect_make(1, 1, -4, 8);
    EXPECT_TRUE(lh_math_frect_is_empty(lh_addr_of(r)));
    EXPECT_EQ(lh_math_frect_get_width(lh_addr_of(r)), 0);
}

TEST(math_frect, roundtrip_through_rect)
{
    const lh_math_rect_t screen = lh_math_rect_make(-8, 15, 4, 9);
    const lh_math_frect_t r = lh_math_rect_to_frect(screen);
    const lh_math_rect_t back = lh_math_frect_to_rect(r);
    EXPECT_EQ(lh_math_rect_get_x(lh_addr_of(back)), -8);
    EXPECT_EQ(lh_math_rect_get_y(lh_addr_of(back)), 15);
    EXPECT_EQ(lh_math_rect_get_size_width(lh_addr_of(back)), 4);
    EXPECT_EQ(lh_math_rect_get_size_height(lh_addr_of(back)), 9);
}

TEST(math_frect, to_rect_truncates_toward_zero)
{
    const lh_math_frect_t r = lh_math_frect_make(3.9f, -3.9f, 8.2f, 1.1f);
    const lh_math_rect_t screen = lh_math_frect_to_rect(r);
    EXPECT_EQ(lh_math_rect_get_x(lh_addr_of(screen)), 3);
    EXPECT_EQ(lh_math_rect_get_y(lh_addr_of(screen)), -3);
    EXPECT_EQ(lh_math_rect_get_size_width(lh_addr_of(screen)), 8);
    EXPECT_EQ(lh_math_rect_get_size_height(lh_addr_of(screen)), 1);
}

} /* namespace */
