#include <lh/assert/runtime.h>
#include <lh/math/vec4.h>
#include <lh/math/view.h>
#include <lh/util/ptr.h>
#include <lh/cast/static.h>
#include <lh/util/addr.h>

lh_void
lh_math_vec4_init(lh_math_vec4_t *self, lh_float_t x, lh_float_t y, lh_float_t z, lh_float_t w)
{
    lh_assert_runtime_ref(self);
    lh_math_vec4_set_x(self, x);
    lh_math_vec4_set_y(self, y);
    lh_math_vec4_set_z(self, z);
    lh_math_vec4_set_w(self, w);
}

lh_void
lh_math_vec4_init_empty(lh_math_vec4_t *self)
{
    lh_math_vec4_init(self, 0, 0, 0, 0);
}

lh_float_t
lh_math_vec4_get_x(const lh_math_vec4_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_float_t
lh_math_vec4_get_y(const lh_math_vec4_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_float_t
lh_math_vec4_get_z(const lh_math_vec4_t *self)
{
    lh_assert_runtime_ref(self);
    return self->z;
}

lh_float_t
lh_math_vec4_get_w(const lh_math_vec4_t *self)
{
    lh_assert_runtime_ref(self);
    return self->w;
}

lh_void
lh_math_vec4_set_x(lh_math_vec4_t *self, lh_float_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_math_vec4_set_y(lh_math_vec4_t *self, lh_float_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

lh_void
lh_math_vec4_set_z(lh_math_vec4_t *self, lh_float_t z)
{
    lh_assert_runtime_ref(self);
    self->z = z;
}

lh_void
lh_math_vec4_set_w(lh_math_vec4_t *self, lh_float_t w)
{
    lh_assert_runtime_ref(self);
    self->w = w;
}

/* ── Conversions ─────────────────────────────────────────────────────────── */

lh_math_vec4_t
lh_math_vec3_to_vec4(lh_math_vec3_t v, lh_float_t w)
{
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result), lh_math_vec3_get_x(lh_addr_of(v)),
                      lh_math_vec3_get_y(lh_addr_of(v)), lh_math_vec3_get_z(lh_addr_of(v)), w);
    return result;
}

lh_math_vec3_t
lh_math_vec4_to_vec3(lh_math_vec4_t v)
{
    lh_math_vec3_t result;
    lh_math_vec3_init(lh_addr_of(result), lh_math_vec4_get_x(lh_addr_of(v)),
                      lh_math_vec4_get_y(lh_addr_of(v)), lh_math_vec4_get_z(lh_addr_of(v)));
    return result;
}

lh_math_vec4_t
lh_math_vec4_add(lh_math_vec4_t a, lh_math_vec4_t b)
{
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result),
                      lh_math_vec4_get_x(lh_addr_of(a)) + lh_math_vec4_get_x(lh_addr_of(b)),
                      lh_math_vec4_get_y(lh_addr_of(a)) + lh_math_vec4_get_y(lh_addr_of(b)),
                      lh_math_vec4_get_z(lh_addr_of(a)) + lh_math_vec4_get_z(lh_addr_of(b)),
                      lh_math_vec4_get_w(lh_addr_of(a)) + lh_math_vec4_get_w(lh_addr_of(b)));
    return result;
}

lh_math_vec4_t
lh_math_vec4_sub(lh_math_vec4_t a, lh_math_vec4_t b)
{
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result),
                      lh_math_vec4_get_x(lh_addr_of(a)) - lh_math_vec4_get_x(lh_addr_of(b)),
                      lh_math_vec4_get_y(lh_addr_of(a)) - lh_math_vec4_get_y(lh_addr_of(b)),
                      lh_math_vec4_get_z(lh_addr_of(a)) - lh_math_vec4_get_z(lh_addr_of(b)),
                      lh_math_vec4_get_w(lh_addr_of(a)) - lh_math_vec4_get_w(lh_addr_of(b)));
    return result;
}

lh_math_vec4_t
lh_math_vec4_scale(lh_math_vec4_t v, lh_float_t s)
{
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result), lh_math_vec4_get_x(lh_addr_of(v)) * s,
                      lh_math_vec4_get_y(lh_addr_of(v)) * s, lh_math_vec4_get_z(lh_addr_of(v)) * s,
                      lh_math_vec4_get_w(lh_addr_of(v)) * s);
    return result;
}

lh_math_vec4_t
lh_math_vec4_neg(lh_math_vec4_t v)
{
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result), -lh_math_vec4_get_x(lh_addr_of(v)),
                      -lh_math_vec4_get_y(lh_addr_of(v)), -lh_math_vec4_get_z(lh_addr_of(v)),
                      -lh_math_vec4_get_w(lh_addr_of(v)));
    return result;
}

/** The 4 components of @p v as a view: a vector is laid out as `float[4]`. */
static lh_math_view_t
lh_math_vec4_view_of(const lh_math_vec4_t *v)
{
    lh_math_slice_t slice;
    lh_math_view_t view;

    lh_math_slice_init(lh_addr_of(slice), lh_ptr_rcast(const lh_float_t, v), 4U);
    lh_math_view_init_by_slice(lh_addr_of(view), lh_addr_of(slice));
    return view;
}

lh_float_t
lh_math_vec4_dot(lh_math_vec4_t a, lh_math_vec4_t b)
{
    const lh_math_view_t va = lh_math_vec4_view_of(lh_addr_of(a));
    const lh_math_view_t vb = lh_math_vec4_view_of(lh_addr_of(b));

    return lh_math_view_dot(lh_addr_of(va), lh_addr_of(vb));
}

lh_float_t
lh_math_vec4_length_sq(lh_math_vec4_t v)
{
    const lh_math_view_t view = lh_math_vec4_view_of(lh_addr_of(v));

    return lh_math_view_length_sq(lh_addr_of(view));
}

lh_float_t
lh_math_vec4_length(lh_math_vec4_t v)
{
    const lh_math_view_t view = lh_math_vec4_view_of(lh_addr_of(v));

    return lh_math_view_length(lh_addr_of(view));
}

lh_math_vec4_t
lh_math_vec4_normalize(lh_math_vec4_t v)
{
    const lh_float_t length = lh_math_vec4_length(v);
    if (length == 0.0f)
    {
        return v;
    }
    return lh_math_vec4_scale(v, 1.0f / length);
}

lh_math_vec4_t
lh_math_vec4_lerp(lh_math_vec4_t a, lh_math_vec4_t b, lh_float_t t)
{
    lh_math_vec4_t result;
    lh_math_vec4_init(
        lh_addr_of(result),
        lh_math_vec4_get_x(lh_addr_of(a)) +
            (lh_math_vec4_get_x(lh_addr_of(b)) - lh_math_vec4_get_x(lh_addr_of(a))) * t,
        lh_math_vec4_get_y(lh_addr_of(a)) +
            (lh_math_vec4_get_y(lh_addr_of(b)) - lh_math_vec4_get_y(lh_addr_of(a))) * t,
        lh_math_vec4_get_z(lh_addr_of(a)) +
            (lh_math_vec4_get_z(lh_addr_of(b)) - lh_math_vec4_get_z(lh_addr_of(a))) * t,
        lh_math_vec4_get_w(lh_addr_of(a)) +
            (lh_math_vec4_get_w(lh_addr_of(b)) - lh_math_vec4_get_w(lh_addr_of(a))) * t);
    return result;
}

lh_bool_t
lh_math_vec4_near(lh_math_vec4_t a, lh_math_vec4_t b, lh_float_t eps)
{
    const lh_math_vec4_t d = lh_math_vec4_sub(a, b);
    const lh_float_t dx = lh_math_vec4_get_x(lh_addr_of(d));
    const lh_float_t dy = lh_math_vec4_get_y(lh_addr_of(d));
    const lh_float_t dz = lh_math_vec4_get_z(lh_addr_of(d));
    const lh_float_t dw = lh_math_vec4_get_w(lh_addr_of(d));
    return lh_cast_static(lh_bool_t, (dx <= eps && -dx <= eps)
                                   && (dy <= eps && -dy <= eps)
                                   && (dz <= eps && -dz <= eps)
                                   && (dw <= eps && -dw <= eps));
}

lh_bool_t
lh_math_vec4_equals(lh_math_vec4_t self, lh_math_vec4_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_math_vec4_get_x(lh_addr_of(self)),
                                               lh_math_vec4_get_x(lh_addr_of(other))) &&
                                         lh_math_eq(lh_math_vec4_get_y(lh_addr_of(self)),
                                                    lh_math_vec4_get_y(lh_addr_of(other))) &&
                                         lh_math_eq(lh_math_vec4_get_z(lh_addr_of(self)),
                                                    lh_math_vec4_get_z(lh_addr_of(other))) &&
                                         lh_math_eq(lh_math_vec4_get_w(lh_addr_of(self)),
                                                    lh_math_vec4_get_w(lh_addr_of(other))));
}