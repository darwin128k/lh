#include <lh/assert/runtime.h>
#include <lh/math/quat.h>
#include <lh/float/sin_cos.h>
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

lh_math_vec4_t
lh_math_quat_to_vec4(lh_math_quat_t q)
{
    return ({ lh_math_vec4_t _v; lh_math_vec4_init(lh_addr_of(_v), lh_math_quat_get_x(lh_addr_of(q)),
                             lh_math_quat_get_y(lh_addr_of(q)),
                             lh_math_quat_get_z(lh_addr_of(q)),
                             lh_math_quat_get_w(lh_addr_of(q))); _v; });
}

lh_math_quat_t
lh_math_quat_from_vec4(lh_math_vec4_t v)
{
    return ({ lh_math_quat_t _v; lh_math_quat_init(lh_addr_of(_v), lh_math_vec4_get_x(lh_addr_of(v)),
                             lh_math_vec4_get_y(lh_addr_of(v)),
                             lh_math_vec4_get_z(lh_addr_of(v)),
                             lh_math_vec4_get_w(lh_addr_of(v))); _v; });
}

lh_math_quat_t
lh_math_quat_identity(void)
{
    lh_math_quat_t _lh_tmp;
    lh_math_quat_init(lh_addr_of(_lh_tmp), 0.0f, 0.0f, 0.0f, 1.0f);
    return _lh_tmp;
}

lh_math_quat_t
lh_math_quat_from_axis_angle(lh_math_vec3_t axis, lh_float_t angle)
{
    lh_float_t s;
    lh_float_t c;
    lh_float_sin_cos(angle * 0.5f, lh_addr_of(s), lh_addr_of(c));
    return ({ lh_math_quat_t _v; lh_math_quat_init(lh_addr_of(_v), lh_math_vec3_get_x(lh_addr_of(axis)) * s,
                             lh_math_vec3_get_y(lh_addr_of(axis)) * s,
                             lh_math_vec3_get_z(lh_addr_of(axis)) * s,
                             c); _v; });
}

lh_math_quat_t
lh_math_quat_mul(lh_math_quat_t a, lh_math_quat_t b)
{
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
    return _lh_tmp;
}

lh_math_quat_t
lh_math_quat_conjugate(lh_math_quat_t q)
{
    return ({ lh_math_quat_t _v; lh_math_quat_init(lh_addr_of(_v), -lh_math_quat_get_x(lh_addr_of(q)),
                             -lh_math_quat_get_y(lh_addr_of(q)),
                             -lh_math_quat_get_z(lh_addr_of(q)),
                             lh_math_quat_get_w(lh_addr_of(q))); _v; });
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
    const lh_math_vec3_t u = ({ lh_math_vec3_t _v; lh_math_vec3_init(lh_addr_of(_v), lh_math_quat_get_x(lh_addr_of(q)),
                                               lh_math_quat_get_y(lh_addr_of(q)),
                                               lh_math_quat_get_z(lh_addr_of(q))); _v; });
    const lh_math_vec3_t t = lh_math_vec3_scale(lh_math_vec3_cross(u, v), 2.0f);
    return lh_math_vec3_add(lh_math_vec3_add(v, lh_math_vec3_scale(t, lh_math_quat_get_w(lh_addr_of(q)))),
                           lh_math_vec3_cross(u, t));
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