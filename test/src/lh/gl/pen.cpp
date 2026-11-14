#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/pen.h>

TEST(gl_pen, init_copies_color_and_width)
{
    lh_gl_color_t color;

    lh_gl_color_init(lh_addr_of(color), 8, 9, 10, 11);
    lh_gl_pen_t pen;

    lh_gl_pen_init(lh_addr_of(pen), color, 4);
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pen_get_color(lh_addr_of(pen))), 8);
    EXPECT_EQ(lh_gl_pen_get_width(lh_addr_of(pen)), 4);
    lh_gl_pen_set_width(lh_addr_of(pen), 0);
    EXPECT_EQ(lh_gl_pen_get_width(lh_addr_of(pen)), 0);
}
