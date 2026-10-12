/**
 * @file point.c
 * @brief Implementation of lh/math/point.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/util/return.h>
#include <lh/cast/static.h>
#include <lh/config.h>
#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/float/round.h>
#endif
#include <lh/math/point.h>
#include <lh/math/size.h>
#include <lh/null.h>
#include <lh/math.h>

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_math_point_init(lh_math_point_t *self, lh_math_scalar_t x, lh_math_scalar_t y)
{
    lh_math_point_set_x(self, x);
    lh_math_point_set_y(self, y);
}

lh_void
lh_math_point_init_empty(lh_math_point_t *self)
{
    lh_math_point_init(self, 0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_scalar_t
lh_math_point_get_x(const lh_math_point_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_math_scalar_t
lh_math_point_get_y(const lh_math_point_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_void
lh_math_point_set_x(lh_math_point_t *self, lh_math_scalar_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_math_point_set_y(lh_math_point_t *self, lh_math_scalar_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

/* ── Conversions ─────────────────────────────────────────────────────────── */

#if LH_LIBRARY_OPTION_MATH_FPU
lh_math_vec2_t
lh_math_point_to_vec2(lh_math_point_t self)
{
    lh_math_vec2_t v;
    lh_math_vec2_init(lh_addr_of(v),
                      lh_cast_static(lh_float_t, lh_math_point_get_x(lh_addr_of(self))),
                      lh_cast_static(lh_float_t, lh_math_point_get_y(lh_addr_of(self))));
    return v;
}

lh_math_point_t
lh_math_vec2_to_point(lh_math_vec2_t v)
{
    /* `ceil(x - 0.5f)`: round to nearest, halves toward the lower pixel, the
     * same rule a pixel-coverage rasterizer typically uses. */
    lh_math_point_t p;
    lh_math_point_init(
        lh_addr_of(p),
        lh_cast_static(lh_math_scalar_t, lh_float_ceil_to_int(lh_math_vec2_get_x(lh_addr_of(v)) - 0.5f)),
        lh_cast_static(lh_math_scalar_t, lh_float_ceil_to_int(lh_math_vec2_get_y(lh_addr_of(v)) - 0.5f)));
    return p;
}
#endif

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_point_eq(const lh_math_point_t *a, const lh_math_point_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_point_get_x(a), lh_math_point_get_x(b))
        && lh_math_eq(lh_math_point_get_y(a), lh_math_point_get_y(b));
}

lh_bool_t
lh_math_point_equals(const lh_math_point_t *self, const lh_math_point_t *other)
{
    return lh_math_point_eq(self, other);
}

lh_bool_t
lh_math_point_is_at_least(const lh_math_point_t *self, const lh_math_point_t *minimum)
{
    lh_math_scalar_t self_x;
    lh_math_scalar_t minimum_x;
    lh_math_scalar_t self_y;
    lh_math_scalar_t minimum_y;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(minimum);
    self_x = lh_math_point_get_x(self);
    minimum_x = lh_math_point_get_x(minimum);
    if (lh_math_ne(self_x, minimum_x))
    {
        return lh_cast_static(lh_bool_t, lh_math_gt(self_x, minimum_x));
    }
    self_y = lh_math_point_get_y(self);
    minimum_y = lh_math_point_get_y(minimum);
    return lh_cast_static(lh_bool_t, lh_math_ge(self_y, minimum_y));
}

lh_bool_t
lh_math_point_is_less(const lh_math_point_t *self, const lh_math_point_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_math_point_is_at_least(self, other));
}

lh_bool_t
lh_math_point_is_greater(const lh_math_point_t *self, const lh_math_point_t *other)
{
    return lh_math_point_is_less(other, self);
}

lh_math_point_t
lh_math_point_min(const lh_math_point_t *a, const lh_math_point_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    lh_math_point_t p;
    lh_math_point_init(lh_addr_of(p), lh_math_min(lh_math_point_get_x(a), lh_math_point_get_x(b)),
                       lh_math_min(lh_math_point_get_y(a), lh_math_point_get_y(b)));
    return p;
}

lh_math_point_t
lh_math_point_max(const lh_math_point_t *a, const lh_math_point_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    lh_math_point_t p;
    lh_math_point_init(lh_addr_of(p), lh_math_max(lh_math_point_get_x(a), lh_math_point_get_x(b)),
                       lh_math_max(lh_math_point_get_y(a), lh_math_point_get_y(b)));
    return p;
}

lh_bool_t
lh_math_point_in_extent(const lh_math_point_t *self, const lh_math_point_t *min,
                        const lh_math_point_t *max)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(min);
    lh_assert_runtime_ref(max);
    if (lh_math_lt(lh_math_point_get_x(self), lh_math_point_get_x(min)))
    {
        return lh_bool_false;
    }
    if (lh_math_lt(lh_math_point_get_y(self), lh_math_point_get_y(min)))
    {
        return lh_bool_false;
    }
    if (lh_math_ge(lh_math_point_get_x(self), lh_math_point_get_x(max)))
    {
        return lh_bool_false;
    }
    if (lh_math_ge(lh_math_point_get_y(self), lh_math_point_get_y(max)))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_point_t
lh_math_point_offset(const lh_math_point_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy)
{
    lh_assert_runtime_ref(self);
    lh_math_point_t p;
    lh_math_point_init(lh_addr_of(p), lh_math_point_get_x(self) + dx,
                       lh_math_point_get_y(self) + dy);
    return p;
}

lh_math_point_t
lh_math_point_offset_size(const lh_math_point_t *self, const lh_math_size_t *size)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(size);
    return lh_math_point_offset(self, lh_math_size_get_width(size), lh_math_size_get_height(size));
}