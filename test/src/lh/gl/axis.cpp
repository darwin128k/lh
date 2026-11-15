#include <gtest/gtest.h>

#include <lh/test/gl/draw_log.h>

#include <lh/gl/axis.h>

using lh_test::rect_is;
using lh_test::rect_of;

TEST(ui_axis, point_get_and_set_along)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), 3, 7);
    EXPECT_EQ(lh_gl_point_get_along(lh_addr_of(point), lh_gl_axis_horizontal), lh_gl_scalar(3));
    EXPECT_EQ(lh_gl_point_get_along(lh_addr_of(point), lh_gl_axis_vertical), lh_gl_scalar(7));
    lh_gl_point_set_along(lh_addr_of(point), lh_gl_axis_vertical, lh_gl_scalar(9));
    lh_gl_point_set_along(lh_addr_of(point), lh_gl_axis_horizontal, lh_gl_scalar(1));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), lh_gl_scalar(1));
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), lh_gl_scalar(9));
}

TEST(ui_axis, point_init_along_puts_across_on_the_other_axis)
{
    lh_gl_point_t point;

    lh_gl_point_init_along(lh_addr_of(point), lh_gl_axis_vertical, lh_gl_scalar(5), lh_gl_scalar(2));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), lh_gl_scalar(2));
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), lh_gl_scalar(5));
    lh_gl_point_init_along(lh_addr_of(point), lh_gl_axis_horizontal, lh_gl_scalar(5), lh_gl_scalar(2));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(point)), lh_gl_scalar(5));
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(point)), lh_gl_scalar(2));
}

TEST(ui_axis, size_get_and_set_along)
{
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 4, 6);
    EXPECT_EQ(lh_gl_size_get_along(lh_addr_of(size), lh_gl_axis_horizontal), lh_gl_scalar(4));
    EXPECT_EQ(lh_gl_size_get_along(lh_addr_of(size), lh_gl_axis_vertical), lh_gl_scalar(6));
    lh_gl_size_set_along(lh_addr_of(size), lh_gl_axis_vertical, lh_gl_scalar(8));
    EXPECT_EQ(lh_gl_size_get_height(lh_addr_of(size)), lh_gl_scalar(8));
    EXPECT_EQ(lh_gl_size_get_width(lh_addr_of(size)), lh_gl_scalar(4));
}

TEST(ui_axis, rect_init_along_takes_a_slice_of_the_base)
{
    const lh_gl_rect_t base = rect_of(10, 20, 30, 40);
    lh_gl_rect_t slice;

    lh_gl_rect_init_along(lh_addr_of(slice), lh_addr_of(base), lh_gl_axis_vertical, lh_gl_scalar(5),
                          lh_gl_scalar(12));
    EXPECT_TRUE(rect_is(slice, rect_of(10, 25, 30, 12)));
    lh_gl_rect_init_along(lh_addr_of(slice), lh_addr_of(base), lh_gl_axis_horizontal, lh_gl_scalar(5),
                          lh_gl_scalar(12));
    EXPECT_TRUE(rect_is(slice, rect_of(15, 20, 12, 40)));
}
