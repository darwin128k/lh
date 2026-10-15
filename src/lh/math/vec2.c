#include <lh/assert/runtime.h>
#include <lh/math/vec2.h>
#include <lh/math/view.h>
#include <lh/util/ptr.h>
#include <lh/util/addr.h>
#include <lh/cast/static.h>

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
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result),
                      lh_math_vec2_get_x(lh_addr_of(a)) + lh_math_vec2_get_x(lh_addr_of(b)),
                      lh_math_vec2_get_y(lh_addr_of(a)) + lh_math_vec2_get_y(lh_addr_of(b)));
    return result;
}

lh_math_vec2_t
lh_math_vec2_sub(lh_math_vec2_t a, lh_math_vec2_t b)
{
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result),
                      lh_math_vec2_get_x(lh_addr_of(a)) - lh_math_vec2_get_x(lh_addr_of(b)),
                      lh_math_vec2_get_y(lh_addr_of(a)) - lh_math_vec2_get_y(lh_addr_of(b)));
    return result;
}

lh_math_vec2_t
lh_math_vec2_scale(lh_math_vec2_t v, lh_float_t s)
{
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result), lh_math_vec2_get_x(lh_addr_of(v)) * s,
                      lh_math_vec2_get_y(lh_addr_of(v)) * s);
    return result;
}

lh_math_vec2_t
lh_math_vec2_neg(lh_math_vec2_t v)
{
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result), -lh_math_vec2_get_x(lh_addr_of(v)),
                      -lh_math_vec2_get_y(lh_addr_of(v)));
    return result;
}

/** The 2 components of @p v as a view: a vector is laid out as `float[2]`. */
static lh_math_view_t
lh_math_vec2_view_of(const lh_math_vec2_t *v)
{
    lh_math_slice_t slice;
    lh_math_view_t view;

    lh_math_slice_init(lh_addr_of(slice), lh_ptr_rcast(const lh_float_t, v), 2U);
    lh_math_view_init_by_slice(lh_addr_of(view), lh_addr_of(slice));
    return view;
}

lh_float_t
lh_math_vec2_dot(lh_math_vec2_t a, lh_math_vec2_t b)
{
    const lh_math_view_t va = lh_math_vec2_view_of(lh_addr_of(a));
    const lh_math_view_t vb = lh_math_vec2_view_of(lh_addr_of(b));

    return lh_math_view_dot(lh_addr_of(va), lh_addr_of(vb));
}

lh_float_t
lh_math_vec2_length_sq(lh_math_vec2_t v)
{
    const lh_math_view_t view = lh_math_vec2_view_of(lh_addr_of(v));

    return lh_math_view_length_sq(lh_addr_of(view));
}

lh_float_t
lh_math_vec2_length(lh_math_vec2_t v)
{
    const lh_math_view_t view = lh_math_vec2_view_of(lh_addr_of(v));

    return lh_math_view_length(lh_addr_of(view));
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
    lh_math_vec2_t result;
    lh_math_vec2_init(
        lh_addr_of(result),
        lh_math_vec2_get_x(lh_addr_of(a)) +
            (lh_math_vec2_get_x(lh_addr_of(b)) - lh_math_vec2_get_x(lh_addr_of(a))) * t,
        lh_math_vec2_get_y(lh_addr_of(a)) +
            (lh_math_vec2_get_y(lh_addr_of(b)) - lh_math_vec2_get_y(lh_addr_of(a))) * t);
    return result;
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