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
    EXPECT_EQ(lh_gl_color_get_word(lh_addr_of(color)), 0xFF010203U);
    lh_gl_color_set_a(lh_addr_of(color), 0);
    EXPECT_EQ(lh_gl_color_get_a(lh_addr_of(color)), 0);
}

TEST(gl_color, over_blends_straight_alpha)
{
    lh_gl_color_t dst;
    lh_gl_color_init(lh_addr_of(dst), 200, 10, 10, 255);
    lh_gl_color_t src;
    lh_gl_color_init(lh_addr_of(src), 0, 180, 20, 100);
    lh_gl_color_t out;
    lh_gl_color_over(lh_addr_of(dst), lh_addr_of(src), lh_addr_of(out));
    EXPECT_EQ(lh_gl_color_get_r(lh_addr_of(out)), 122);
    EXPECT_EQ(lh_gl_color_get_g(lh_addr_of(out)), 77);
    EXPECT_EQ(lh_gl_color_get_b(lh_addr_of(out)), 14);
    EXPECT_EQ(lh_gl_color_get_a(lh_addr_of(out)), 255);
    lh_gl_color_set_a(lh_addr_of(src), 0);
    lh_gl_color_over(lh_addr_of(dst), lh_addr_of(src), lh_addr_of(out));
    EXPECT_EQ(lh_gl_color_get_r(lh_addr_of(out)), 200);
    lh_gl_color_set_a(lh_addr_of(src), 255);
    lh_gl_color_over(lh_addr_of(dst), lh_addr_of(src), lh_addr_of(out));
    EXPECT_EQ(lh_gl_color_get_g(lh_addr_of(out)), 180);
    EXPECT_EQ(lh_gl_color_get_a(lh_addr_of(out)), 255);
    lh_gl_color_init(lh_addr_of(dst), 0, 0, 0, 0);
    lh_gl_color_init(lh_addr_of(src), 0, 0, 0, 0);
    lh_gl_color_over(lh_addr_of(dst), lh_addr_of(src), lh_addr_of(out));
    EXPECT_EQ(lh_gl_color_get_a(lh_addr_of(out)), 0);
}
