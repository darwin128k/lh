#include <lh/math/quat.h>
#include <lh/float/sin_cos.h>

lh_math_quat_t
lh_math_quat_make(lh_float_t x, lh_float_t y, lh_float_t z, lh_float_t w)
{
    lh_math_quat_t q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

lh_math_vec4_t
lh_math_quat_to_vec4(lh_math_quat_t q)
{
    return lh_math_vec4_make(q.x, q.y, q.z, q.w);
}

lh_math_quat_t
lh_math_quat_from_vec4(lh_math_vec4_t v)
{
    return lh_math_quat_make(v.x, v.y, v.z, v.w);
}

lh_math_quat_t
lh_math_quat_identity(void)
{
    return lh_math_quat_make(0.0f, 0.0f, 0.0f, 1.0f);
}

lh_math_quat_t
lh_math_quat_from_axis_angle(lh_math_vec3_t axis, lh_float_t angle)
{
    lh_float_t s;
    lh_float_t c;
    lh_float_sin_cos(angle * 0.5f, &s, &c);
    return lh_math_quat_make(axis.x * s, axis.y * s, axis.z * s, c);
}

lh_math_quat_t
lh_math_quat_mul(lh_math_quat_t a, lh_math_quat_t b)
{
    return lh_math_quat_make(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
                        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

lh_math_quat_t
lh_math_quat_conjugate(lh_math_quat_t q)
{
    return lh_math_quat_make(-q.x, -q.y, -q.z, q.w);
}

lh_float_t
lh_math_quat_dot(lh_math_quat_t a, lh_math_quat_t b)
{
    return lh_math_vec4_dot(lh_math_quat_to_vec4(a), lh_math_quat_to_vec4(b));
}

lh_math_quat_t
lh_math_quat_normalize(lh_math_quat_t q)
{
    return lh_math_quat_from_vec4(lh_math_vec4_normalize(lh_math_quat_to_vec4(q)));
}

lh_math_vec3_t
lh_math_quat_rotate(lh_math_quat_t q, lh_math_vec3_t v)
{
    /* v + w * t + u x t, with u = (x, y, z) and t = 2 (u x v): the expanded
     * q v q* without forming the intermediate quaternions. */
    const lh_math_vec3_t u = lh_math_vec3_make(q.x, q.y, q.z);
    const lh_math_vec3_t t = lh_math_vec3_scale(lh_math_vec3_cross(u, v), 2.0f);
    return lh_math_vec3_add(lh_math_vec3_add(v, lh_math_vec3_scale(t, q.w)), lh_math_vec3_cross(u, t));
}

lh_math_quat_t
lh_math_quat_nlerp(lh_math_quat_t a, lh_math_quat_t b, lh_float_t t)
{
    lh_math_vec4_t to = lh_math_quat_to_vec4(b);
    if (lh_math_quat_dot(a, b) < 0.0f)
    {
        to = lh_math_vec4_neg(to);
    }
    return lh_math_quat_from_vec4(lh_math_vec4_normalize(lh_math_vec4_lerp(lh_math_quat_to_vec4(a), to, t)));
}
