#include <lh/assert/runtime.h>
#include <lh/assert/static.h>
#include <lh/math/vec4.h>
#include <lh/util/offset.h>

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

/* ── Conversions ─────────────────────────────────────────────────────────── */

lh_void
lh_math_vec3_to_vec4(lh_math_vec3_t v, lh_float_t w, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result), lh_math_vec3_get_x(lh_addr_of(v)),
                      lh_math_vec3_get_y(lh_addr_of(v)), lh_math_vec3_get_z(lh_addr_of(v)), w);
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec4_to_vec3(lh_math_vec4_t v, lh_math_vec3_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec3_t result;
    lh_math_vec3_init(lh_addr_of(result), lh_math_vec4_get_x(lh_addr_of(v)),
                      lh_math_vec4_get_y(lh_addr_of(v)), lh_math_vec4_get_z(lh_addr_of(v)));
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec4_add(lh_math_vec4_t a, lh_math_vec4_t b, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result),
                      lh_math_vec4_get_x(lh_addr_of(a)) + lh_math_vec4_get_x(lh_addr_of(b)),
                      lh_math_vec4_get_y(lh_addr_of(a)) + lh_math_vec4_get_y(lh_addr_of(b)),
                      lh_math_vec4_get_z(lh_addr_of(a)) + lh_math_vec4_get_z(lh_addr_of(b)),
                      lh_math_vec4_get_w(lh_addr_of(a)) + lh_math_vec4_get_w(lh_addr_of(b)));
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec4_sub(lh_math_vec4_t a, lh_math_vec4_t b, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result),
                      lh_math_vec4_get_x(lh_addr_of(a)) - lh_math_vec4_get_x(lh_addr_of(b)),
                      lh_math_vec4_get_y(lh_addr_of(a)) - lh_math_vec4_get_y(lh_addr_of(b)),
                      lh_math_vec4_get_z(lh_addr_of(a)) - lh_math_vec4_get_z(lh_addr_of(b)),
                      lh_math_vec4_get_w(lh_addr_of(a)) - lh_math_vec4_get_w(lh_addr_of(b)));
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec4_scale(lh_math_vec4_t v, lh_float_t s, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result), lh_math_vec4_get_x(lh_addr_of(v)) * s,
                      lh_math_vec4_get_y(lh_addr_of(v)) * s, lh_math_vec4_get_z(lh_addr_of(v)) * s,
                      lh_math_vec4_get_w(lh_addr_of(v)) * s);
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_vec4_madd(lh_math_vec4_t a, lh_math_vec4_t b, lh_float_t s, lh_math_vec4_t *out)
{
    lh_math_vec4_init(out,
                      lh_math_vec4_get_x(lh_addr_of(a)) + lh_math_vec4_get_x(lh_addr_of(b)) * s,
                      lh_math_vec4_get_y(lh_addr_of(a)) + lh_math_vec4_get_y(lh_addr_of(b)) * s,
                      lh_math_vec4_get_z(lh_addr_of(a)) + lh_math_vec4_get_z(lh_addr_of(b)) * s,
                      lh_math_vec4_get_w(lh_addr_of(a)) + lh_math_vec4_get_w(lh_addr_of(b)) * s);
}

lh_void
lh_math_vec4_neg(lh_math_vec4_t v, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_vec4_t result;
    lh_math_vec4_init(lh_addr_of(result), lh_math_neg(lh_math_vec4_get_x(lh_addr_of(v))),
                      lh_math_neg(lh_math_vec4_get_y(lh_addr_of(v))), lh_math_neg(lh_math_vec4_get_z(lh_addr_of(v))),
                      lh_math_neg(lh_math_vec4_get_w(lh_addr_of(v))));
    lh_ptr_deref(out) = result;
}

/* lh_math_vec4_view_of reads the vector as lh_float_t[4]: the components must
   follow each other with no padding, from the first byte. */
lh_assert_static(sizeof(lh_math_vec4_t) == 4U * sizeof(lh_float_t), "lh_math_vec4_t must be laid out as lh_float_t[4]");
lh_assert_static(lh_offset_of(lh_math_vec4_t, x) == 0U, "lh_math_vec4_t must start with x");

lh_void
lh_math_vec4_view_of(const lh_math_vec4_t *v, lh_math_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_slice_t slice;
    lh_math_view_t view;

    lh_math_slice_init(lh_addr_of(slice), lh_ptr_rcast(const lh_float_t, v), 4U);
    lh_math_view_init_by_slice(lh_addr_of(view), lh_addr_of(slice));
    lh_ptr_deref(out) = view;
}

lh_float_t
lh_math_vec4_dot(lh_math_vec4_t a, lh_math_vec4_t b)
{
    lh_math_view_t va;
    lh_math_vec4_view_of(lh_addr_of(a), lh_addr_of(va));
    lh_math_view_t vb;
    lh_math_vec4_view_of(lh_addr_of(b), lh_addr_of(vb));

    return lh_math_view_dot(lh_addr_of(va), lh_addr_of(vb));
}

lh_float_t
lh_math_vec4_length_sq(lh_math_vec4_t v)
{
    lh_math_view_t view;
    lh_math_vec4_view_of(lh_addr_of(v), lh_addr_of(view));

    return lh_math_view_length_sq(lh_addr_of(view));
}

lh_float_t
lh_math_vec4_length(lh_math_vec4_t v)
{
    lh_math_view_t view;
    lh_math_vec4_view_of(lh_addr_of(v), lh_addr_of(view));

    return lh_math_view_length(lh_addr_of(view));
}

lh_void
lh_math_vec4_normalize(lh_math_vec4_t v, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_float_t length = lh_math_vec4_length(v);
    if (length == 0.0f)
    {
        lh_ptr_deref(out) = v;
        return;
    }
    lh_math_vec4_scale(v, 1.0f / length, out);
}

lh_void
lh_math_vec4_lerp(lh_math_vec4_t a, lh_math_vec4_t b, lh_float_t t, lh_math_vec4_t *out)
{
    lh_assert_runtime_ref(out);
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
    lh_ptr_deref(out) = result;
}

lh_bool_t
lh_math_vec4_near(lh_math_vec4_t a, lh_math_vec4_t b, lh_float_t eps)
{
    lh_math_vec4_t d;
    lh_math_vec4_sub(a, b, lh_addr_of(d));
    const lh_float_t dx = lh_math_vec4_get_x(lh_addr_of(d));
    const lh_float_t dy = lh_math_vec4_get_y(lh_addr_of(d));
    const lh_float_t dz = lh_math_vec4_get_z(lh_addr_of(d));
    const lh_float_t dw = lh_math_vec4_get_w(lh_addr_of(d));
    return lh_cast_static(lh_bool_t, (dx <= eps && lh_math_neg(dx) <= eps)
                                   && (dy <= eps && lh_math_neg(dy) <= eps)
                                   && (dz <= eps && lh_math_neg(dz) <= eps)
                                   && (dw <= eps && lh_math_neg(dw) <= eps));
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