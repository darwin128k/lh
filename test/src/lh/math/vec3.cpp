#include <gtest/gtest.h>

#include <lh/math/vec2.h>
#include <lh/math/vec3.h>
#include <lh/util/addr.h>

#include <cstddef>

namespace
{

const lh_float_t k_eps = 1e-6f;

void
expect_vec3(lh_math_vec3_t v, lh_float_t x, lh_float_t y, lh_float_t z)
{
    EXPECT_FLOAT_EQ(v.x, x);
    EXPECT_FLOAT_EQ(v.y, y);
    EXPECT_FLOAT_EQ(v.z, z);
}

TEST(vec3, layout_is_three_consecutive_floats)
{
    // Same layout as float[3] (GoldSrc's vec3_t), so game data can be read in place.
    EXPECT_EQ(sizeof(lh_math_vec3_t), 3 * sizeof(float));
    EXPECT_EQ(offsetof(lh_math_vec3_t, x), 0 * sizeof(float));
    EXPECT_EQ(offsetof(lh_math_vec3_t, y), 1 * sizeof(float));
    EXPECT_EQ(offsetof(lh_math_vec3_t, z), 2 * sizeof(float));
}

TEST(vec3, make_add_sub_scale_neg)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f);
    lh_math_vec3_t b;

    lh_math_vec3_init(lh_addr_of(b), 4.0f, -5.0f, 0.5f);
    expect_vec3(a, 1.0f, 2.0f, 3.0f);
    expect_vec3(lh_math_vec3_add(a, b), 5.0f, -3.0f, 3.5f);
    expect_vec3(lh_math_vec3_sub(a, b), -3.0f, 7.0f, 2.5f);
    expect_vec3(lh_math_vec3_scale(a, 2.0f), 2.0f, 4.0f, 6.0f);
    expect_vec3(lh_math_vec3_neg(a), -1.0f, -2.0f, -3.0f);
}

TEST(vec3, dot)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f);
    lh_math_vec3_t b;

    lh_math_vec3_init(lh_addr_of(b), 4.0f, -5.0f, 6.0f);
    EXPECT_FLOAT_EQ(lh_math_vec3_dot(a, b), 4.0f - 10.0f + 18.0f);
    // Perpendicular axes.
    EXPECT_FLOAT_EQ(lh_math_vec3_dot(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })(), ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 1, 0); return _v; })()), 0.0f);
}

TEST(vec3, length)
{
    lh_math_vec3_t v;

    lh_math_vec3_init(lh_addr_of(v), 2.0f, 3.0f, 6.0f); // 4 + 9 + 36 = 49
    EXPECT_FLOAT_EQ(lh_math_vec3_length_sq(v), 49.0f);
    EXPECT_FLOAT_EQ(lh_math_vec3_length(v), 7.0f);
    EXPECT_FLOAT_EQ(lh_math_vec3_length(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 0); return _v; })()), 0.0f);
}

TEST(vec3, normalize)
{
    const lh_math_vec3_t n = lh_math_vec3_normalize(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 2.0f, 3.0f, 6.0f); return _v; })());
    expect_vec3(n, 2.0f / 7.0f, 3.0f / 7.0f, 6.0f / 7.0f);
    EXPECT_NEAR(lh_math_vec3_length(n), 1.0f, k_eps);
}

TEST(vec3, normalize_zero_stays_zero)
{
    expect_vec3(lh_math_vec3_normalize(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 0); return _v; })()), 0.0f, 0.0f, 0.0f);
}

TEST(vec3, cross_of_axes_is_the_third_axis)
{
    lh_math_vec3_t x;

    lh_math_vec3_init(lh_addr_of(x), 1, 0, 0);
    lh_math_vec3_t y;

    lh_math_vec3_init(lh_addr_of(y), 0, 1, 0);
    lh_math_vec3_t z;

    lh_math_vec3_init(lh_addr_of(z), 0, 0, 1);
    expect_vec3(lh_math_vec3_cross(x, y), 0, 0, 1);
    expect_vec3(lh_math_vec3_cross(y, z), 1, 0, 0);
    expect_vec3(lh_math_vec3_cross(z, x), 0, 1, 0);
    expect_vec3(lh_math_vec3_cross(y, x), 0, 0, -1); // anticommutative
}

TEST(vec3, cross_is_perpendicular_to_both)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 1.5f, -2.0f, 0.25f);
    lh_math_vec3_t b;

    lh_math_vec3_init(lh_addr_of(b), -3.0f, 0.5f, 4.0f);
    const lh_math_vec3_t c = lh_math_vec3_cross(a, b);
    EXPECT_NEAR(lh_math_vec3_dot(c, a), 0.0f, 1e-5f);
    EXPECT_NEAR(lh_math_vec3_dot(c, b), 0.0f, 1e-5f);
    // Parallel vectors have no perpendicular: zero.
    expect_vec3(lh_math_vec3_cross(a, lh_math_vec3_scale(a, 3.0f)), 0, 0, 0);
}

TEST(vec3, lerp)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 0.0f, 10.0f, -4.0f);
    lh_math_vec3_t b;

    lh_math_vec3_init(lh_addr_of(b), 10.0f, 20.0f, 4.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 0.0f), 0.0f, 10.0f, -4.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 1.0f), 10.0f, 20.0f, 4.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 0.5f), 5.0f, 15.0f, 0.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 2.0f), 20.0f, 30.0f, 12.0f); // not clamped
}

TEST(vec3, near)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(lh_math_vec3_near(a, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1.0f, 2.0f, 3.0f); return _v; })(), 0.0f));
    EXPECT_TRUE(lh_math_vec3_near(a, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1.0005f, 1.9995f, 3.0f); return _v; })(), 0.001f));
    EXPECT_FALSE(lh_math_vec3_near(a, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1.0f, 2.0f, 3.01f); return _v; })(), 0.001f));
    EXPECT_FALSE(lh_math_vec3_near(a, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0.99f, 2.0f, 3.0f); return _v; })(), 0.001f));
}

/* ── conversions ────────────────────────────────────────────────────────── */

TEST(vec3, vec2_to_vec3_adds_z)
{
    lh_math_vec2_t a;

    lh_math_vec2_init(lh_addr_of(a), 3.5f, -7.25f);
    expect_vec3(lh_math_vec2_to_vec3(a, 42.0f), 3.5f, -7.25f, 42.0f);
    expect_vec3(lh_math_vec2_to_vec3(a, -1.0f), 3.5f, -7.25f, -1.0f);
}

TEST(vec3, vec2_to_vec3_z0_is_the_plane_of_a_2d_entity)
{
    lh_math_vec2_t a;

    lh_math_vec2_init(lh_addr_of(a), 3.5f, -7.25f);
    expect_vec3(lh_math_vec2_to_vec3_z0(a), 3.5f, -7.25f, 0.0f);
    // Equal to the explicit form it replaces.
    expect_vec3(lh_math_vec2_to_vec3_z0(a),
                lh_math_vec2_get_x(lh_addr_of(a)), lh_math_vec2_get_y(lh_addr_of(a)), 0.0f);
}

TEST(vec3, vec3_to_vec2_drops_z)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 3.5f, -7.25f, 99.0f);
    const lh_math_vec2_t b = lh_math_vec3_to_vec2(a);
    EXPECT_FLOAT_EQ(b.x, 3.5f);
    EXPECT_FLOAT_EQ(b.y, -7.25f);
}

TEST(vec3, vec2_round_trip_through_vec3_keeps_xy)
{
    lh_math_vec2_t a;

    lh_math_vec2_init(lh_addr_of(a), 3.5f, -7.25f);
    const lh_math_vec2_t b = lh_math_vec3_to_vec2(lh_math_vec2_to_vec3_z0(a));
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
}

} // namespace
