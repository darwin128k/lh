#include <gtest/gtest.h>

#include <lh/math/ipoint.h>
#include <lh/math/ipoint3.h>
#include <lh/math/vec2.h>
#include <lh/math/vec3.h>
#include <lh/util/addr.h>

namespace
{

lh_math_iscalar_t
x_of(lh_math_ipoint_t p)
{
    return lh_math_ipoint_get_x(lh_addr_of(p));
}

lh_math_iscalar_t
y_of(lh_math_ipoint_t p)
{
    return lh_math_ipoint_get_y(lh_addr_of(p));
}

/* ── point ↔ vec2 ───────────────────────────────────────────────────────── */

TEST(convert_point_vec2, point_to_vec2_is_exact)
{
    const lh_math_vec2_t v = lh_math_ipoint_to_vec2(lh_math_ipoint_make(3, -7));
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, -7.0f);
}

TEST(convert_point_vec2, vec2_to_point_rounds_to_nearest)
{
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(3.4f, 0.0f))), 3);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(3.6f, 0.0f))), 4);
    /* A tie goes to the lower pixel, as the rasterizer's ceil(x - 0.5f) does. */
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(3.5f, 0.0f))), 3);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(-3.5f, 0.0f))), -4);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(-3.4f, 0.0f))), -3);
    /* Already integral: unchanged. */
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(5.0f, 0.0f))), 5);
}

TEST(convert_point_vec2, point_round_trip_is_lossless)
{
    const lh_math_ipoint_t p = lh_math_ipoint_make(12, -34);
    const lh_math_ipoint_t back = lh_math_vec2_to_ipoint(lh_math_ipoint_to_vec2(p));
    EXPECT_EQ(x_of(back), 12);
    EXPECT_EQ(y_of(back), -34);
}

/* ── point3 ↔ vec3 ──────────────────────────────────────────────────────── */

TEST(convert_point3_vec3, point3_to_vec3_is_exact)
{
    const lh_math_vec3_t v = lh_math_ipoint3_to_vec3(lh_math_ipoint3_make(3, -7, 11));
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, -7.0f);
    EXPECT_FLOAT_EQ(v.z, 11.0f);
}

TEST(convert_point3_vec3, vec3_to_point3_rounds_to_nearest)
{
    const lh_math_ipoint3_t p = lh_math_vec3_to_ipoint3(lh_math_vec3_make(3.4f, 3.6f, -3.5f));
    EXPECT_EQ(lh_math_ipoint3_get_x(lh_addr_of(p)), 3);
    EXPECT_EQ(lh_math_ipoint3_get_y(lh_addr_of(p)), 4);
    EXPECT_EQ(lh_math_ipoint3_get_z(lh_addr_of(p)), -4); // tie toward the lower cell
}

TEST(convert_point3_vec3, point3_round_trip_is_lossless)
{
    const lh_math_ipoint3_t p = lh_math_ipoint3_make(12, -34, 56);
    const lh_math_ipoint3_t back = lh_math_vec3_to_ipoint3(lh_math_ipoint3_to_vec3(p));
    EXPECT_EQ(lh_math_ipoint3_get_x(lh_addr_of(back)), 12);
    EXPECT_EQ(lh_math_ipoint3_get_y(lh_addr_of(back)), -34);
    EXPECT_EQ(lh_math_ipoint3_get_z(lh_addr_of(back)), 56);
}

/* ── the float → pixel boundary agrees with the rasterizer ──────────────── */

/* lh_entity_2d_draw_background picks the pixels an entity covers with `ceil(x - 0.5f)`.
 * A vec2_to_ipoint at the same position must land on the same pixel, or a
 * picked point and a drawn rect would disagree. */
TEST(convert_float_pixel, agrees_with_the_pixel_the_rasterizer_covers)
{
    /* Pixel 10 covers [10, 11) in continuous coordinates, so its centre is
     * 10.5. Everything from 9.5 up to (but not including) 10.5 rounds to
     * pixel 10, which is what the rect covers. */
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(9.5f, 0.0f))), 9);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(9.9f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(10.0f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(10.4f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(10.5f, 0.0f))), 10);
    EXPECT_EQ(x_of(lh_math_vec2_to_ipoint(lh_math_vec2_make(10.6f, 0.0f))), 11);
}

} // namespace
