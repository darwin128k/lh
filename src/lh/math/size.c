/**
 * @file size.c
 * @brief Implementation of lh/math/size.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/util/return.h>
#include <lh/math/size.h>
#include <lh/math.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_size_t
lh_math_size_make(lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_math_size_t s;
    lh_math_size_set_width(lh_addr_of(s), width);
    lh_math_size_set_height(lh_addr_of(s), height);
    return s;
}

lh_math_size_t
lh_math_size_make_empty(void)
{
    return lh_math_size_make(0, 0);
}

lh_math_size_t
lh_math_size_from_extent(const lh_math_point_t *min, const lh_math_point_t *max)
{
    lh_assert_runtime_ref(min);
    lh_assert_runtime_ref(max);
    return lh_math_size_make(lh_math_point_get_x(max) - lh_math_point_get_x(min),
                             lh_math_point_get_y(max) - lh_math_point_get_y(min));
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_scalar_t
lh_math_size_get_width(const lh_math_size_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_math_scalar_t
lh_math_size_get_height(const lh_math_size_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_math_size_set_width(lh_math_size_t *self, lh_math_scalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_math_size_set_height(lh_math_size_t *self, lh_math_scalar_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_size_eq(const lh_math_size_t *a, const lh_math_size_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_size_get_width(a), lh_math_size_get_width(b))
        && lh_math_eq(lh_math_size_get_height(a), lh_math_size_get_height(b));
}

lh_bool_t
lh_math_size_is_empty(const lh_math_size_t *self)
{
    lh_assert_runtime_ref(self);
    return (lh_math_size_get_width(self) <= 0) || (lh_math_size_get_height(self) <= 0);
}

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_size_t
lh_math_size_inset(const lh_math_size_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy)
{
    lh_assert_runtime_ref(self);
    return lh_math_size_make(lh_math_size_get_width(self) - dx - dx,
                             lh_math_size_get_height(self) - dy - dy);
}
