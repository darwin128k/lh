#include <lh/mat4.h>

lh_mat4_t
lh_mat4_from_columns(lh_vec4_t c0, lh_vec4_t c1, lh_vec4_t c2, lh_vec4_t c3)
{
    lh_mat4_t m;
    m.columns[0] = c0;
    m.columns[1] = c1;
    m.columns[2] = c2;
    m.columns[3] = c3;
    return m;
}

lh_mat4_t
lh_mat4_identity(void)
{
    return lh_mat4_from_scale(lh_vec3_make(1.0f, 1.0f, 1.0f));
}

lh_mat4_t
lh_mat4_from_translation(lh_vec3_t offset)
{
    lh_mat4_t m = lh_mat4_identity();
    m.columns[3] = lh_vec4_make(offset.x, offset.y, offset.z, 1.0f);
    return m;
}

lh_mat4_t
lh_mat4_from_scale(lh_vec3_t factors)
{
    return lh_mat4_from_columns(
        lh_vec4_make(factors.x, 0.0f, 0.0f, 0.0f), lh_vec4_make(0.0f, factors.y, 0.0f, 0.0f),
        lh_vec4_make(0.0f, 0.0f, factors.z, 0.0f), lh_vec4_make(0.0f, 0.0f, 0.0f, 1.0f));
}

lh_mat4_t
lh_mat4_from_quat(lh_quat_t q)
{
    const lh_float_t xx = q.x * q.x;
    const lh_float_t yy = q.y * q.y;
    const lh_float_t zz = q.z * q.z;
    const lh_float_t xy = q.x * q.y;
    const lh_float_t xz = q.x * q.z;
    const lh_float_t yz = q.y * q.z;
    const lh_float_t wx = q.w * q.x;
    const lh_float_t wy = q.w * q.y;
    const lh_float_t wz = q.w * q.z;

    /* Column c is where the rotation takes axis c. */
    return lh_mat4_from_columns(
        lh_vec4_make(1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f),
        lh_vec4_make(2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f),
        lh_vec4_make(2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f),
        lh_vec4_make(0.0f, 0.0f, 0.0f, 1.0f));
}

lh_vec4_t
lh_mat4_mul_vec4(lh_mat4_t m, lh_vec4_t v)
{
    /* A sum of the columns, weighted by v's components. */
    const lh_vec4_t xy =
        lh_vec4_add(lh_vec4_scale(m.columns[0], v.x), lh_vec4_scale(m.columns[1], v.y));
    const lh_vec4_t zw =
        lh_vec4_add(lh_vec4_scale(m.columns[2], v.z), lh_vec4_scale(m.columns[3], v.w));
    return lh_vec4_add(xy, zw);
}

lh_mat4_t
lh_mat4_mul(lh_mat4_t a, lh_mat4_t b)
{
    /* Column c of the product is a applied to column c of b. */
    return lh_mat4_from_columns(
        lh_mat4_mul_vec4(a, b.columns[0]), lh_mat4_mul_vec4(a, b.columns[1]),
        lh_mat4_mul_vec4(a, b.columns[2]), lh_mat4_mul_vec4(a, b.columns[3]));
}

lh_vec3_t
lh_mat4_transform_point(lh_mat4_t m, lh_vec3_t p)
{
    const lh_vec4_t r = lh_mat4_mul_vec4(m, lh_vec4_make(p.x, p.y, p.z, 1.0f));
    return lh_vec3_make(r.x, r.y, r.z);
}

lh_vec3_t
lh_mat4_transform_dir(lh_mat4_t m, lh_vec3_t d)
{
    const lh_vec4_t r = lh_mat4_mul_vec4(m, lh_vec4_make(d.x, d.y, d.z, 0.0f));
    return lh_vec3_make(r.x, r.y, r.z);
}

lh_mat4_t
lh_mat4_transpose(lh_mat4_t m)
{
    const lh_vec4_t *c = m.columns;
    return lh_mat4_from_columns(
        lh_vec4_make(c[0].x, c[1].x, c[2].x, c[3].x), lh_vec4_make(c[0].y, c[1].y, c[2].y, c[3].y),
        lh_vec4_make(c[0].z, c[1].z, c[2].z, c[3].z), lh_vec4_make(c[0].w, c[1].w, c[2].w, c[3].w));
}

lh_bool_t
lh_mat4_inverse(lh_mat4_t m, lh_mat4_t *out)
{
    /* Cofactors over the adjugate (as in Mesa's gluInvertMatrix). Written
     * for one storage order, it holds for the other too: the inverse of the
     * transpose is the transpose of the inverse. */
    lh_float_t a[16]; /* column * 4 + row */
    lh_float_t inv[16];
    for (lh_int_t c = 0; c < 4; ++c)
    {
        a[c * 4 + 0] = m.columns[c].x;
        a[c * 4 + 1] = m.columns[c].y;
        a[c * 4 + 2] = m.columns[c].z;
        a[c * 4 + 3] = m.columns[c].w;
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
    for (lh_int_t c = 0; c < 4; ++c)
    {
        out->columns[c] = lh_vec4_make(inv[c * 4 + 0] * inv_det, inv[c * 4 + 1] * inv_det,
                                       inv[c * 4 + 2] * inv_det, inv[c * 4 + 3] * inv_det);
    }
    return lh_bool_true;
}

lh_bool_t
lh_mat4_near(lh_mat4_t a, lh_mat4_t b, lh_float_t eps)
{
    for (lh_int_t c = 0; c < 4; ++c)
    {
        if (!lh_vec4_near(a.columns[c], b.columns[c], eps))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}
