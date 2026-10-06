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
    lh_math_vec4_t a;

    lh_math_vec4_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f, 4.0f);
    lh_math_vec4_t b;

    lh_math_vec4_init(lh_addr_of(b), 0.5f, -1.0f, 2.0f, 0.0f);
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
    lh_math_vec4_t v;

    lh_math_vec4_init(lh_addr_of(v), 1.0f, 1.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(lh_math_vec4_dot(v, ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 1.0f, 2.0f, 3.0f, 4.0f); return _v; })()), 10.0f);
    EXPECT_FLOAT_EQ(lh_math_vec4_length_sq(v), 4.0f);
    EXPECT_FLOAT_EQ(lh_math_vec4_length(v), 2.0f);
    const lh_math_vec4_t n = lh_math_vec4_normalize(v);
    EXPECT_FLOAT_EQ(n.x, 0.5f);
    EXPECT_FLOAT_EQ(n.w, 0.5f);
    const lh_math_vec4_t zero = lh_math_vec4_normalize(([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 0, 0, 0, 0); return _v; })());
    EXPECT_FLOAT_EQ(zero.w, 0.0f);
}

TEST(vec4, lerp_and_near)
{
    lh_math_vec4_t a;

    lh_math_vec4_init(lh_addr_of(a), 0.0f, 0.0f, 0.0f, 1.0f);
    lh_math_vec4_t b;

    lh_math_vec4_init(lh_addr_of(b), 4.0f, 8.0f, -4.0f, 1.0f);
    const lh_math_vec4_t m = lh_math_vec4_lerp(a, b, 0.5f);
    EXPECT_TRUE(lh_math_vec4_near(m, ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 2.0f, 4.0f, -2.0f, 1.0f); return _v; })(), 0.0f));
    EXPECT_FALSE(lh_math_vec4_near(m, ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 2.0f, 4.0f, -2.0f, 1.1f); return _v; })(), 0.01f));
}

/* ── conversions ────────────────────────────────────────────────────────── */

TEST(vec4, vec3_to_vec4_sets_w)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f);
    lh_math_vec4_t r = lh_math_vec3_to_vec4(a, 1.0f);
    EXPECT_FLOAT_EQ(r.x, 1.0f);
    EXPECT_FLOAT_EQ(r.y, 2.0f);
    EXPECT_FLOAT_EQ(r.z, 3.0f);
    EXPECT_FLOAT_EQ(r.w, 1.0f);
    // w = 0 is the direction form used by mat4_transform_dir.
    r = lh_math_vec3_to_vec4(a, 0.0f);
    EXPECT_FLOAT_EQ(r.w, 0.0f);
}

TEST(vec4, vec4_to_vec3_drops_w)
{
    lh_math_vec4_t a;

    lh_math_vec4_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f, 99.0f);
    const lh_math_vec3_t b = lh_math_vec4_to_vec3(a);
    EXPECT_FLOAT_EQ(b.x, 1.0f);
    EXPECT_FLOAT_EQ(b.y, 2.0f);
    EXPECT_FLOAT_EQ(b.z, 3.0f);
}

TEST(vec4, vec3_round_trip_keeps_xyz)
{
    lh_math_vec3_t a;

    lh_math_vec3_init(lh_addr_of(a), 1.5f, -2.25f, 3.125f);
    const lh_math_vec3_t b = lh_math_vec4_to_vec3(lh_math_vec3_to_vec4(a, 7.0f));
    EXPECT_FLOAT_EQ(b.x, a.x);
    EXPECT_FLOAT_EQ(b.y, a.y);
    EXPECT_FLOAT_EQ(b.z, a.z);
}

} // namespace
