#include <gtest/gtest.h>

#include <lh/math/vec3.h>

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
    const lh_math_vec3_t a = lh_math_vec3_make(1.0f, 2.0f, 3.0f);
    const lh_math_vec3_t b = lh_math_vec3_make(4.0f, -5.0f, 0.5f);
    expect_vec3(a, 1.0f, 2.0f, 3.0f);
    expect_vec3(lh_math_vec3_add(a, b), 5.0f, -3.0f, 3.5f);
    expect_vec3(lh_math_vec3_sub(a, b), -3.0f, 7.0f, 2.5f);
    expect_vec3(lh_math_vec3_scale(a, 2.0f), 2.0f, 4.0f, 6.0f);
    expect_vec3(lh_math_vec3_neg(a), -1.0f, -2.0f, -3.0f);
}

TEST(vec3, dot)
{
    const lh_math_vec3_t a = lh_math_vec3_make(1.0f, 2.0f, 3.0f);
    const lh_math_vec3_t b = lh_math_vec3_make(4.0f, -5.0f, 6.0f);
    EXPECT_FLOAT_EQ(lh_math_vec3_dot(a, b), 4.0f - 10.0f + 18.0f);
    // Perpendicular axes.
    EXPECT_FLOAT_EQ(lh_math_vec3_dot(lh_math_vec3_make(1, 0, 0), lh_math_vec3_make(0, 1, 0)), 0.0f);
}

TEST(vec3, length)
{
    const lh_math_vec3_t v = lh_math_vec3_make(2.0f, 3.0f, 6.0f); // 4 + 9 + 36 = 49
    EXPECT_FLOAT_EQ(lh_math_vec3_length_sq(v), 49.0f);
    EXPECT_FLOAT_EQ(lh_math_vec3_length(v), 7.0f);
    EXPECT_FLOAT_EQ(lh_math_vec3_length(lh_math_vec3_make(0, 0, 0)), 0.0f);
}

TEST(vec3, normalize)
{
    const lh_math_vec3_t n = lh_math_vec3_normalize(lh_math_vec3_make(2.0f, 3.0f, 6.0f));
    expect_vec3(n, 2.0f / 7.0f, 3.0f / 7.0f, 6.0f / 7.0f);
    EXPECT_NEAR(lh_math_vec3_length(n), 1.0f, k_eps);
}

TEST(vec3, normalize_zero_stays_zero)
{
    expect_vec3(lh_math_vec3_normalize(lh_math_vec3_make(0, 0, 0)), 0.0f, 0.0f, 0.0f);
}

TEST(vec3, cross_of_axes_is_the_third_axis)
{
    const lh_math_vec3_t x = lh_math_vec3_make(1, 0, 0);
    const lh_math_vec3_t y = lh_math_vec3_make(0, 1, 0);
    const lh_math_vec3_t z = lh_math_vec3_make(0, 0, 1);
    expect_vec3(lh_math_vec3_cross(x, y), 0, 0, 1);
    expect_vec3(lh_math_vec3_cross(y, z), 1, 0, 0);
    expect_vec3(lh_math_vec3_cross(z, x), 0, 1, 0);
    expect_vec3(lh_math_vec3_cross(y, x), 0, 0, -1); // anticommutative
}

TEST(vec3, cross_is_perpendicular_to_both)
{
    const lh_math_vec3_t a = lh_math_vec3_make(1.5f, -2.0f, 0.25f);
    const lh_math_vec3_t b = lh_math_vec3_make(-3.0f, 0.5f, 4.0f);
    const lh_math_vec3_t c = lh_math_vec3_cross(a, b);
    EXPECT_NEAR(lh_math_vec3_dot(c, a), 0.0f, 1e-5f);
    EXPECT_NEAR(lh_math_vec3_dot(c, b), 0.0f, 1e-5f);
    // Parallel vectors have no perpendicular: zero.
    expect_vec3(lh_math_vec3_cross(a, lh_math_vec3_scale(a, 3.0f)), 0, 0, 0);
}

TEST(vec3, lerp)
{
    const lh_math_vec3_t a = lh_math_vec3_make(0.0f, 10.0f, -4.0f);
    const lh_math_vec3_t b = lh_math_vec3_make(10.0f, 20.0f, 4.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 0.0f), 0.0f, 10.0f, -4.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 1.0f), 10.0f, 20.0f, 4.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 0.5f), 5.0f, 15.0f, 0.0f);
    expect_vec3(lh_math_vec3_lerp(a, b, 2.0f), 20.0f, 30.0f, 12.0f); // not clamped
}

TEST(vec3, near)
{
    const lh_math_vec3_t a = lh_math_vec3_make(1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(lh_math_vec3_near(a, lh_math_vec3_make(1.0f, 2.0f, 3.0f), 0.0f));
    EXPECT_TRUE(lh_math_vec3_near(a, lh_math_vec3_make(1.0005f, 1.9995f, 3.0f), 0.001f));
    EXPECT_FALSE(lh_math_vec3_near(a, lh_math_vec3_make(1.0f, 2.0f, 3.01f), 0.001f));
    EXPECT_FALSE(lh_math_vec3_near(a, lh_math_vec3_make(0.99f, 2.0f, 3.0f), 0.001f));
}

} // namespace
