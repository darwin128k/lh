#include <gtest/gtest.h>

#include <lh/math/point.h>
#include <lh/math/vec2.h>
#include <lh/util/addr.h>

namespace
{

lh_math_scalar_t
x_of(lh_math_point_t p)
{
    return lh_math_point_get_x(lh_addr_of(p));
}

lh_math_scalar_t
y_of(lh_math_point_t p)
{
    return lh_math_point_get_y(lh_addr_of(p));
}

/* ── point ↔ vec2 ───────────────────────────────────────────────────────── */

TEST(convert_point_vec2, point_to_vec2_is_exact)
{
    const lh_math_vec2_t v = lh_math_point_to_vec2(lh_math_point_make(3, -7));
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, -7.0f);
}

TEST(convert_point_vec2, vec2_to_point_rounds_to_nearest)
{
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(3.4f, 0.0f))), 3);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(3.6f, 0.0f))), 4);
    /* A tie goes to the lower pixel, as the rasterizer's ceil(x - 0.5f) does. */
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(3.5f, 0.0f))), 3);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(-3.5f, 0.0f))), -4);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(-3.4f, 0.0f))), -3);
    /* Already integral: unchanged. */
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(5.0f, 0.0f))), 5);
}

TEST(convert_point_vec2, point_round_trip_is_lossless)
{
    const lh_math_point_t p = lh_math_point_make(12, -34);
    const lh_math_point_t back = lh_math_vec2_to_point(lh_math_point_to_vec2(p));
    EXPECT_EQ(x_of(back), 12);
    EXPECT_EQ(y_of(back), -34);
}

/* ── the float → pixel boundary agrees with the rasterizer ──────────────── */

/* lh_entity_2d_draw_background picks the pixels an entity covers with `ceil(x - 0.5f)`.
 * A vec2_to_point at the same position must land on the same pixel, or a
 * picked point and a drawn rect would disagree. */
TEST(convert_float_pixel, agrees_with_the_pixel_the_rasterizer_covers)
{
    /* Pixel 10 covers [10, 11) in continuous coordinates, so its centre is
     * 10.5. Everything from 9.5 up to (but not including) 10.5 rounds to
     * pixel 10, which is what the rect covers. */
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(9.5f, 0.0f))), 9);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(9.9f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(10.0f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(10.4f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(10.5f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_point(lh_math_vec2_make(10.6f, 0.0f))), 11);
}

} // namespace
