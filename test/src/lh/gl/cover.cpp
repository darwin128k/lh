#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/cover.h>

namespace
{

TEST(gl_cover, a_rounded_rectangle_drops_the_corner)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 0, 0, 4, 4);
    EXPECT_EQ(lh_gl_cover_round_rect(lh_addr_of(rect), 2, 0, 0), 0);
    EXPECT_EQ(lh_gl_cover_round_rect(lh_addr_of(rect), 2, 3, 0), 0);
    EXPECT_EQ(lh_gl_cover_round_rect(lh_addr_of(rect), 2, 1, 0), 255);
    EXPECT_EQ(lh_gl_cover_round_rect(lh_addr_of(rect), 2, 1, 1), 255);
    EXPECT_EQ(lh_gl_cover_round_rect(lh_addr_of(rect), 0, 0, 0), 255);
    lh_gl_scalar_t x0 = -1;
    lh_gl_scalar_t x1 = -1;
    EXPECT_TRUE(lh_gl_cover_round_rect_span(lh_addr_of(rect), 2, 0, lh_addr_of(x0), lh_addr_of(x1)));
    EXPECT_EQ(x0, 1);
    EXPECT_EQ(x1, 3);
    EXPECT_TRUE(lh_gl_cover_round_rect_span(lh_addr_of(rect), 2, 1, lh_addr_of(x0), lh_addr_of(x1)));
    EXPECT_EQ(x0, 0);
    EXPECT_EQ(x1, 4);
}

TEST(gl_cover, a_circle_drops_the_corner_of_its_box)
{
    lh_gl_point_t center;

    lh_gl_point_init(lh_addr_of(center), 2, 2);
    EXPECT_EQ(lh_gl_cover_circle(lh_addr_of(center), 2, 0, 0), 0);
    EXPECT_EQ(lh_gl_cover_circle(lh_addr_of(center), 2, 0, 1), 255);
    EXPECT_EQ(lh_gl_cover_circle(lh_addr_of(center), 2, 2, 2), 255);
    EXPECT_EQ(lh_gl_cover_circle(lh_addr_of(center), 0, 2, 2), 0);
}

TEST(gl_cover, a_triangle_keeps_its_interior)
{
    lh_gl_point_t a;

    lh_gl_point_init(lh_addr_of(a), 0, 0);
    lh_gl_point_t b;

    lh_gl_point_init(lh_addr_of(b), 4, 0);
    lh_gl_point_t c;

    lh_gl_point_init(lh_addr_of(c), 0, 4);
    EXPECT_EQ(lh_gl_cover_triangle(lh_addr_of(a), lh_addr_of(b), lh_addr_of(c), 0, 0), 255);
    EXPECT_EQ(lh_gl_cover_triangle(lh_addr_of(a), lh_addr_of(b), lh_addr_of(c), 1, 1), 255);
    EXPECT_EQ(lh_gl_cover_triangle(lh_addr_of(a), lh_addr_of(b), lh_addr_of(c), 3, 1), 0);
    EXPECT_EQ(lh_gl_cover_triangle(lh_addr_of(a), lh_addr_of(b), lh_addr_of(c), 3, 3), 0);
}

} // namespace
