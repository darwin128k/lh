#include <gtest/gtest.h>

#include <lh/math/vec4.h>

#include <cstddef>

namespace
{

TEST(vec4, layout_is_four_consecutive_floats)
{
    EXPECT_EQ(sizeof(lh_math_vec4_t), 4 * sizeof(float));
    EXPECT_EQ(offsetof(lh_math_vec4_t, w), 3 * sizeof(float));
}

TEST(vec4, arithmetic)
{
    const lh_math_vec4_t a = lh_math_vec4_make(1.0f, 2.0f, 3.0f, 4.0f);
    const lh_math_vec4_t b = lh_math_vec4_make(0.5f, -1.0f, 2.0f, 0.0f);
    lh_math_vec4_t r = lh_math_vec4_add(a, b);
    EXPECT_FLOAT_EQ(r.x, 1.5f);
    EXPECT_FLOAT_EQ(r.y, 1.0f);
    EXPECT_FLOAT_EQ(r.z, 5.0f);
    EXPECT_FLOAT_EQ(r.w, 4.0f);
    r = lh_math_vec4_sub(a, b);
    EXPECT_FLOAT_EQ(r.w, 4.0f);
    EXPECT_FLOAT_EQ(r.y, 3.0f);
    r = lh_math_vec4_scale(a, 0.5f);
    EXPECT_FLOAT_EQ(r.x, 0.5f);
    EXPECT_FLOAT_EQ(r.w, 2.0f);
    r = lh_math_vec4_neg(a);
    EXPECT_FLOAT_EQ(r.z, -3.0f);
}

TEST(vec4, dot_length_normalize)
{
    const lh_math_vec4_t v = lh_math_vec4_make(1.0f, 1.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(lh_math_vec4_dot(v, lh_math_vec4_make(1.0f, 2.0f, 3.0f, 4.0f)), 10.0f);
    EXPECT_FLOAT_EQ(lh_math_vec4_length_sq(v), 4.0f);
    EXPECT_FLOAT_EQ(lh_math_vec4_length(v), 2.0f);
    const lh_math_vec4_t n = lh_math_vec4_normalize(v);
    EXPECT_FLOAT_EQ(n.x, 0.5f);
    EXPECT_FLOAT_EQ(n.w, 0.5f);
    const lh_math_vec4_t zero = lh_math_vec4_normalize(lh_math_vec4_make(0, 0, 0, 0));
    EXPECT_FLOAT_EQ(zero.w, 0.0f);
}

TEST(vec4, lerp_and_near)
{
    const lh_math_vec4_t a = lh_math_vec4_make(0.0f, 0.0f, 0.0f, 1.0f);
    const lh_math_vec4_t b = lh_math_vec4_make(4.0f, 8.0f, -4.0f, 1.0f);
    const lh_math_vec4_t m = lh_math_vec4_lerp(a, b, 0.5f);
    EXPECT_TRUE(lh_math_vec4_near(m, lh_math_vec4_make(2.0f, 4.0f, -2.0f, 1.0f), 0.0f));
    EXPECT_FALSE(lh_math_vec4_near(m, lh_math_vec4_make(2.0f, 4.0f, -2.0f, 1.1f), 0.01f));
}

} // namespace
