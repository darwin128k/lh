#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/mask.h>

namespace
{

TEST(gl_mask, a_round_edge_is_partial_and_the_outside_is_empty)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_gl_mask_t round;

    lh_gl_mask_init_round(lh_addr_of(round), rect, 4);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(round), 0, 0), 0);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(round), 1, 0), 127);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(round), 5, 5), 255);
    EXPECT_EQ(lh_gl_mask_get_kind(lh_addr_of(round)), lh_gl_mask_kind_round);
}

TEST(gl_mask, intersect_unite_and_exclude_combine_two_shapes)
{
    lh_gl_rect_t wide;
    lh_gl_rect_t tall;

    lh_gl_rect_init(lh_addr_of(wide), 0, 0, 8, 4);
    lh_gl_rect_init(lh_addr_of(tall), 2, 0, 4, 8);
    lh_gl_mask_t left;
    lh_gl_mask_t right;

    lh_gl_mask_init_round(lh_addr_of(left), wide, 0);
    lh_gl_mask_init_round(lh_addr_of(right), tall, 0);
    lh_gl_mask_t both;
    lh_gl_mask_t either;
    lh_gl_mask_t hole;

    lh_gl_mask_init_intersect(lh_addr_of(both), lh_addr_of(left), lh_addr_of(right));
    lh_gl_mask_init_unite(lh_addr_of(either), lh_addr_of(left), lh_addr_of(right));
    lh_gl_mask_init_exclude(lh_addr_of(hole), lh_addr_of(left), lh_addr_of(right));
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(both), 3, 1), 255);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(both), 0, 1), 0);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(either), 0, 1), 255);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(either), 3, 6), 255);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(hole), 0, 1), 255);
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(hole), 3, 1), 0);
    lh_gl_mask_init_intersect(lh_addr_of(both), nullptr, lh_addr_of(right));
    EXPECT_EQ(lh_gl_mask_coverage(lh_addr_of(both), 3, 1), 0);
}

} // namespace
