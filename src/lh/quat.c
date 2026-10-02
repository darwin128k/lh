#include <lh/quat.h>
#include <lh/float/sin_cos.h>
#include <lh/float/sqrt.h>

lh_quat_t
lh_quat_make(lh_float_t x, lh_float_t y, lh_float_t z, lh_float_t w)
{
    lh_quat_t q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

lh_quat_t
lh_quat_identity(void)
{
    return lh_quat_make(0.0f, 0.0f, 0.0f, 1.0f);
}

lh_quat_t
lh_quat_from_axis_angle(lh_vec3_t axis, lh_float_t angle)
{
    lh_float_t s;
    lh_float_t c;
    lh_float_sin_cos(angle * 0.5f, &s, &c);
    return lh_quat_make(axis.x * s, axis.y * s, axis.z * s, c);
}

lh_quat_t
lh_quat_mul(lh_quat_t a, lh_quat_t b)
{
    return lh_quat_make(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
                        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

lh_quat_t
lh_quat_conjugate(lh_quat_t q)
{
    return lh_quat_make(-q.x, -q.y, -q.z, q.w);
}

lh_float_t
lh_quat_dot(lh_quat_t a, lh_quat_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

lh_quat_t
lh_quat_normalize(lh_quat_t q)
{
    const lh_float_t length = lh_float_sqrt(lh_quat_dot(q, q));
    if (length == 0.0f)
    {
        return q;
    }
    const lh_float_t inv = 1.0f / length;
    return lh_quat_make(q.x * inv, q.y * inv, q.z * inv, q.w * inv);
}

lh_vec3_t
lh_quat_rotate(lh_quat_t q, lh_vec3_t v)
{
    /* v + w * t + u x t, with u = (x, y, z) and t = 2 (u x v): the expanded
     * q v q* without forming the intermediate quaternions. */
    const lh_vec3_t u = lh_vec3_make(q.x, q.y, q.z);
    const lh_vec3_t t = lh_vec3_scale(lh_vec3_cross(u, v), 2.0f);
    return lh_vec3_add(lh_vec3_add(v, lh_vec3_scale(t, q.w)), lh_vec3_cross(u, t));
}

lh_quat_t
lh_quat_nlerp(lh_quat_t a, lh_quat_t b, lh_float_t t)
{
    if (lh_quat_dot(a, b) < 0.0f)
    {
        b = lh_quat_make(-b.x, -b.y, -b.z, -b.w);
    }
    return lh_quat_normalize(lh_quat_make(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
                                          a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t));
}
