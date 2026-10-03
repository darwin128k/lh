#include <lh/assert/runtime.h>
#include <lh/math/vec3.h>
#include <lh/cast/static.h>
#include <lh/float/sqrt.h>

lh_math_vec3_t
lh_math_vec3_make(lh_float_t x, lh_float_t y, lh_float_t z)
{
    lh_math_vec3_t v;
    lh_math_vec3_set_x(lh_addr_of(v), x);
    lh_math_vec3_set_y(lh_addr_of(v), y);
    lh_math_vec3_set_z(lh_addr_of(v), z);
    return v;
}

lh_float_t
lh_math_vec3_get_x(const lh_math_vec3_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_float_t
lh_math_vec3_get_y(const lh_math_vec3_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_float_t
lh_math_vec3_get_z(const lh_math_vec3_t *self)
{
    lh_assert_runtime_ref(self);
    return self->z;
}

lh_void
lh_math_vec3_set_x(lh_math_vec3_t *self, lh_float_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_math_vec3_set_y(lh_math_vec3_t *self, lh_float_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

lh_void
lh_math_vec3_set_z(lh_math_vec3_t *self, lh_float_t z)
{
    lh_assert_runtime_ref(self);
    self->z = z;
}

lh_math_vec3_t
lh_math_vec3_add(lh_math_vec3_t a, lh_math_vec3_t b)
{
    return lh_math_vec3_make(lh_math_vec3_get_x(lh_addr_of(a)) + lh_math_vec3_get_x(lh_addr_of(b)),
                             lh_math_vec3_get_y(lh_addr_of(a)) + lh_math_vec3_get_y(lh_addr_of(b)),
                             lh_math_vec3_get_z(lh_addr_of(a)) + lh_math_vec3_get_z(lh_addr_of(b)));
}

lh_math_vec3_t
lh_math_vec3_sub(lh_math_vec3_t a, lh_math_vec3_t b)
{
    return lh_math_vec3_make(lh_math_vec3_get_x(lh_addr_of(a)) - lh_math_vec3_get_x(lh_addr_of(b)),
                             lh_math_vec3_get_y(lh_addr_of(a)) - lh_math_vec3_get_y(lh_addr_of(b)),
                             lh_math_vec3_get_z(lh_addr_of(a)) - lh_math_vec3_get_z(lh_addr_of(b)));
}

lh_math_vec3_t
lh_math_vec3_scale(lh_math_vec3_t v, lh_float_t s)
{
    return lh_math_vec3_make(lh_math_vec3_get_x(lh_addr_of(v)) * s,
                             lh_math_vec3_get_y(lh_addr_of(v)) * s,
                             lh_math_vec3_get_z(lh_addr_of(v)) * s);
}

lh_math_vec3_t
lh_math_vec3_neg(lh_math_vec3_t v)
{
    return lh_math_vec3_make(-lh_math_vec3_get_x(lh_addr_of(v)),
                             -lh_math_vec3_get_y(lh_addr_of(v)),
                             -lh_math_vec3_get_z(lh_addr_of(v)));
}

lh_float_t
lh_math_vec3_dot(lh_math_vec3_t a, lh_math_vec3_t b)
{
    return lh_math_vec3_get_x(lh_addr_of(a)) * lh_math_vec3_get_x(lh_addr_of(b))
         + lh_math_vec3_get_y(lh_addr_of(a)) * lh_math_vec3_get_y(lh_addr_of(b))
         + lh_math_vec3_get_z(lh_addr_of(a)) * lh_math_vec3_get_z(lh_addr_of(b));
}

lh_float_t
lh_math_vec3_length_sq(lh_math_vec3_t v)
{
    return lh_math_vec3_dot(v, v);
}

lh_float_t
lh_math_vec3_length(lh_math_vec3_t v)
{
    return lh_float_sqrt(lh_math_vec3_length_sq(v));
}

lh_math_vec3_t
lh_math_vec3_normalize(lh_math_vec3_t v)
{
    const lh_float_t length = lh_math_vec3_length(v);
    if (length == 0.0f)
    {
        return v;
    }
    return lh_math_vec3_scale(v, 1.0f / length);
}

lh_math_vec3_t
lh_math_vec3_lerp(lh_math_vec3_t a, lh_math_vec3_t b, lh_float_t t)
{
    return lh_math_vec3_make(lh_math_vec3_get_x(lh_addr_of(a))
                                 + (lh_math_vec3_get_x(lh_addr_of(b)) - lh_math_vec3_get_x(lh_addr_of(a))) * t,
                             lh_math_vec3_get_y(lh_addr_of(a))
                                 + (lh_math_vec3_get_y(lh_addr_of(b)) - lh_math_vec3_get_y(lh_addr_of(a))) * t,
                             lh_math_vec3_get_z(lh_addr_of(a))
                                 + (lh_math_vec3_get_z(lh_addr_of(b)) - lh_math_vec3_get_z(lh_addr_of(a))) * t);
}

lh_bool_t
lh_math_vec3_near(lh_math_vec3_t a, lh_math_vec3_t b, lh_float_t eps)
{
    const lh_math_vec3_t d = lh_math_vec3_sub(a, b);
    const lh_float_t dx = lh_math_vec3_get_x(lh_addr_of(d));
    const lh_float_t dy = lh_math_vec3_get_y(lh_addr_of(d));
    const lh_float_t dz = lh_math_vec3_get_z(lh_addr_of(d));
    return lh_cast_static(lh_bool_t, (dx <= eps && -dx <= eps)
                                   && (dy <= eps && -dy <= eps)
                                   && (dz <= eps && -dz <= eps));
}

lh_math_vec3_t
lh_math_vec3_cross(lh_math_vec3_t a, lh_math_vec3_t b)
{
    return lh_math_vec3_make(lh_math_vec3_get_y(lh_addr_of(a)) * lh_math_vec3_get_z(lh_addr_of(b))
                                 - lh_math_vec3_get_z(lh_addr_of(a)) * lh_math_vec3_get_y(lh_addr_of(b)),
                             lh_math_vec3_get_z(lh_addr_of(a)) * lh_math_vec3_get_x(lh_addr_of(b))
                                 - lh_math_vec3_get_x(lh_addr_of(a)) * lh_math_vec3_get_z(lh_addr_of(b)),
                             lh_math_vec3_get_x(lh_addr_of(a)) * lh_math_vec3_get_y(lh_addr_of(b))
                                 - lh_math_vec3_get_y(lh_addr_of(a)) * lh_math_vec3_get_x(lh_addr_of(b)));
}