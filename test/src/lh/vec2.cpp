#include <gtest/gtest.h>

#include <lh/vec2.h>

#include <cstddef>

namespace
{

TEST(vec2, layout_is_two_consecutive_floats)
{
    EXPECT_EQ(sizeof(lh_vec2_t), 2 * sizeof(float));
    EXPECT_EQ(offsetof(lh_vec2_t, y), sizeof(float));
}

TEST(vec2, arithmetic)
{
    const lh_vec2_t a = lh_vec2_make(1.0f, 2.0f);
    const lh_vec2_t b = lh_vec2_make(3.0f, -4.0f);
    lh_vec2_t r = lh_vec2_add(a, b);
    EXPECT_FLOAT_EQ(r.x, 4.0f);
    EXPECT_FLOAT_EQ(r.y, -2.0f);
    r = lh_vec2_sub(a, b);
    EXPECT_FLOAT_EQ(r.x, -2.0f);
    EXPECT_FLOAT_EQ(r.y, 6.0f);
    r = lh_vec2_scale(a, -3.0f);
    EXPECT_FLOAT_EQ(r.x, -3.0f);
    EXPECT_FLOAT_EQ(r.y, -6.0f);
    r = lh_vec2_neg(b);
    EXPECT_FLOAT_EQ(r.x, -3.0f);
    EXPECT_FLOAT_EQ(r.y, 4.0f);
}

TEST(vec2, dot_length_normalize)
{
    const lh_vec2_t v = lh_vec2_make(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(lh_vec2_dot(v, lh_vec2_make(2.0f, -1.0f)), 2.0f);
    EXPECT_FLOAT_EQ(lh_vec2_length_sq(v), 25.0f);
    EXPECT_FLOAT_EQ(lh_vec2_length(v), 5.0f);
    const lh_vec2_t n = lh_vec2_normalize(v);
    EXPECT_FLOAT_EQ(n.x, 0.6f);
    EXPECT_FLOAT_EQ(n.y, 0.8f);
    const lh_vec2_t zero = lh_vec2_normalize(lh_vec2_make(0.0f, 0.0f));
    EXPECT_FLOAT_EQ(zero.x, 0.0f);
    EXPECT_FLOAT_EQ(zero.y, 0.0f);
}

TEST(vec2, lerp_and_near)
{
    const lh_vec2_t a = lh_vec2_make(0.0f, 0.0f);
    const lh_vec2_t b = lh_vec2_make(10.0f, -10.0f);
    const lh_vec2_t m = lh_vec2_lerp(a, b, 0.25f);
    EXPECT_FLOAT_EQ(m.x, 2.5f);
    EXPECT_FLOAT_EQ(m.y, -2.5f);
    EXPECT_TRUE(lh_vec2_near(m, lh_vec2_make(2.5001f, -2.4999f), 0.001f));
    EXPECT_FALSE(lh_vec2_near(m, lh_vec2_make(2.6f, -2.5f), 0.001f));
}

} // namespace
