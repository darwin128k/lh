/**
 * @file size.c
 * @brief Implementation of lh/math/size.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/util/return.h>
#include <lh/math/size.h>
#include <lh/math.h>

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_math_size_init(lh_math_size_t *self, lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_math_size_set_width(self, width);
    lh_math_size_set_height(self, height);
}

lh_void
lh_math_size_init_empty(lh_math_size_t *self)
{
    lh_math_size_init(self, 0, 0);
}

lh_math_size_t
lh_math_size_from_extent(const lh_math_point_t *min, const lh_math_point_t *max)
{
    lh_assert_runtime_ref(min);
    lh_assert_runtime_ref(max);
    lh_math_size_t s;
    lh_math_size_init(lh_addr_of(s), lh_math_point_get_x(max) - lh_math_point_get_x(min),
                      lh_math_point_get_y(max) - lh_math_point_get_y(min));
    return s;
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
lh_math_size_equals(const lh_math_size_t *self, const lh_math_size_t *other)
{
    return lh_math_size_eq(self, other);
}

lh_bool_t
lh_math_size_is_at_least(const lh_math_size_t *self, const lh_math_size_t *minimum)
{
    lh_math_scalar_t self_width;
    lh_math_scalar_t minimum_width;
    lh_math_scalar_t self_height;
    lh_math_scalar_t minimum_height;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(minimum);
    self_width = lh_math_size_get_width(self);
    minimum_width = lh_math_size_get_width(minimum);
    if (lh_math_ne(self_width, minimum_width))
    {
        return lh_cast_static(lh_bool_t, lh_math_gt(self_width, minimum_width));
    }
    self_height = lh_math_size_get_height(self);
    minimum_height = lh_math_size_get_height(minimum);
    return lh_cast_static(lh_bool_t, lh_math_ge(self_height, minimum_height));
}

lh_bool_t
lh_math_size_is_less(const lh_math_size_t *self, const lh_math_size_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_math_size_is_at_least(self, other));
}

lh_bool_t
lh_math_size_is_greater(const lh_math_size_t *self, const lh_math_size_t *other)
{
    return lh_math_size_is_less(other, self);
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
    lh_math_size_t s;
    lh_math_size_init(lh_addr_of(s), lh_math_size_get_width(self) - dx - dx,
                      lh_math_size_get_height(self) - dy - dy);
    return s;
}
