#include <lh/assert/runtime.h>
#include <lh/assert/static.h>
#include <lh/math/vec2.h>
#include <lh/util/offset.h>

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

lh_void
lh_math_vec2_add(lh_math_vec2_t a, lh_math_vec2_t b, lh_math_vec2_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result),
                      lh_math_vec2_get_x(lh_addr_of(a)) + lh_math_vec2_get_x(lh_addr_of(b)),
                      lh_math_vec2_get_y(lh_addr_of(a)) + lh_math_vec2_get_y(lh_addr_of(b)));
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec2_sub(lh_math_vec2_t a, lh_math_vec2_t b, lh_math_vec2_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result),
                      lh_math_vec2_get_x(lh_addr_of(a)) - lh_math_vec2_get_x(lh_addr_of(b)),
                      lh_math_vec2_get_y(lh_addr_of(a)) - lh_math_vec2_get_y(lh_addr_of(b)));
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec2_scale(lh_math_vec2_t v, lh_float_t s, lh_math_vec2_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result), lh_math_vec2_get_x(lh_addr_of(v)) * s,
                      lh_math_vec2_get_y(lh_addr_of(v)) * s);
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec2_neg(lh_math_vec2_t v, lh_math_vec2_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec2_t result;
    lh_math_vec2_init(lh_addr_of(result), lh_math_neg(lh_math_vec2_get_x(lh_addr_of(v))),
                      lh_math_neg(lh_math_vec2_get_y(lh_addr_of(v))));
    lh_ptr_deref(out) = result;
}

/* lh_math_vec2_view_of reads the vector as lh_float_t[2]: the components must
   follow each other with no padding, from the first byte. */
lh_assert_static(sizeof(lh_math_vec2_t) == 2U * sizeof(lh_float_t),
                 "lh_math_vec2_t must be laid out as lh_float_t[2]");
lh_assert_static(lh_offset_of(lh_math_vec2_t, x) == 0U, "lh_math_vec2_t must start with x");

lh_void
lh_math_vec2_view_of(const lh_math_vec2_t *v, lh_math_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_slice_t slice;
    lh_math_view_t view;

    lh_math_slice_init(lh_addr_of(slice), lh_ptr_rcast(const lh_float_t, v), 2U);
    lh_math_view_init_by_slice(lh_addr_of(view), lh_addr_of(slice));
    lh_ptr_deref(out) = view;
}

lh_float_t
lh_math_vec2_dot(lh_math_vec2_t a, lh_math_vec2_t b)
{
    lh_math_view_t va;
    lh_math_vec2_view_of(lh_addr_of(a), lh_addr_of(va));
    lh_math_view_t vb;
    lh_math_vec2_view_of(lh_addr_of(b), lh_addr_of(vb));

    return lh_math_view_dot(lh_addr_of(va), lh_addr_of(vb));
}

lh_float_t
lh_math_vec2_length_sq(lh_math_vec2_t v)
{
    lh_math_view_t view;
    lh_math_vec2_view_of(lh_addr_of(v), lh_addr_of(view));

    return lh_math_view_length_sq(lh_addr_of(view));
}

lh_float_t
lh_math_vec2_length(lh_math_vec2_t v)
{
    lh_math_view_t view;
    lh_math_vec2_view_of(lh_addr_of(v), lh_addr_of(view));

    return lh_math_view_length(lh_addr_of(view));
}

lh_void
lh_math_vec2_normalize(lh_math_vec2_t v, lh_math_vec2_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_float_t length = lh_math_vec2_length(v);
    if (length == 0.0f)
    {
        lh_ptr_deref(out) = v;
        return;
    }
    lh_math_vec2_scale(v, 1.0f / length, out);
}

lh_void
lh_math_vec2_lerp(lh_math_vec2_t a, lh_math_vec2_t b, lh_float_t t, lh_math_vec2_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec2_t result;
    lh_math_vec2_init(
        lh_addr_of(result),
        lh_math_vec2_get_x(lh_addr_of(a)) +
            (lh_math_vec2_get_x(lh_addr_of(b)) - lh_math_vec2_get_x(lh_addr_of(a))) * t,
        lh_math_vec2_get_y(lh_addr_of(a)) +
            (lh_math_vec2_get_y(lh_addr_of(b)) - lh_math_vec2_get_y(lh_addr_of(a))) * t);
    lh_ptr_deref(out) = result;
}

lh_bool_t
lh_math_vec2_near(lh_math_vec2_t a, lh_math_vec2_t b, lh_float_t eps)
{
    lh_math_vec2_t d;
    lh_math_vec2_sub(a, b, lh_addr_of(d));
    const lh_float_t dx = lh_math_vec2_get_x(lh_addr_of(d));
    const lh_float_t dy = lh_math_vec2_get_y(lh_addr_of(d));
    return lh_cast_static(lh_bool_t, (dx <= eps && lh_math_neg(dx) <= eps) &&
                                         (dy <= eps && lh_math_neg(dy) <= eps));
}

lh_bool_t
lh_math_vec2_equals(lh_math_vec2_t self, lh_math_vec2_t other)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_math_vec2_get_x(lh_addr_of(self)),
                                                lh_math_vec2_get_x(lh_addr_of(other))) &&
                                         lh_math_eq(lh_math_vec2_get_y(lh_addr_of(self)),
                                                    lh_math_vec2_get_y(lh_addr_of(other))));
}