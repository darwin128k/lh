#include <gtest/gtest.h>

#include <lh/math/mat4.h>

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

TEST(mat4, layout_is_sixteen_floats_column_major)
{
    EXPECT_EQ(sizeof(lh_math_mat4_t), 16 * sizeof(float));
    const lh_math_mat4_t m = lh_math_mat4_from_translation(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 7, 8, 9); return _v; })());
    // OpenGL's layout: the translation sits in elements 12, 13, 14.
    const float *f = &m.columns[0].x;
    EXPECT_FLOAT_EQ(f[12], 7.0f);
    EXPECT_FLOAT_EQ(f[13], 8.0f);
    EXPECT_FLOAT_EQ(f[14], 9.0f);
    EXPECT_FLOAT_EQ(f[15], 1.0f);
}

TEST(mat4, translation_moves_points_not_directions)
{
    const lh_math_mat4_t t = lh_math_mat4_from_translation(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 2, 3); return _v; })());
    expect_vec3_near(lh_math_mat4_transform_point(t, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 10, 20, 30); return _v; })()), 11, 22, 33);
    expect_vec3_near(lh_math_mat4_transform_dir(t, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 10, 20, 30); return _v; })()), 10, 20, 30);
}

TEST(mat4, scale)
{
    const lh_math_mat4_t s = lh_math_mat4_from_scale(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 2, 3, -1); return _v; })());
    expect_vec3_near(lh_math_mat4_transform_point(s, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 1, 1); return _v; })()), 2, 3, -1);
}

TEST(mat4, from_quat_matches_quat_rotate)
{
    const lh_math_quat_t q =
        lh_math_quat_from_axis_angle(lh_math_vec3_normalize(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), -1, 2, 0.5f); return _v; })()), 1.3f);
    const lh_math_mat4_t m = lh_math_mat4_from_quat(q);
    lh_math_vec3_t v;

    lh_math_vec3_init(lh_addr_of(v), 0.4f, -2, 5);
    const lh_math_vec3_t expected = lh_math_quat_rotate(q, v);
    expect_vec3_near(lh_math_mat4_transform_dir(m, v), expected.x, expected.y, expected.z);
}

TEST(mat4, mul_applies_right_operand_first)
{
    // Scale, then rotate a quarter turn about z, then move.
    const lh_math_mat4_t s = lh_math_mat4_from_scale(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 2, 2, 2); return _v; })());
    const lh_math_mat4_t r =
        lh_math_mat4_from_quat(lh_math_quat_from_axis_angle(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 0, 1); return _v; })(), k_pi / 2));
    const lh_math_mat4_t t = lh_math_mat4_from_translation(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 10, 0, 0); return _v; })());
    const lh_math_mat4_t trs = lh_math_mat4_mul(t, lh_math_mat4_mul(r, s));
    // (1,0,0) -> (2,0,0) -> (0,2,0) -> (10,2,0)
    expect_vec3_near(lh_math_mat4_transform_point(trs, ([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 0); return _v; })()), 10, 2, 0);
}

TEST(mat4, identity_is_neutral)
{
    const lh_math_mat4_t m = lh_math_mat4_from_translation(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 2, 3); return _v; })());
    EXPECT_TRUE(lh_math_mat4_near(lh_math_mat4_mul(m, lh_math_mat4_identity()), m, 0.0f));
    EXPECT_TRUE(lh_math_mat4_near(lh_math_mat4_mul(lh_math_mat4_identity(), m), m, 0.0f));
}

TEST(mat4, transpose)
{
    const lh_math_mat4_t m =
        lh_math_mat4_from_columns(([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 1, 2, 3, 4); return _v; })(), ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 5, 6, 7, 8); return _v; })(),
                             ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 9, 10, 11, 12); return _v; })(), ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 13, 14, 15, 16); return _v; })());
    const lh_math_mat4_t t = lh_math_mat4_transpose(m);
    EXPECT_FLOAT_EQ(t.columns[0].y, 5.0f);
    EXPECT_FLOAT_EQ(t.columns[3].x, 4.0f);
    EXPECT_FLOAT_EQ(t.columns[2].w, 15.0f);
    EXPECT_TRUE(lh_math_mat4_near(lh_math_mat4_transpose(t), m, 0.0f));
}

TEST(mat4, inverse_of_a_general_matrix)
{
    // Full rank, no special structure, so every cofactor term matters.
    const lh_math_mat4_t m =
        lh_math_mat4_from_columns(([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 2, 1, 0, 1); return _v; })(), ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), -1, 3, 2, 0); return _v; })(),
                             ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 0, 1, 4, -2); return _v; })(), ([&]() { lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), 1, 0, 1, 3); return _v; })());
    lh_math_mat4_t inv;
    ASSERT_TRUE(lh_math_mat4_inverse(m, &inv));
    EXPECT_TRUE(lh_math_mat4_near(lh_math_mat4_mul(m, inv), lh_math_mat4_identity(), k_eps));
    EXPECT_TRUE(lh_math_mat4_near(lh_math_mat4_mul(inv, m), lh_math_mat4_identity(), k_eps));
}

TEST(mat4, inverse_undoes_a_transform)
{
    const lh_math_mat4_t m = lh_math_mat4_mul(
        lh_math_mat4_from_translation(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 5, -3, 2); return _v; })()),
        lh_math_mat4_from_quat(lh_math_quat_from_axis_angle(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 0, 1, 0); return _v; })(), 0.9f)));
    lh_math_mat4_t inv;
    ASSERT_TRUE(lh_math_mat4_inverse(m, &inv));
    lh_math_vec3_t p;

    lh_math_vec3_init(lh_addr_of(p), 1, 2, 3);
    expect_vec3_near(lh_math_mat4_transform_point(inv, lh_math_mat4_transform_point(m, p)), 1, 2, 3);
}

TEST(mat4, singular_matrix_has_no_inverse)
{
    const lh_math_mat4_t flat = lh_math_mat4_from_scale(([&]() { lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), 1, 0, 1); return _v; })());
    lh_math_mat4_t out = lh_math_mat4_identity();
    EXPECT_FALSE(lh_math_mat4_inverse(flat, &out));
    EXPECT_TRUE(lh_math_mat4_near(out, lh_math_mat4_identity(), 0.0f)); // untouched
}

} // namespace
