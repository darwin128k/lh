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
#include <lh/null.h>
#include <lh/math.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_point_t
lh_math_point_make(lh_math_scalar_t x, lh_math_scalar_t y)
{
    lh_math_point_t p;
    lh_math_point_set_x(lh_addr_of(p), x);
    lh_math_point_set_y(lh_addr_of(p), y);
    return p;
}

lh_math_point_t
lh_math_point_make_empty(void)
{
    return lh_math_point_make(0, 0);
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
    return lh_math_vec2_make(lh_cast_static(lh_float_t, lh_math_point_get_x(lh_addr_of(self))),
                             lh_cast_static(lh_float_t, lh_math_point_get_y(lh_addr_of(self))));
}

lh_math_point_t
lh_math_vec2_to_point(lh_math_vec2_t v)
{
    /* `ceil(x - 0.5f)`: round to nearest, halves toward the lower pixel, the
     * same rule a pixel-coverage rasterizer typically uses. */
    return lh_math_point_make(
        lh_cast_static(lh_math_scalar_t, lh_float_ceil_to_int(lh_math_vec2_get_x(lh_addr_of(v)) - 0.5f)),
        lh_cast_static(lh_math_scalar_t, lh_float_ceil_to_int(lh_math_vec2_get_y(lh_addr_of(v)) - 0.5f)));
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

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_point_t
lh_math_point_offset(const lh_math_point_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy)
{
    lh_assert_runtime_ref(self);
    return lh_math_point_make(lh_math_point_get_x(self) + dx,
                             lh_math_point_get_y(self) + dy);
}