#include <gtest/gtest.h>
#include <type_traits>

#include <lh/config.h>
#include <lh/math/rect.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>

namespace
{

TEST(ui_rect, components_are_the_scalar)
{
    const lh_ui_rect_t r = lh_ui_rect_make(1, 2, 3, 4);
    using component = decltype(lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(r))));
    EXPECT_TRUE((std::is_same<component, lh_ui_scalar_t>::value));
}

TEST(ui_rect, holds_origin_and_size)
{
    const lh_ui_rect_t r = lh_ui_rect_make(10, 20, 30, 40);
    EXPECT_EQ(lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(r))), 10);
    EXPECT_EQ(lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(r))), 20);
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(r))), 30);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(r))), 40);
    EXPECT_FALSE(lh_ui_rect_is_empty(lh_addr_of(r)));
}

TEST(ui_rect, zero_size_is_empty)
{
    const lh_ui_rect_t r = lh_ui_rect_make_empty();
    EXPECT_TRUE(lh_ui_rect_is_empty(lh_addr_of(r)));
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(r))), 0);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(r))), 0);
}

TEST(ui_rect, negative_extent_is_empty)
{
    const lh_ui_rect_t r = lh_ui_rect_make(1, 1, -4, 8);
    EXPECT_TRUE(lh_ui_rect_is_empty(lh_addr_of(r)));
}

TEST(ui_rect, roundtrip_through_the_screen_rect)
{
    const lh_math_rect_t screen = lh_math_rect_make(-8, 15, 4, 9);
    const lh_ui_rect_t r = lh_ui_rect_from_rect(screen);
    const lh_math_rect_t back = lh_ui_rect_to_rect(r);
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin_as_const(lh_addr_of(back))), -8);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin_as_const(lh_addr_of(back))), 15);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size_as_const(lh_addr_of(back))), 4);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size_as_const(lh_addr_of(back))), 9);
}

#if LH_LIBRARY_OPTION_MATH_FPU
TEST(ui_rect, to_rect_truncates_toward_zero)
{
    const lh_ui_rect_t r = lh_ui_rect_make(3.9f, -3.9f, 8.2f, 1.1f);
    const lh_math_rect_t screen = lh_ui_rect_to_rect(r);
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin_as_const(lh_addr_of(screen))), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin_as_const(lh_addr_of(screen))), -3);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size_as_const(lh_addr_of(screen))), 8);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size_as_const(lh_addr_of(screen))), 1);
}
#endif

} /* namespace */
