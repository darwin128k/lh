#include <gtest/gtest.h>

#include <lh/math/rect.h>
#include <lh/math/rect3.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_rect3, holds_origin_xy_z_size_and_z_depth)
{
    /* make(x, y, z, width, height, z_depth) — 6 args. */
    const lh_math_rect3_t b = lh_math_rect3_make(2, 4, 6, 8, 10, 12);
    EXPECT_EQ(lh_math_rect3_get_x(lh_addr_of(b)), 2);
    EXPECT_EQ(lh_math_rect3_get_y(lh_addr_of(b)), 4);
    EXPECT_EQ(lh_math_rect3_get_z(lh_addr_of(b)), 6);
    EXPECT_EQ(lh_math_rect3_get_width(lh_addr_of(b)), 8);
    EXPECT_EQ(lh_math_rect3_get_height(lh_addr_of(b)), 10);
    EXPECT_EQ(lh_math_rect3_get_z_depth(lh_addr_of(b)), 12);
}

TEST(math_rect3, embeds_2d_rect)
{
    /* A rect3 IS a 2D rect (its `rect` field) plus z and z_depth. The 2D
     * origin and size must match a 2D rect constructed with the same x, y,
     * width, height — that is what "2D is the core of 3D" means. The z and
     * z_depth are additional fields. */
    const lh_math_rect3_t b = lh_math_rect3_make(2, 4, 6, 8, 10, 12);
    const lh_math_rect_t two_d = lh_math_rect_make(2, 4, 8, 10);
    EXPECT_EQ(lh_math_rect_get_x(lh_addr_of(two_d)), lh_math_rect3_get_x(lh_addr_of(b)));
    EXPECT_EQ(lh_math_rect_get_y(lh_addr_of(two_d)), lh_math_rect3_get_y(lh_addr_of(b)));
    EXPECT_EQ(lh_math_rect_get_size_width(lh_addr_of(two_d)), lh_math_rect3_get_width(lh_addr_of(b)));
    EXPECT_EQ(lh_math_rect_get_size_height(lh_addr_of(two_d)), lh_math_rect3_get_height(lh_addr_of(b)));
    EXPECT_EQ(lh_math_rect3_get_z(lh_addr_of(b)), 6);
    EXPECT_EQ(lh_math_rect3_get_z_depth(lh_addr_of(b)), 12);
}

TEST(math_rect3, setters_for_z_and_z_depth)
{
    lh_math_rect3_t b = lh_math_rect3_make(0, 0, 0, 5, 5, 5);
    lh_math_rect3_set_z(lh_addr_of(b), 100);
    lh_math_rect3_set_z_depth(lh_addr_of(b), 50);
    EXPECT_EQ(lh_math_rect3_get_z(lh_addr_of(b)), 100);
    EXPECT_EQ(lh_math_rect3_get_z_depth(lh_addr_of(b)), 50);
    /* Unrelated fields are unchanged. */
    EXPECT_EQ(lh_math_rect3_get_x(lh_addr_of(b)), 0);
    EXPECT_EQ(lh_math_rect3_get_y(lh_addr_of(b)), 0);
    EXPECT_EQ(lh_math_rect3_get_width(lh_addr_of(b)), 5);
    EXPECT_EQ(lh_math_rect3_get_height(lh_addr_of(b)), 5);
}

TEST(math_rect3, zero_is_empty)
{
    const lh_math_rect3_t b = lh_math_rect3_make_empty();
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(b)));
    EXPECT_EQ(lh_math_rect3_get_width(lh_addr_of(b)), 0);
    EXPECT_EQ(lh_math_rect3_get_height(lh_addr_of(b)), 0);
    EXPECT_EQ(lh_math_rect3_get_z_depth(lh_addr_of(b)), 0);
}

TEST(math_rect3, is_empty_when_any_extent_zero_or_negative)
{
    /* zero width → empty */
    const lh_math_rect3_t zero_w = lh_math_rect3_make(0, 0, 0, 0, 5, 5);
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(zero_w)));
    /* zero height → empty */
    const lh_math_rect3_t zero_h = lh_math_rect3_make(0, 0, 0, 5, 0, 5);
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(zero_h)));
    /* zero z_depth → empty */
    const lh_math_rect3_t zero_zd = lh_math_rect3_make(0, 0, 5, 5, 5, 0);
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(zero_zd)));
    /* negative z_depth → empty */
    const lh_math_rect3_t neg_zd = lh_math_rect3_make(0, 0, 5, 5, 5, -1);
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(neg_zd)));
    /* all positive → not empty */
    const lh_math_rect3_t ok = lh_math_rect3_make(0, 0, 5, 5, 5, 5);
    EXPECT_FALSE(lh_math_rect3_is_empty(lh_addr_of(ok)));
}

TEST(math_rect3, intersection_xy_and_z)
{
    /* a: x/y in [0,10)x[0,10), z in [0,5). b: x/y in [5,15)x[5,15), z in [3,8).
     * Intersection: x/y in [5,10)x[5,10), z in [3,5). */
    const lh_math_rect3_t a = lh_math_rect3_make(0, 0, 0, 10, 10, 5);
    const lh_math_rect3_t b = lh_math_rect3_make(5, 5, 3, 10, 10, 5);
    const lh_math_rect3_t i = lh_math_rect3_intersection(lh_addr_of(a), lh_addr_of(b));
    EXPECT_EQ(lh_math_rect3_get_x(lh_addr_of(i)), 5);
    EXPECT_EQ(lh_math_rect3_get_y(lh_addr_of(i)), 5);
    EXPECT_EQ(lh_math_rect3_get_width(lh_addr_of(i)), 5);
    EXPECT_EQ(lh_math_rect3_get_height(lh_addr_of(i)), 5);
    EXPECT_EQ(lh_math_rect3_get_z(lh_addr_of(i)), 3);
    EXPECT_EQ(lh_math_rect3_get_z_depth(lh_addr_of(i)), 2);
}

TEST(math_rect3, intersection_z_disjoint_is_empty)
{
    /* a: z in [0,5). disjoint: z in [20,25). Same x/y footprint. */
    const lh_math_rect3_t a = lh_math_rect3_make(0, 0, 0, 10, 10, 5);
    const lh_math_rect3_t disjoint = lh_math_rect3_make(0, 0, 20, 10, 10, 5);
    const lh_math_rect3_t r = lh_math_rect3_intersection(lh_addr_of(a), lh_addr_of(disjoint));
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(r)));
}

TEST(math_rect3, intersection_xy_disjoint_is_empty)
{
    /* Same z slab, but x/y footprints don't overlap. */
    const lh_math_rect3_t a = lh_math_rect3_make(0, 0, 0, 5, 5, 5);
    const lh_math_rect3_t disjoint = lh_math_rect3_make(20, 20, 0, 5, 5, 5);
    const lh_math_rect3_t r = lh_math_rect3_intersection(lh_addr_of(a), lh_addr_of(disjoint));
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(r)));
}

TEST(math_rect3, intersection_with_empty_is_empty)
{
    const lh_math_rect3_t a = lh_math_rect3_make(0, 0, 0, 5, 5, 5);
    const lh_math_rect3_t empty = lh_math_rect3_make_empty();
    const lh_math_rect3_t r = lh_math_rect3_intersection(lh_addr_of(a), lh_addr_of(empty));
    EXPECT_TRUE(lh_math_rect3_is_empty(lh_addr_of(r)));
}

TEST(math_rect3, eq)
{
    const lh_math_rect3_t a = lh_math_rect3_make(1, 2, 3, 4, 5, 6);
    const lh_math_rect3_t b = lh_math_rect3_make(1, 2, 3, 4, 5, 6);
    const lh_math_rect3_t c = lh_math_rect3_make(1, 2, 3, 4, 5, 7);
    EXPECT_TRUE(lh_math_rect3_eq(lh_addr_of(a), lh_addr_of(b)));
    EXPECT_FALSE(lh_math_rect3_eq(lh_addr_of(a), lh_addr_of(c)));
}

TEST(math_rect3, offset_translates_xyz_keeps_size_and_depth)
{
    const lh_math_rect3_t b = lh_math_rect3_make(0, 0, 0, 10, 8, 5);
    const lh_math_rect3_t moved = lh_math_rect3_offset(lh_addr_of(b), 3, -2, 7);
    EXPECT_EQ(lh_math_rect3_get_x(lh_addr_of(moved)), 3);
    EXPECT_EQ(lh_math_rect3_get_y(lh_addr_of(moved)), -2);
    EXPECT_EQ(lh_math_rect3_get_z(lh_addr_of(moved)), 7);
    EXPECT_EQ(lh_math_rect3_get_width(lh_addr_of(moved)), 10);
    EXPECT_EQ(lh_math_rect3_get_height(lh_addr_of(moved)), 8);
    EXPECT_EQ(lh_math_rect3_get_z_depth(lh_addr_of(moved)), 5);
}

} // namespace