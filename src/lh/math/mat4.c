#include <lh/assert/runtime.h>
#include <lh/math/mat4.h>

lh_void
lh_math_mat4_get_column(const lh_math_mat4_t *self, unsigned index, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    lh_ptr_deref(out) = self->columns[index];
}

lh_void
lh_math_mat4_set_column(lh_math_mat4_t *self, unsigned index, lh_math_vec4_t column)
{
    lh_assert_runtime_ref(self);
    self->columns[index] = column;
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_void
lh_math_mat4_from_columns(lh_math_vec4_t c0, lh_math_vec4_t c1, lh_math_vec4_t c2, lh_math_vec4_t c3, lh_math_mat4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_mat4_t m;
    lh_math_mat4_set_column(lh_addr_of(m), 0, c0);
    lh_math_mat4_set_column(lh_addr_of(m), 1, c1);
    lh_math_mat4_set_column(lh_addr_of(m), 2, c2);
    lh_math_mat4_set_column(lh_addr_of(m), 3, c3);
    lh_ptr_deref(out) = m;
}

lh_void
lh_math_mat4_from_scale(lh_math_vec3_t factors, lh_math_mat4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t column0;
    lh_math_vec4_init(lh_addr_of(column0), lh_math_vec3_get_x(lh_addr_of(factors)), 0.0f, 0.0f,
                      0.0f);
    lh_math_vec4_t column1;
    lh_math_vec4_init(lh_addr_of(column1), 0.0f, lh_math_vec3_get_y(lh_addr_of(factors)), 0.0f,
                      0.0f);
    lh_math_vec4_t column2;
    lh_math_vec4_init(lh_addr_of(column2), 0.0f, 0.0f, lh_math_vec3_get_z(lh_addr_of(factors)),
                      0.0f);
    lh_math_vec4_t column3;
    lh_math_vec4_init(lh_addr_of(column3), 0.0f, 0.0f, 0.0f, 1.0f);
    lh_math_mat4_from_columns(column0, column1, column2, column3, out);
}

lh_void
lh_math_mat4_identity(lh_math_mat4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec3_t result;
    lh_math_vec3_init(lh_addr_of(result), 1.0f, 1.0f, 1.0f);
    lh_math_mat4_from_scale(result, out);
}

lh_void
lh_math_mat4_from_translation(lh_math_vec3_t offset, lh_math_mat4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_mat4_t m;
    lh_math_mat4_identity(lh_addr_of(m));
    lh_math_vec4_t vec4;
    lh_math_vec4_init(lh_addr_of(vec4), lh_math_vec3_get_x(lh_addr_of(offset)),
                      lh_math_vec3_get_y(lh_addr_of(offset)),
                      lh_math_vec3_get_z(lh_addr_of(offset)), 1.0f);
    lh_math_mat4_set_column(lh_addr_of(m), 3, vec4);
    lh_ptr_deref(out) = m;
}

lh_void
lh_math_mat4_set_column_vec3(lh_math_mat4_t *self, unsigned index, lh_math_vec3_t xyz, lh_math_fscalar_t w)
{
    lh_math_vec4_t column;

    lh_math_vec4_init(lh_addr_of(column), lh_math_vec3_get_x(lh_addr_of(xyz)), lh_math_vec3_get_y(lh_addr_of(xyz)),
                      lh_math_vec3_get_z(lh_addr_of(xyz)), w);
    lh_math_mat4_set_column(self, index, column);
}

lh_void
lh_math_mat4_from_quat(lh_math_quat_t q, lh_math_mat4_t *out)
{
    lh_math_vec3_t axis;

    /* Column c is where the rotation takes axis c; the last stays (0, 0, 0, 1). */
    lh_math_mat4_identity(out);
    lh_math_quat_axis_x(q, lh_addr_of(axis));
    lh_math_mat4_set_column_vec3(out, 0, axis, 0.0f);
    lh_math_quat_axis_y(q, lh_addr_of(axis));
    lh_math_mat4_set_column_vec3(out, 1, axis, 0.0f);
    lh_math_quat_axis_z(q, lh_addr_of(axis));
    lh_math_mat4_set_column_vec3(out, 2, axis, 0.0f);
}

lh_void
lh_math_mat4_mul_vec4(lh_math_mat4_t m, lh_math_vec4_t v, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t column;

    /* A sum of the columns, weighted by v's components. */
    lh_math_mat4_get_column(lh_addr_of(m), 0, lh_addr_of(column));
    lh_math_vec4_scale(column, lh_math_vec4_get_x(lh_addr_of(v)), out);
    lh_math_mat4_get_column(lh_addr_of(m), 1, lh_addr_of(column));
    lh_math_vec4_madd(lh_ptr_deref(out), column, lh_math_vec4_get_y(lh_addr_of(v)), out);
    lh_math_mat4_get_column(lh_addr_of(m), 2, lh_addr_of(column));
    lh_math_vec4_madd(lh_ptr_deref(out), column, lh_math_vec4_get_z(lh_addr_of(v)), out);
    lh_math_mat4_get_column(lh_addr_of(m), 3, lh_addr_of(column));
    lh_math_vec4_madd(lh_ptr_deref(out), column, lh_math_vec4_get_w(lh_addr_of(v)), out);
}

lh_void
lh_math_mat4_mul(lh_math_mat4_t a, lh_math_mat4_t b, lh_math_mat4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t column;

    /* Column c of the product is a applied to column c of b. a and b are
     * copies, so writing each column straight into out is safe even when out
     * is where a or b came from. */
    for (unsigned c = 0; c < 4; ++c)
    {
        lh_math_mat4_get_column(lh_addr_of(b), c, lh_addr_of(column));
        lh_math_mat4_mul_vec4(a, column, lh_addr_of(column));
        lh_math_mat4_set_column(out, c, column);
    }
}

lh_void
lh_math_mat4_transform_point(lh_math_mat4_t m, lh_math_vec3_t p, lh_math_vec3_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t p4;
    lh_math_vec4_t r;

    lh_math_vec3_to_vec4(p, 1.0f, lh_addr_of(p4));
    lh_math_mat4_mul_vec4(m, p4, lh_addr_of(r));
    lh_math_vec4_to_vec3(r, out);
}

lh_void
lh_math_mat4_transform_dir(lh_math_mat4_t m, lh_math_vec3_t d, lh_math_vec3_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t d4;
    lh_math_vec4_t r;

    lh_math_vec3_to_vec4(d, 0.0f, lh_addr_of(d4));
    lh_math_mat4_mul_vec4(m, d4, lh_addr_of(r));
    lh_math_vec4_to_vec3(r, out);
}

lh_void
lh_math_mat4_transpose(lh_math_mat4_t m, lh_math_mat4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t c0;
    lh_math_mat4_get_column(lh_addr_of(m), 0, lh_addr_of(c0));
    lh_math_vec4_t c1;
    lh_math_mat4_get_column(lh_addr_of(m), 1, lh_addr_of(c1));
    lh_math_vec4_t c2;
    lh_math_mat4_get_column(lh_addr_of(m), 2, lh_addr_of(c2));
    lh_math_vec4_t c3;
    lh_math_mat4_get_column(lh_addr_of(m), 3, lh_addr_of(c3));
    lh_math_vec4_t column0;
    lh_math_vec4_init(lh_addr_of(column0), lh_math_vec4_get_x(lh_addr_of(c0)),
                      lh_math_vec4_get_x(lh_addr_of(c1)), lh_math_vec4_get_x(lh_addr_of(c2)),
                      lh_math_vec4_get_x(lh_addr_of(c3)));
    lh_math_vec4_t column1;
    lh_math_vec4_init(lh_addr_of(column1), lh_math_vec4_get_y(lh_addr_of(c0)),
                      lh_math_vec4_get_y(lh_addr_of(c1)), lh_math_vec4_get_y(lh_addr_of(c2)),
                      lh_math_vec4_get_y(lh_addr_of(c3)));
    lh_math_vec4_t column2;
    lh_math_vec4_init(lh_addr_of(column2), lh_math_vec4_get_z(lh_addr_of(c0)),
                      lh_math_vec4_get_z(lh_addr_of(c1)), lh_math_vec4_get_z(lh_addr_of(c2)),
                      lh_math_vec4_get_z(lh_addr_of(c3)));
    lh_math_vec4_t column3;
    lh_math_vec4_init(lh_addr_of(column3), lh_math_vec4_get_w(lh_addr_of(c0)),
                      lh_math_vec4_get_w(lh_addr_of(c1)), lh_math_vec4_get_w(lh_addr_of(c2)),
                      lh_math_vec4_get_w(lh_addr_of(c3)));
    lh_math_mat4_from_columns(column0, column1, column2, column3, out);
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
        lh_math_vec4_t col;
        lh_math_mat4_get_column(lh_addr_of(m), c, lh_addr_of(col));
        a[c * 4 + 0] = lh_math_vec4_get_x(lh_addr_of(col));
        a[c * 4 + 1] = lh_math_vec4_get_y(lh_addr_of(col));
        a[c * 4 + 2] = lh_math_vec4_get_z(lh_addr_of(col));
        a[c * 4 + 3] = lh_math_vec4_get_w(lh_addr_of(col));
    }

    inv[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] +
             a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
    inv[4] = lh_math_neg(a[4]) * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] -
             a[8] * a[7] * a[14] - a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
    inv[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] +
             a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
    inv[12] = lh_math_neg(a[4]) * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] -
              a[8] * a[6] * a[13] - a[12] * a[5] * a[10] + a[12] * a[6] * a[9];
    inv[1] = lh_math_neg(a[1]) * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] -
             a[9] * a[3] * a[14] - a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
    inv[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] +
             a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
    inv[9] = lh_math_neg(a[0]) * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] -
             a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
    inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] +
              a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
    inv[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] +
             a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
    inv[6] = lh_math_neg(a[0]) * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] -
             a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
    inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] +
              a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
    inv[14] = lh_math_neg(a[0]) * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] -
              a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
    inv[3] = lh_math_neg(a[1]) * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] -
             a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
    inv[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] +
             a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
    inv[11] = lh_math_neg(a[0]) * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] -
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
        lh_math_vec4_t vec4;
        lh_math_vec4_init(lh_addr_of(vec4), inv[c * 4 + 0] * inv_det, inv[c * 4 + 1] * inv_det,
                          inv[c * 4 + 2] * inv_det, inv[c * 4 + 3] * inv_det);
        lh_math_mat4_set_column(out, c, vec4);
    }
    return lh_bool_true;
}

lh_bool_t
lh_math_mat4_near(lh_math_mat4_t a, lh_math_mat4_t b, lh_float_t eps)
{
    for (unsigned c = 0; c < 4; ++c)
    {
        lh_math_vec4_t vec4;
        lh_math_mat4_get_column(lh_addr_of(b), c, lh_addr_of(vec4));
        lh_math_vec4_t vec41;
        lh_math_mat4_get_column(lh_addr_of(a), c, lh_addr_of(vec41));
        if (!lh_math_vec4_near(vec41, vec4, eps))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}