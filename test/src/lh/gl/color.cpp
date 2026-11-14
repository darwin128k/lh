#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/color.h>

TEST(gl_color, init_stores_straight_channels)
{
    lh_gl_color_t color;

    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 255);
    EXPECT_EQ(lh_gl_color_get_r(lh_addr_of(color)), 1);
    EXPECT_EQ(lh_gl_color_get_g(lh_addr_of(color)), 2);
    EXPECT_EQ(lh_gl_color_get_b(lh_addr_of(color)), 3);
    EXPECT_EQ(lh_gl_color_get_a(lh_addr_of(color)), 255);
    lh_gl_color_set_a(lh_addr_of(color), 0);
    EXPECT_EQ(lh_gl_color_get_a(lh_addr_of(color)), 0);
}
