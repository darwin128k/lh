#include <lh/assert/runtime.h>
#include <lh/math/mat4.h>

lh_math_mat4_t
lh_math_mat4_from_columns(lh_math_vec4_t c0, lh_math_vec4_t c1, lh_math_vec4_t c2, lh_math_vec4_t c3)
{
    lh_math_mat4_t m;
    lh_math_mat4_set_column(lh_addr_of(m), 0, c0);
    lh_math_mat4_set_column(lh_addr_of(m), 1, c1);
    lh_math_mat4_set_column(lh_addr_of(m), 2, c2);
    lh_math_mat4_set_column(lh_addr_of(m), 3, c3);
    return m;
}

lh_math_mat4_t
lh_math_mat4_identity(void)
{
    return lh_math_mat4_from_scale(lh_math_vec3_make(1.0f, 1.0f, 1.0f));
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_vec4_t
lh_math_mat4_get_column(const lh_math_mat4_t *self, unsigned index)
{
    lh_assert_runtime_ref(self);
    return self->columns[index];
}

lh_void
lh_math_mat4_set_column(lh_math_mat4_t *self, unsigned index, lh_math_vec4_t column)
{
    lh_assert_runtime_ref(self);
    self->columns[index] = column;
}

lh_math_mat4_t
lh_math_mat4_from_translation(lh_math_vec3_t offset)
{
    lh_math_mat4_t m = lh_math_mat4_identity();
    lh_math_mat4_set_column(lh_addr_of(m), 3, lh_math_vec4_make(lh_math_vec3_get_x(lh_addr_of(offset)),
                                                       lh_math_vec3_get_y(lh_addr_of(offset)),
                                                       lh_math_vec3_get_z(lh_addr_of(offset)),
                                                       1.0f));
    return m;
}

lh_math_mat4_t
lh_math_mat4_from_scale(lh_math_vec3_t factors)
{
    return lh_math_mat4_from_columns(
        lh_math_vec4_make(lh_math_vec3_get_x(lh_addr_of(factors)), 0.0f, 0.0f, 0.0f),
        lh_math_vec4_make(0.0f, lh_math_vec3_get_y(lh_addr_of(factors)), 0.0f, 0.0f),
        lh_math_vec4_make(0.0f, 0.0f, lh_math_vec3_get_z(lh_addr_of(factors)), 0.0f),
        lh_math_vec4_make(0.0f, 0.0f, 0.0f, 1.0f));
}

lh_math_mat4_t
lh_math_mat4_from_quat(lh_math_quat_t q)
{
    const lh_float_t xx = lh_math_quat_get_x(lh_addr_of(q)) * lh_math_quat_get_x(lh_addr_of(q));
    const lh_float_t yy = lh_math_quat_get_y(lh_addr_of(q)) * lh_math_quat_get_y(lh_addr_of(q));
    const lh_float_t zz = lh_math_quat_get_z(lh_addr_of(q)) * lh_math_quat_get_z(lh_addr_of(q));
    const lh_float_t xy = lh_math_quat_get_x(lh_addr_of(q)) * lh_math_quat_get_y(lh_addr_of(q));
    const lh_float_t xz = lh_math_quat_get_x(lh_addr_of(q)) * lh_math_quat_get_z(lh_addr_of(q));
    const lh_float_t yz = lh_math_quat_get_y(lh_addr_of(q)) * lh_math_quat_get_z(lh_addr_of(q));
    const lh_float_t wx = lh_math_quat_get_w(lh_addr_of(q)) * lh_math_quat_get_x(lh_addr_of(q));
    const lh_float_t wy = lh_math_quat_get_w(lh_addr_of(q)) * lh_math_quat_get_y(lh_addr_of(q));
    const lh_float_t wz = lh_math_quat_get_w(lh_addr_of(q)) * lh_math_quat_get_z(lh_addr_of(q));

    /* Column c is where the rotation takes axis c. */
    return lh_math_mat4_from_columns(
        lh_math_vec4_make(1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f),
        lh_math_vec4_make(2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f),
        lh_math_vec4_make(2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f),
        lh_math_vec4_make(0.0f, 0.0f, 0.0f, 1.0f));
}

lh_math_vec4_t
lh_math_mat4_mul_vec4(lh_math_mat4_t m, lh_math_vec4_t v)
{
    /* A sum of the columns, weighted by v's components. */
    const lh_math_vec4_t xy = lh_math_vec4_add(
        lh_math_vec4_scale(lh_math_mat4_get_column(lh_addr_of(m), 0), lh_math_vec4_get_x(lh_addr_of(v))),
        lh_math_vec4_scale(lh_math_mat4_get_column(lh_addr_of(m), 1), lh_math_vec4_get_y(lh_addr_of(v))));
    const lh_math_vec4_t zw = lh_math_vec4_add(
        lh_math_vec4_scale(lh_math_mat4_get_column(lh_addr_of(m), 2), lh_math_vec4_get_z(lh_addr_of(v))),
        lh_math_vec4_scale(lh_math_mat4_get_column(lh_addr_of(m), 3), lh_math_vec4_get_w(lh_addr_of(v))));
    return lh_math_vec4_add(xy, zw);
}

lh_math_mat4_t
lh_math_mat4_mul(lh_math_mat4_t a, lh_math_mat4_t b)
{
    /* Column c of the product is a applied to column c of b. */
    return lh_math_mat4_from_columns(lh_math_mat4_mul_vec4(a, lh_math_mat4_get_column(lh_addr_of(b), 0)),
                                     lh_math_mat4_mul_vec4(a, lh_math_mat4_get_column(lh_addr_of(b), 1)),
                                     lh_math_mat4_mul_vec4(a, lh_math_mat4_get_column(lh_addr_of(b), 2)),
                                     lh_math_mat4_mul_vec4(a, lh_math_mat4_get_column(lh_addr_of(b), 3)));
}

lh_math_vec3_t
lh_math_mat4_transform_point(lh_math_mat4_t m, lh_math_vec3_t p)
{
    const lh_math_vec4_t r = lh_math_mat4_mul_vec4(
        m, lh_math_vec4_make(lh_math_vec3_get_x(lh_addr_of(p)),
                             lh_math_vec3_get_y(lh_addr_of(p)),
                             lh_math_vec3_get_z(lh_addr_of(p)),
                             1.0f));
    return lh_math_vec3_make(lh_math_vec4_get_x(lh_addr_of(r)),
                             lh_math_vec4_get_y(lh_addr_of(r)),
                             lh_math_vec4_get_z(lh_addr_of(r)));
}

lh_math_vec3_t
lh_math_mat4_transform_dir(lh_math_mat4_t m, lh_math_vec3_t d)
{
    const lh_math_vec4_t r = lh_math_mat4_mul_vec4(
        m, lh_math_vec4_make(lh_math_vec3_get_x(lh_addr_of(d)),
                             lh_math_vec3_get_y(lh_addr_of(d)),
                             lh_math_vec3_get_z(lh_addr_of(d)),
                             0.0f));
    return lh_math_vec3_make(lh_math_vec4_get_x(lh_addr_of(r)),
                             lh_math_vec4_get_y(lh_addr_of(r)),
                             lh_math_vec4_get_z(lh_addr_of(r)));
}

lh_math_mat4_t
lh_math_mat4_transpose(lh_math_mat4_t m)
{
    const lh_math_vec4_t c0 = lh_math_mat4_get_column(lh_addr_of(m), 0);
    const lh_math_vec4_t c1 = lh_math_mat4_get_column(lh_addr_of(m), 1);
    const lh_math_vec4_t c2 = lh_math_mat4_get_column(lh_addr_of(m), 2);
    const lh_math_vec4_t c3 = lh_math_mat4_get_column(lh_addr_of(m), 3);
    return lh_math_mat4_from_columns(
        lh_math_vec4_make(lh_math_vec4_get_x(lh_addr_of(c0)), lh_math_vec4_get_x(lh_addr_of(c1)),
                          lh_math_vec4_get_x(lh_addr_of(c2)), lh_math_vec4_get_x(lh_addr_of(c3))),
        lh_math_vec4_make(lh_math_vec4_get_y(lh_addr_of(c0)), lh_math_vec4_get_y(lh_addr_of(c1)),
                          lh_math_vec4_get_y(lh_addr_of(c2)), lh_math_vec4_get_y(lh_addr_of(c3))),
        lh_math_vec4_make(lh_math_vec4_get_z(lh_addr_of(c0)), lh_math_vec4_get_z(lh_addr_of(c1)),
                          lh_math_vec4_get_z(lh_addr_of(c2)), lh_math_vec4_get_z(lh_addr_of(c3))),
        lh_math_vec4_make(lh_math_vec4_get_w(lh_addr_of(c0)), lh_math_vec4_get_w(lh_addr_of(c1)),
                          lh_math_vec4_get_w(lh_addr_of(c2)), lh_math_vec4_get_w(lh_addr_of(c3))));
}

lh_bool_t
lh_math_mat4_inverse(lh_math_mat4_t m, lh_math_mat4_t *out)
{
    lh_assert_runtime_ref(out);
    /* Cofactors over the adjugate (as in Mesa's gluInvertMatrix). Written
     * for one storage order, it holds for the other too: the inverse of the
     * transpose is the transpose of the inverse. */
    lh_float_t a[16]; /* column * 4 + row */
    lh_float_t inv[16];
    for (unsigned c = 0; c < 4; ++c)
    {
        const lh_math_vec4_t col = lh_math_mat4_get_column(lh_addr_of(m), c);
        a[c * 4 + 0] = lh_math_vec4_get_x(lh_addr_of(col));
        a[c * 4 + 1] = lh_math_vec4_get_y(lh_addr_of(col));
        a[c * 4 + 2] = lh_math_vec4_get_z(lh_addr_of(col));
        a[c * 4 + 3] = lh_math_vec4_get_w(lh_addr_of(col));
    }

    inv[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] +
             a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
    inv[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] -
             a[8] * a[7] * a[14] - a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
    inv[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] +
             a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
    inv[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] -
              a[8] * a[6] * a[13] - a[12] * a[5] * a[10] + a[12] * a[6] * a[9];
    inv[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] -
             a[9] * a[3] * a[14] - a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
    inv[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] +
             a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
    inv[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] -
             a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
    inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] +
              a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
    inv[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] +
             a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
    inv[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] -
             a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
    inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] +
              a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
    inv[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] -
              a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
    inv[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] -
             a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
    inv[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] +
             a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
    inv[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] -
              a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
    inv[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] +
              a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

    const lh_float_t det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];
    if (det == 0.0f)
    {
        return lh_bool_false;
    }

    const lh_float_t inv_det = 1.0f / det;
    for (unsigned c = 0; c < 4; ++c)
    {
        lh_math_mat4_set_column(out, c,
                                lh_math_vec4_make(inv[c * 4 + 0] * inv_det,
                                                  inv[c * 4 + 1] * inv_det,
                                                  inv[c * 4 + 2] * inv_det,
                                                  inv[c * 4 + 3] * inv_det));
    }
    return lh_bool_true;
}

lh_bool_t
lh_math_mat4_near(lh_math_mat4_t a, lh_math_mat4_t b, lh_float_t eps)
{
    for (unsigned c = 0; c < 4; ++c)
    {
        if (!lh_math_vec4_near(lh_math_mat4_get_column(lh_addr_of(a), c), lh_math_mat4_get_column(lh_addr_of(b), c), eps))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}