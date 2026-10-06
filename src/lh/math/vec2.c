#include <lh/assert/runtime.h>
#include <lh/math/vec2.h>
#include <lh/cast/static.h>
#include <lh/float/sqrt.h>

lh_void
lh_math_vec2_init(lh_math_vec2_t *self, lh_float_t x, lh_float_t y)
{
    lh_assert_runtime_ref(self);
    lh_math_vec2_set_x(self, x);
    lh_math_vec2_set_y(self, y);
}

lh_void
lh_math_vec2_init_empty(lh_math_vec2_t *self)
{
    lh_math_vec2_init(self, 0, 0);
}

lh_float_t
lh_math_vec2_get_x(const lh_math_vec2_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_float_t
lh_math_vec2_get_y(const lh_math_vec2_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_void
lh_math_vec2_set_x(lh_math_vec2_t *self, lh_float_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_math_vec2_set_y(lh_math_vec2_t *self, lh_float_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

lh_math_vec2_t
lh_math_vec2_add(lh_math_vec2_t a, lh_math_vec2_t b)
{
    return ({ lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), lh_math_vec2_get_x(lh_addr_of(a)) + lh_math_vec2_get_x(lh_addr_of(b)),
                             lh_math_vec2_get_y(lh_addr_of(a)) + lh_math_vec2_get_y(lh_addr_of(b))); _v; });
}

lh_math_vec2_t
lh_math_vec2_sub(lh_math_vec2_t a, lh_math_vec2_t b)
{
    return ({ lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), lh_math_vec2_get_x(lh_addr_of(a)) - lh_math_vec2_get_x(lh_addr_of(b)),
                             lh_math_vec2_get_y(lh_addr_of(a)) - lh_math_vec2_get_y(lh_addr_of(b))); _v; });
}

lh_math_vec2_t
lh_math_vec2_scale(lh_math_vec2_t v, lh_float_t s)
{
    return ({ lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), lh_math_vec2_get_x(lh_addr_of(v)) * s, lh_math_vec2_get_y(lh_addr_of(v)) * s); _v; });
}

lh_math_vec2_t
lh_math_vec2_neg(lh_math_vec2_t v)
{
    return ({ lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), -lh_math_vec2_get_x(lh_addr_of(v)), -lh_math_vec2_get_y(lh_addr_of(v))); _v; });
}

lh_float_t
lh_math_vec2_dot(lh_math_vec2_t a, lh_math_vec2_t b)
{
    return lh_math_vec2_get_x(lh_addr_of(a)) * lh_math_vec2_get_x(lh_addr_of(b))
         + lh_math_vec2_get_y(lh_addr_of(a)) * lh_math_vec2_get_y(lh_addr_of(b));
}

lh_float_t
lh_math_vec2_length_sq(lh_math_vec2_t v)
{
    return lh_math_vec2_dot(v, v);
}

lh_float_t
lh_math_vec2_length(lh_math_vec2_t v)
{
    return lh_float_sqrt(lh_math_vec2_length_sq(v));
}

lh_math_vec2_t
lh_math_vec2_normalize(lh_math_vec2_t v)
{
    const lh_float_t length = lh_math_vec2_length(v);
    if (length == 0.0f)
    {
        return v;
    }
    return lh_math_vec2_scale(v, 1.0f / length);
}

lh_math_vec2_t
lh_math_vec2_lerp(lh_math_vec2_t a, lh_math_vec2_t b, lh_float_t t)
{
    return ({ lh_math_vec2_t _v; lh_math_vec2_init(lh_addr_of(_v), lh_math_vec2_get_x(lh_addr_of(a))
                                 + (lh_math_vec2_get_x(lh_addr_of(b)) - lh_math_vec2_get_x(lh_addr_of(a))) * t,
                             lh_math_vec2_get_y(lh_addr_of(a))
                                 + (lh_math_vec2_get_y(lh_addr_of(b)) - lh_math_vec2_get_y(lh_addr_of(a))) * t); _v; });
}

lh_bool_t
lh_math_vec2_near(lh_math_vec2_t a, lh_math_vec2_t b, lh_float_t eps)
{
    const lh_math_vec2_t d = lh_math_vec2_sub(a, b);
    const lh_float_t dx = lh_math_vec2_get_x(lh_addr_of(d));
    const lh_float_t dy = lh_math_vec2_get_y(lh_addr_of(d));
    return lh_cast_static(lh_bool_t, (dx <= eps && -dx <= eps) && (dy <= eps && -dy <= eps));
}

lh_bool_t
lh_math_vec2_equals(lh_math_vec2_t self, lh_math_vec2_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_math_vec2_get_x(lh_addr_of(self)),
                                               lh_math_vec2_get_x(lh_addr_of(other))) &&
                                         lh_math_eq(lh_math_vec2_get_y(lh_addr_of(self)),
                                                    lh_math_vec2_get_y(lh_addr_of(other))));
}