/**
 * @file scalar.c
 * @brief Implementation of `lh/math/point/scalar.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/util/return.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/math/point/scalar.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_point_scalar_t
lh_math_point_scalar_make(lh_math_scalar_t x, lh_math_scalar_t y)
{
    lh_math_point_scalar_t p;
    lh_math_point_scalar_set_x(lh_addr_of(p), x);
    lh_math_point_scalar_set_y(lh_addr_of(p), y);
    return p;
}

lh_math_point_scalar_t
lh_math_point_scalar_make_empty(void)
{
    return lh_math_point_scalar_make(0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_scalar_t
lh_math_point_scalar_get_x(const lh_math_point_scalar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_math_scalar_t
lh_math_point_scalar_get_y(const lh_math_point_scalar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_void
lh_math_point_scalar_set_x(lh_math_point_scalar_t *self, lh_math_scalar_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_math_point_scalar_set_y(lh_math_point_scalar_t *self, lh_math_scalar_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

/* ── Conversions ─────────────────────────────────────────────────────────── */

lh_math_point_t
lh_math_point_scalar_to_point(lh_math_point_scalar_t self)
{
    return lh_math_point_make(
        lh_cast_static(lh_math_coord_t, lh_math_point_scalar_get_x(lh_addr_of(self))),
        lh_cast_static(lh_math_coord_t, lh_math_point_scalar_get_y(lh_addr_of(self))));
}

lh_math_point_scalar_t
lh_math_point_to_point_scalar(lh_math_point_t self)
{
    return lh_math_point_scalar_make(
        lh_cast_static(lh_math_scalar_t, lh_math_point_get_x(lh_addr_of(self))),
        lh_cast_static(lh_math_scalar_t, lh_math_point_get_y(lh_addr_of(self))));
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_point_scalar_eq(const lh_math_point_scalar_t *a, const lh_math_point_scalar_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_point_scalar_get_x(a), lh_math_point_scalar_get_x(b))
        && lh_math_eq(lh_math_point_scalar_get_y(a), lh_math_point_scalar_get_y(b));
}
