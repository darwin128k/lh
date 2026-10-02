#include <gtest/gtest.h>

#include <lh/quat.h>

#include <cmath>
#include <cstddef>

namespace
{

const lh_float_t k_pi = 3.14159265f;
const lh_float_t k_eps = 1e-5f;

void
expect_vec3_near(lh_vec3_t v, lh_float_t x, lh_float_t y, lh_float_t z)
{
    EXPECT_NEAR(v.x, x, k_eps);
    EXPECT_NEAR(v.y, y, k_eps);
    EXPECT_NEAR(v.z, z, k_eps);
}

TEST(quat, layout_is_xyzw)
{
    EXPECT_EQ(sizeof(lh_quat_t), 4 * sizeof(float));
    EXPECT_EQ(offsetof(lh_quat_t, w), 3 * sizeof(float));
}

TEST(quat, to_and_from_vec4_keep_the_numbers)
{
    const lh_vec4_t v = lh_quat_to_vec4(lh_quat_make(1, 2, 3, 4));
    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.w, 4.0f);
    const lh_quat_t q = lh_quat_from_vec4(v);
    EXPECT_FLOAT_EQ(q.y, 2.0f);
    EXPECT_FLOAT_EQ(q.z, 3.0f);
}

TEST(quat, identity_rotates_nothing)
{
    expect_vec3_near(lh_quat_rotate(lh_quat_identity(), lh_vec3_make(1, 2, 3)), 1, 2, 3);
}

TEST(quat, quarter_turns_about_each_axis)
{
    const lh_quat_t about_z = lh_quat_from_axis_angle(lh_vec3_make(0, 0, 1), k_pi / 2);
    expect_vec3_near(lh_quat_rotate(about_z, lh_vec3_make(1, 0, 0)), 0, 1, 0);
    expect_vec3_near(lh_quat_rotate(about_z, lh_vec3_make(0, 1, 0)), -1, 0, 0);
    expect_vec3_near(lh_quat_rotate(about_z, lh_vec3_make(0, 0, 1)), 0, 0, 1);

    const lh_quat_t about_x = lh_quat_from_axis_angle(lh_vec3_make(1, 0, 0), k_pi / 2);
    expect_vec3_near(lh_quat_rotate(about_x, lh_vec3_make(0, 1, 0)), 0, 0, 1);

    const lh_quat_t about_y = lh_quat_from_axis_angle(lh_vec3_make(0, 1, 0), k_pi / 2);
    expect_vec3_near(lh_quat_rotate(about_y, lh_vec3_make(0, 0, 1)), 1, 0, 0);
}

TEST(quat, from_axis_angle_matches_libm_over_many_turns)
{
    // The half-angle sine and cosine come from lh's own range reduction and
    // polynomials; compare them against the C library across +-16 turns.
    for (int i = -2000; i <= 2000; ++i)
    {
        const lh_float_t angle = static_cast<lh_float_t>(i) * 0.05f;
        const lh_quat_t q = lh_quat_from_axis_angle(lh_vec3_make(0, 0, 1), angle);
        ASSERT_NEAR(q.z, std::sin(angle * 0.5f), 2e-6f) << "angle " << angle;
        ASSERT_NEAR(q.w, std::cos(angle * 0.5f), 2e-6f) << "angle " << angle;
    }
}

TEST(quat, mul_composes_right_to_left)
{
    const lh_quat_t a = lh_quat_from_axis_angle(lh_vec3_make(0, 0, 1), k_pi / 2);
    const lh_quat_t b = lh_quat_from_axis_angle(lh_vec3_make(1, 0, 0), k_pi / 2);
    const lh_vec3_t v = lh_vec3_make(0.3f, -1.2f, 2.0f);
    const lh_vec3_t step = lh_quat_rotate(a, lh_quat_rotate(b, v));
    const lh_vec3_t both = lh_quat_rotate(lh_quat_mul(a, b), v);
    expect_vec3_near(both, step.x, step.y, step.z);
}

TEST(quat, conjugate_undoes_rotation)
{
    const lh_quat_t q = lh_quat_from_axis_angle(lh_vec3_normalize(lh_vec3_make(1, 2, 3)), 0.7f);
    const lh_vec3_t v = lh_vec3_make(4, 5, 6);
    expect_vec3_near(lh_quat_rotate(lh_quat_conjugate(q), lh_quat_rotate(q, v)), 4, 5, 6);
}

TEST(quat, normalize)
{
    const lh_quat_t q = lh_quat_normalize(lh_quat_make(0, 0, 3, 4));
    EXPECT_FLOAT_EQ(q.z, 0.6f);
    EXPECT_FLOAT_EQ(q.w, 0.8f);
    const lh_quat_t zero = lh_quat_normalize(lh_quat_make(0, 0, 0, 0));
    EXPECT_FLOAT_EQ(zero.w, 0.0f);
}

TEST(quat, nlerp_ends_and_short_way)
{
    const lh_vec3_t z = lh_vec3_make(0, 0, 1);
    const lh_quat_t a = lh_quat_from_axis_angle(z, 0.0f);
    const lh_quat_t b = lh_quat_from_axis_angle(z, k_pi / 2);
    expect_vec3_near(lh_quat_rotate(lh_quat_nlerp(a, b, 0.0f), lh_vec3_make(1, 0, 0)), 1, 0, 0);
    expect_vec3_near(lh_quat_rotate(lh_quat_nlerp(a, b, 1.0f), lh_vec3_make(1, 0, 0)), 0, 1, 0);

    // Halfway is 45 degrees, also when b is given as its negation (-b is the
    // same rotation and must not send the path the long way round).
    const lh_quat_t neg_b = lh_quat_make(-b.x, -b.y, -b.z, -b.w);
    const lh_float_t h = std::sqrt(0.5f);
    expect_vec3_near(lh_quat_rotate(lh_quat_nlerp(a, b, 0.5f), lh_vec3_make(1, 0, 0)), h, h, 0);
    expect_vec3_near(lh_quat_rotate(lh_quat_nlerp(a, neg_b, 0.5f), lh_vec3_make(1, 0, 0)), h, h, 0);
}

} // namespace
