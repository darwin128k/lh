#include <lh/assert/runtime.h>
#include <lh/math/quat.h>
#include <lh/math/float/sin_cos.h>
#include <lh/util/addr.h>

lh_void
lh_math_quat_init(lh_math_quat_t *self, lh_float_t x, lh_float_t y, lh_float_t z, lh_float_t w)
{
    lh_assert_runtime_ref(self);
    lh_math_quat_set_x(self, x);
    lh_math_quat_set_y(self, y);
    lh_math_quat_set_z(self, z);
    lh_math_quat_set_w(self, w);
}

lh_float_t
lh_math_quat_get_x(const lh_math_quat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_float_t
lh_math_quat_get_y(const lh_math_quat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_float_t
lh_math_quat_get_z(const lh_math_quat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->z;
}

lh_float_t
lh_math_quat_get_w(const lh_math_quat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->w;
}

lh_void
lh_math_quat_set_x(lh_math_quat_t *self, lh_float_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_math_quat_set_y(lh_math_quat_t *self, lh_float_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

lh_void
lh_math_quat_set_z(lh_math_quat_t *self, lh_float_t z)
{
    lh_assert_runtime_ref(self);
    self->z = z;
}

lh_void
lh_math_quat_set_w(lh_math_quat_t *self, lh_float_t w)
{
    lh_assert_runtime_ref(self);
    self->w = w;
}

lh_void
lh_math_quat_to_vec4(lh_math_quat_t q, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result), lh_math_quat_get_x(lh_addr_of(q)),
                      lh_math_quat_get_y(lh_addr_of(q)), lh_math_quat_get_z(lh_addr_of(q)),
                      lh_math_quat_get_w(lh_addr_of(q)));
    *out = result;
    return;
}

lh_void
lh_math_quat_from_vec4(lh_math_vec4_t v, lh_math_quat_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_quat_t result;
    lh_math_quat_init(lh_addr_of(result), lh_math_vec4_get_x(lh_addr_of(v)),
                      lh_math_vec4_get_y(lh_addr_of(v)), lh_math_vec4_get_z(lh_addr_of(v)),
                      lh_math_vec4_get_w(lh_addr_of(v)));
    *out = result;
    return;
}

lh_void
lh_math_quat_identity(lh_math_quat_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_quat_t _lh_tmp;
    lh_math_quat_init(lh_addr_of(_lh_tmp), 0.0f, 0.0f, 0.0f, 1.0f);
    *out = _lh_tmp;
    return;
}

lh_void
lh_math_quat_from_axis_angle(lh_math_vec3_t axis, lh_float_t angle, lh_math_quat_t *out)
{
    lh_assert_runtime_ref(out);
    lh_float_t s;
    lh_float_t c;
    lh_math_float_sin_cos(angle * 0.5f, lh_addr_of(s), lh_addr_of(c));
    lh_math_quat_t result;
    lh_math_quat_init(lh_addr_of(result), lh_math_vec3_get_x(lh_addr_of(axis)) * s,
                      lh_math_vec3_get_y(lh_addr_of(axis)) * s,
                      lh_math_vec3_get_z(lh_addr_of(axis)) * s, c);
    *out = result;
    return;
}

lh_void
lh_math_quat_mul(lh_math_quat_t a, lh_math_quat_t b, lh_math_quat_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_float_t ax = lh_math_quat_get_x(lh_addr_of(a));
    const lh_float_t ay = lh_math_quat_get_y(lh_addr_of(a));
    const lh_float_t az = lh_math_quat_get_z(lh_addr_of(a));
    const lh_float_t aw = lh_math_quat_get_w(lh_addr_of(a));
    const lh_float_t bx = lh_math_quat_get_x(lh_addr_of(b));
    const lh_float_t by = lh_math_quat_get_y(lh_addr_of(b));
    const lh_float_t bz = lh_math_quat_get_z(lh_addr_of(b));
    const lh_float_t bw = lh_math_quat_get_w(lh_addr_of(b));
    lh_math_quat_t _lh_tmp;
    lh_math_quat_init(lh_addr_of(_lh_tmp), aw * bx + ax * bw + ay * bz - az * by,
                             aw * by - ax * bz + ay * bw + az * bx,
                             aw * bz + ax * by - ay * bx + az * bw,
                             aw * bw - ax * bx - ay * by - az * bz);
    *out = _lh_tmp;
    return;
}

lh_void
lh_math_quat_conjugate(lh_math_quat_t q, lh_math_quat_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_quat_t result;
    lh_math_quat_init(lh_addr_of(result), -lh_math_quat_get_x(lh_addr_of(q)),
                      -lh_math_quat_get_y(lh_addr_of(q)), -lh_math_quat_get_z(lh_addr_of(q)),
                      lh_math_quat_get_w(lh_addr_of(q)));
    *out = result;
    return;
}

lh_float_t
lh_math_quat_dot(lh_math_quat_t a, lh_math_quat_t b)
{
    lh_math_vec4_t vec4;
    lh_math_quat_to_vec4(b, lh_addr_of(vec4));
    lh_math_vec4_t vec41;
    lh_math_quat_to_vec4(a, lh_addr_of(vec41));
    return lh_math_vec4_dot(vec41, vec4);
}

lh_void
lh_math_quat_normalize(lh_math_quat_t q, lh_math_quat_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t v;
    lh_math_vec4_t unit;

    lh_math_quat_to_vec4(q, lh_addr_of(v));
    lh_math_vec4_normalize(v, lh_addr_of(unit));
    lh_math_quat_from_vec4(unit, out);
}

lh_void
lh_math_quat_rotate(lh_math_quat_t q, lh_math_vec3_t v, lh_math_vec3_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec3_t vec3;
    lh_math_vec3_init(lh_addr_of(vec3), lh_math_quat_get_x(lh_addr_of(q)),
                      lh_math_quat_get_y(lh_addr_of(q)), lh_math_quat_get_z(lh_addr_of(q)));
    /* v + w * t + u x t, with u = (x, y, z) and t = 2 (u x v): the expanded
     * q v q* without forming the intermediate quaternions. */
    const lh_math_vec3_t u = vec3;
    lh_math_vec3_t uv;
    lh_math_vec3_t t;
    lh_math_vec3_t wt;
    lh_math_vec3_t ut;
    lh_math_vec3_t sum;

    lh_math_vec3_cross(u, v, lh_addr_of(uv));
    lh_math_vec3_scale(uv, 2.0f, lh_addr_of(t));
    lh_math_vec3_scale(t, lh_math_quat_get_w(lh_addr_of(q)), lh_addr_of(wt));
    lh_math_vec3_cross(u, t, lh_addr_of(ut));
    lh_math_vec3_add(v, wt, lh_addr_of(sum));
    lh_math_vec3_add(sum, ut, out);
}

lh_void
lh_math_quat_nlerp(lh_math_quat_t a, lh_math_quat_t b, lh_float_t t, lh_math_quat_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t to;
    lh_math_quat_to_vec4(b, lh_addr_of(to));
    if (lh_math_quat_dot(a, b) < 0.0f)
    {
        lh_math_vec4_neg(to, lh_addr_of(to));
    }
    lh_math_vec4_t from;
    lh_math_vec4_t mixed;
    lh_math_vec4_t unit;

    lh_math_quat_to_vec4(a, lh_addr_of(from));
    lh_math_vec4_lerp(from, to, t, lh_addr_of(mixed));
    lh_math_vec4_normalize(mixed, lh_addr_of(unit));
    lh_math_quat_from_vec4(unit, out);
}