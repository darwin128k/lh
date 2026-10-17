#include <gtest/gtest.h>

#include <lh/math/vec2.h>
#include <lh/util/addr.h>
#include <lh/test/out.h>
#include <lh/math/vec2/initializer.h>

#include <cstddef>

namespace
{

TEST(vec2, layout_is_two_consecutive_floats)
{
    EXPECT_EQ(sizeof(lh_math_vec2_t), 2 * sizeof(float));
    EXPECT_EQ(offsetof(lh_math_vec2_t, y), sizeof(float));
}

TEST(vec2, arithmetic)
{
    lh_math_vec2_t a;

    lh_math_vec2_init(lh_addr_of(a), 1.0f, 2.0f);
    lh_math_vec2_t b;

    lh_math_vec2_init(lh_addr_of(b), 3.0f, -4.0f);
    lh_math_vec2_t r = lh_test::out(lh_math_vec2_add, a, b);
    EXPECT_FLOAT_EQ(r.x, 4.0f);
    EXPECT_FLOAT_EQ(r.y, -2.0f);
    r = lh_test::out(lh_math_vec2_sub, a, b);
    EXPECT_FLOAT_EQ(r.x, -2.0f);
    EXPECT_FLOAT_EQ(r.y, 6.0f);
    r = lh_test::out(lh_math_vec2_scale, a, -3.0f);
    EXPECT_FLOAT_EQ(r.x, -3.0f);
    EXPECT_FLOAT_EQ(r.y, -6.0f);
    r = lh_test::out(lh_math_vec2_neg, b);
    EXPECT_FLOAT_EQ(r.x, -3.0f);
    EXPECT_FLOAT_EQ(r.y, 4.0f);
}

TEST(vec2, dot_length_normalize)
{
    lh_math_vec2_t v;

    lh_math_vec2_init(lh_addr_of(v), 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(lh_math_vec2_dot(v, ([&]() { lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), 2.0f, -1.0f); return _v; })()), 2.0f);
    EXPECT_FLOAT_EQ(lh_math_vec2_length_sq(v), 25.0f);
    EXPECT_FLOAT_EQ(lh_math_vec2_length(v), 5.0f);
    const lh_math_vec2_t n = lh_test::out(lh_math_vec2_normalize, v);
    EXPECT_FLOAT_EQ(n.x, 0.6f);
    EXPECT_FLOAT_EQ(n.y, 0.8f);
    const lh_math_vec2_t zero = lh_test::out(lh_math_vec2_normalize, ([&]() { lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), 0.0f, 0.0f); return _v; })());
    EXPECT_FLOAT_EQ(zero.x, 0.0f);
    EXPECT_FLOAT_EQ(zero.y, 0.0f);
}

TEST(vec2, lerp_and_near)
{
    lh_math_vec2_t a;

    lh_math_vec2_init(lh_addr_of(a), 0.0f, 0.0f);
    lh_math_vec2_t b;

    lh_math_vec2_init(lh_addr_of(b), 10.0f, -10.0f);
    const lh_math_vec2_t m = lh_test::out(lh_math_vec2_lerp, a, b, 0.25f);
    EXPECT_FLOAT_EQ(m.x, 2.5f);
    EXPECT_FLOAT_EQ(m.y, -2.5f);
    EXPECT_TRUE(lh_math_vec2_near(m, ([&]() { lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), 2.5001f, -2.4999f); return _v; })(), 0.001f));
    EXPECT_FALSE(lh_math_vec2_near(m, ([&]() { lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), 2.6f, -2.5f); return _v; })(), 0.001f));
}

TEST(vec2, view_of_reads_the_components_in_order)
{
    lh_math_vec2_t v;

    lh_math_vec2_init(lh_addr_of(v), 3.0f, -5.0f);
    const lh_math_view_t view = lh_test::out(lh_math_vec2_view_of, lh_addr_of(v));

    EXPECT_EQ(lh_math_view_get_size(lh_addr_of(view)), 2U);
    EXPECT_EQ(lh_math_view_get_at(lh_addr_of(view), 0U), lh_math_vec2_get_x(lh_addr_of(v)));
    EXPECT_EQ(lh_math_view_get_at(lh_addr_of(view), 1U), lh_math_vec2_get_y(lh_addr_of(v)));
}

TEST(math_vec2_initializer, fills_components_in_order)
{
    const lh_math_vec2_t value = lh_math_vec2_initializer(1.0f, 2.0f);

    EXPECT_FLOAT_EQ(value.x, 1.0f);
    EXPECT_FLOAT_EQ(value.y, 2.0f);
}

} // namespace
