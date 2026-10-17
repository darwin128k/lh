#include <gtest/gtest.h>

#include <lh/math/quat.h>
#include <lh/util/addr.h>
#include <lh/test/out.h>
#include <lh/math/quat/initializer.h>

#include <cmath>
#include <cstddef>

namespace
{

const lh_float_t k_pi = 3.14159265f;
const lh_float_t k_eps = 1e-5f;

void
expect_vec3_near(lh_math_vec3_t v, lh_float_t x, lh_float_t y, lh_float_t z)
{
    EXPECT_NEAR(v.x, x, k_eps);
    EXPECT_NEAR(v.y, y, k_eps);
    EXPECT_NEAR(v.z, z, k_eps);
}

TEST(quat, layout_is_xyzw)
{
    EXPECT_EQ(sizeof(lh_math_quat_t), 4 * sizeof(float));
    EXPECT_EQ(offsetof(lh_math_quat_t, w), 3 * sizeof(float));
}

TEST(quat, to_and_from_vec4_keep_the_numbers)
{
    const lh_math_vec4_t v = lh_test::out(lh_math_quat_to_vec4, ([&]() { lh_math_quat_t _v; lh_math_quat_init(lh_addr_of(_v), 1, 2, 3, 4); return _v; })());
    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.w, 4.0f);
    const lh_math_quat_t q = lh_test::out(lh_math_quat_from_vec4, v);
    EXPECT_FLOAT_EQ(q.y, 2.0f);
    EXPECT_FLOAT_EQ(q.z, 3.0f);
}

TEST(quat, identity_rotates_nothing)
{
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, lh_test::out(lh_math_quat_identity), ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 2, 3); return _v; })()), 1, 2, 3);
}

TEST(quat, quarter_turns_about_each_axis)
{
    const lh_math_quat_t about_z = lh_test::out(lh_math_quat_from_axis_angle, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 1); return _v; })(), k_pi / 2);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, about_z, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })()), 0, 1, 0);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, about_z, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 1, 0); return _v; })()), -1, 0, 0);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, about_z, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 1); return _v; })()), 0, 0, 1);

    const lh_math_quat_t about_x = lh_test::out(lh_math_quat_from_axis_angle, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })(), k_pi / 2);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, about_x, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 1, 0); return _v; })()), 0, 0, 1);

    const lh_math_quat_t about_y = lh_test::out(lh_math_quat_from_axis_angle, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 1, 0); return _v; })(), k_pi / 2);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, about_y, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 1); return _v; })()), 1, 0, 0);
}

TEST(quat, from_axis_angle_matches_libm_over_many_turns)
{
    // The half-angle sine and cosine come from lh's own range reduction and
    // polynomials; compare them against the C library across +-16 turns.
    for (int i = -2000; i <= 2000; ++i)
    {
        const lh_float_t angle = static_cast<lh_float_t>(i) * 0.05f;
        const lh_math_quat_t q = lh_test::out(lh_math_quat_from_axis_angle, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 1); return _v; })(), angle);
        ASSERT_NEAR(q.z, std::sin(angle * 0.5f), 2e-6f) << "angle " << angle;
        ASSERT_NEAR(q.w, std::cos(angle * 0.5f), 2e-6f) << "angle " << angle;
    }
}

TEST(quat, mul_composes_right_to_left)
{
    const lh_math_quat_t a = lh_test::out(lh_math_quat_from_axis_angle, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 1); return _v; })(), k_pi / 2);
    const lh_math_quat_t b = lh_test::out(lh_math_quat_from_axis_angle, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })(), k_pi / 2);
    lh_math_vec3_t v;

    lh_math_vec3_init(lh_addr_of(v), 0.3f, -1.2f, 2.0f);
    const lh_math_vec3_t step = lh_test::out(lh_math_quat_rotate, a, lh_test::out(lh_math_quat_rotate, b, v));
    const lh_math_vec3_t both = lh_test::out(lh_math_quat_rotate, lh_test::out(lh_math_quat_mul, a, b), v);
    expect_vec3_near(both, step.x, step.y, step.z);
}

TEST(quat, conjugate_undoes_rotation)
{
    const lh_math_quat_t q = lh_test::out(lh_math_quat_from_axis_angle, lh_test::out(lh_math_vec3_normalize, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 2, 3); return _v; })()), 0.7f);
    lh_math_vec3_t v;

    lh_math_vec3_init(lh_addr_of(v), 4, 5, 6);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, lh_test::out(lh_math_quat_conjugate, q), lh_test::out(lh_math_quat_rotate, q, v)), 4, 5, 6);
}

TEST(quat, normalize)
{
    const lh_math_quat_t q = lh_test::out(lh_math_quat_normalize, ([&]() { lh_math_quat_t _v; lh_math_quat_init(lh_addr_of(_v), 0, 0, 3, 4); return _v; })());
    EXPECT_FLOAT_EQ(q.z, 0.6f);
    EXPECT_FLOAT_EQ(q.w, 0.8f);
    const lh_math_quat_t zero = lh_test::out(lh_math_quat_normalize, ([&]() { lh_math_quat_t _v; lh_math_quat_init(lh_addr_of(_v), 0, 0, 0, 0); return _v; })());
    EXPECT_FLOAT_EQ(zero.w, 0.0f);
}

TEST(quat, nlerp_ends_and_short_way)
{
    lh_math_vec3_t z;

    lh_math_vec3_init(lh_addr_of(z), 0, 0, 1);
    const lh_math_quat_t a = lh_test::out(lh_math_quat_from_axis_angle, z, 0.0f);
    const lh_math_quat_t b = lh_test::out(lh_math_quat_from_axis_angle, z, k_pi / 2);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, lh_test::out(lh_math_quat_nlerp, a, b, 0.0f), ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })()), 1, 0, 0);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, lh_test::out(lh_math_quat_nlerp, a, b, 1.0f), ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })()), 0, 1, 0);

    // Halfway is 45 degrees, also when b is given as its negation (-b is the
    // same rotation and must not send the path the long way round).
    lh_math_quat_t neg_b;

    lh_math_quat_init(lh_addr_of(neg_b), -b.x, -b.y, -b.z, -b.w);
    const lh_float_t h = std::sqrt(0.5f);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, lh_test::out(lh_math_quat_nlerp, a, b, 0.5f), ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })()), h, h, 0);
    expect_vec3_near(lh_test::out(lh_math_quat_rotate, lh_test::out(lh_math_quat_nlerp, a, neg_b, 0.5f), ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })()), h, h, 0);
}

TEST(math_quat_initializer, fills_components_in_order)
{
    const lh_math_quat_t value = lh_math_quat_initializer(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_FLOAT_EQ(value.x, 1.0f);
    EXPECT_FLOAT_EQ(value.y, 2.0f);
    EXPECT_FLOAT_EQ(value.z, 3.0f);
    EXPECT_FLOAT_EQ(value.w, 4.0f);
}

} // namespace

TEST(math_quat_bricks, axes_are_where_the_rotation_takes_them)
{
    lh_math_vec3_t z;
    lh_math_quat_t q;

    /* 90 degrees about z takes x to y, y to -x and leaves z. */
    lh_math_vec3_init(&z, 0.0f, 0.0f, 1.0f);
    lh_math_quat_from_axis_angle(z, k_pi / 2.0f, &q);
    expect_vec3_near(lh_test::out(lh_math_quat_axis_x, q), 0.0f, 1.0f, 0.0f);
    expect_vec3_near(lh_test::out(lh_math_quat_axis_y, q), -1.0f, 0.0f, 0.0f);
    expect_vec3_near(lh_test::out(lh_math_quat_axis_z, q), 0.0f, 0.0f, 1.0f);
    expect_vec3_near(lh_test::out(lh_math_quat_axis_cyclic, 0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f, 0.0f);
}
