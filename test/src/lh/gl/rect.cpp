#include <gtest/gtest.h>
#include <type_traits>

#include <lh/config.h>
#include <lh/core/math/rect.h>
#include <lh/gl/rect.h>

namespace
{

TEST(ui_rect, components_are_the_scalar)
{
    lh_gl_rect_t r;

    lh_gl_rect_init(lh_addr_of(r), 1, 2, 3, 4);
    using component = decltype(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(r))));
    EXPECT_TRUE((std::is_same<component, lh_gl_scalar_t>::value));
}

TEST(ui_rect, holds_origin_and_size)
{
    lh_gl_rect_t r;

    lh_gl_rect_init(lh_addr_of(r), 10, 20, 30, 40);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(r))), 10);
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(r))), 20);
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(r))), 30);
    EXPECT_EQ(lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(r))), 40);
    EXPECT_FALSE(lh_gl_rect_is_empty(lh_addr_of(r)));
}

TEST(ui_rect, zero_size_is_empty)
{
    lh_gl_rect_t r;

    lh_gl_rect_init_empty(lh_addr_of(r));
    EXPECT_TRUE(lh_gl_rect_is_empty(lh_addr_of(r)));
    EXPECT_EQ(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(r))), 0);
    EXPECT_EQ(lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(r))), 0);
}

TEST(ui_rect, negative_extent_is_empty)
{
    lh_gl_rect_t r;

    lh_gl_rect_init(lh_addr_of(r), 1, 1, -4, 8);
    EXPECT_TRUE(lh_gl_rect_is_empty(lh_addr_of(r)));
}

TEST(ui_rect, roundtrip_through_the_screen_rect)
{
    lh_math_rect_t screen;

    lh_math_rect_init(lh_addr_of(screen), -8, 15, 4, 9);
    lh_gl_rect_t r;
    lh_gl_rect_from_rect(screen, lh_addr_of(r));
    lh_math_rect_t back;
    lh_gl_rect_to_rect(r, lh_addr_of(back));
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin(lh_addr_of(back))), -8);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin(lh_addr_of(back))), 15);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size(lh_addr_of(back))), 4);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size(lh_addr_of(back))), 9);
}

#if LH_LIBRARY_OPTION_MATH_FPU
TEST(ui_rect, to_rect_truncates_toward_zero)
{
    lh_gl_rect_t r;

    lh_gl_rect_init(lh_addr_of(r), 3.9f, -3.9f, 8.2f, 1.1f);
    lh_math_rect_t screen;
    lh_gl_rect_to_rect(r, lh_addr_of(screen));
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin(lh_addr_of(screen))), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin(lh_addr_of(screen))), -3);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size(lh_addr_of(screen))), 8);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size(lh_addr_of(screen))), 1);
}
#endif

} /* namespace */
