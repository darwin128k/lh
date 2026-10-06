/**
 * @file rect.c
 * @brief Implementation of lh/math/rect.h.
 *
 * Thin composition of ::lh_math_point_t and ::lh_math_size_t. Empty rectangles
 * follow ::lh_math_size_is_empty on the embedded size.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/math/rect.h>
#include <lh/math.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_rect_t
lh_math_rect_from_extent(const lh_math_point_t *min, const lh_math_point_t *max)
{
    lh_math_size_t size;
    lh_assert_runtime_ref(min);
    lh_assert_runtime_ref(max);
    size = lh_math_size_from_extent(min, max);
    if (lh_math_size_is_empty(lh_addr_of(size)))
    {
        lh_math_rect_t _lh_tmp;
    lh_math_rect_init_empty(lh_addr_of(_lh_tmp));
    return _lh_tmp;
    }
    return ({ lh_math_rect_t _v; lh_math_rect_init_origin_size(lh_addr_of(_v), lh_ptr_deref(min), size); _v; });
}

lh_math_rect_t
lh_math_rect_from_min_max(lh_math_scalar_t x_min, lh_math_scalar_t y_min,
                          lh_math_scalar_t x_max, lh_math_scalar_t y_max)
{
    lh_math_point_t min;
    lh_math_point_t max;
    min = ({ lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), x_min, y_min); _v; });
    max = ({ lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), x_max, y_max); _v; });
    return lh_math_rect_from_extent(lh_addr_of(min), lh_addr_of(max));
}

lh_void
lh_math_rect_init(lh_math_rect_t *self, lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width,
                  lh_math_scalar_t height)
{
    lh_math_rect_init_origin_size(self, ({ lh_math_point_t _v; lh_math_point_init(lh_addr_of(_v), x, y); _v; }), ({ lh_math_size_t _v; lh_math_size_init(lh_addr_of(_v), width, height); _v; }));
}

lh_void
lh_math_rect_init_origin_size(lh_math_rect_t *self, lh_math_point_t origin, lh_math_size_t size)
{
    lh_assert_runtime_ref(self);
    lh_math_rect_set_origin(self, origin);
    lh_math_rect_set_size(self, size);
}

lh_void
lh_math_rect_init_empty(lh_math_rect_t *self)
{
    lh_math_rect_init(self, 0, 0, 0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_point_t *
lh_math_rect_get_origin(lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

const lh_math_point_t *
lh_math_rect_get_origin_as_const(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_math_size_t *
lh_math_rect_get_size(lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

const lh_math_size_t *
lh_math_rect_get_size_as_const(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

lh_void
lh_math_rect_set_origin(lh_math_rect_t *self, lh_math_point_t origin)
{
    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_math_rect_get_origin(self)) = origin;
}

lh_void
lh_math_rect_set_size(lh_math_rect_t *self, lh_math_size_t size)
{
    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_math_rect_get_size(self)) = size;
}

lh_math_point_t
lh_math_rect_far(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_point_offset_size(lh_math_rect_get_origin_as_const(self),
                                     lh_math_rect_get_size_as_const(self));
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_rect_is_empty(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_size_is_empty(lh_math_rect_get_size_as_const(self));
}

lh_bool_t
lh_math_rect_contains_point(const lh_math_rect_t *self, lh_math_point_t point)
{
    lh_math_point_t far;
    lh_assert_runtime_ref(self);
    if (lh_math_rect_is_empty(self))
    {
        return lh_bool_false;
    }
    far = lh_math_rect_far(self);
    return lh_math_point_in_extent(lh_addr_of(point), lh_math_rect_get_origin_as_const(self),
                                   lh_addr_of(far));
}

lh_bool_t
lh_math_rect_intersects(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_math_rect_t overlap;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    overlap = lh_math_rect_intersection(a, b);
    return !lh_math_rect_is_empty(lh_addr_of(overlap));
}

lh_bool_t
lh_math_rect_eq(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_point_eq(lh_math_rect_get_origin_as_const(a), lh_math_rect_get_origin_as_const(b))
        && lh_math_size_eq(lh_math_rect_get_size_as_const(a), lh_math_rect_get_size_as_const(b));
}

lh_bool_t
lh_math_rect_equals(const lh_math_rect_t *self, const lh_math_rect_t *other)
{
    return lh_math_rect_eq(self, other);
}

lh_bool_t
lh_math_rect_is_at_least(const lh_math_rect_t *self, const lh_math_rect_t *minimum)
{
    const lh_math_point_t *self_origin;
    const lh_math_point_t *minimum_origin;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(minimum);
    self_origin = lh_math_rect_get_origin_as_const(self);
    minimum_origin = lh_math_rect_get_origin_as_const(minimum);
    if (!lh_math_point_is_at_least(self_origin, minimum_origin))
    {
        return lh_bool_false;
    }
    if (lh_math_point_is_greater(self_origin, minimum_origin))
    {
        return lh_bool_true;
    }
    return lh_math_size_is_at_least(lh_math_rect_get_size_as_const(self),
                                    lh_math_rect_get_size_as_const(minimum));
}

lh_bool_t
lh_math_rect_is_less(const lh_math_rect_t *self, const lh_math_rect_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_math_rect_is_at_least(self, other));
}

lh_bool_t
lh_math_rect_is_greater(const lh_math_rect_t *self, const lh_math_rect_t *other)
{
    return lh_math_rect_is_less(other, self);
}

lh_math_point_t
lh_math_rect_origin_min(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_point_min(lh_math_rect_get_origin_as_const(a), lh_math_rect_get_origin_as_const(b));
}

lh_math_point_t
lh_math_rect_origin_max(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_point_max(lh_math_rect_get_origin_as_const(a), lh_math_rect_get_origin_as_const(b));
}

lh_math_point_t
lh_math_rect_far_min(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_math_point_t far_a;
    lh_math_point_t far_b;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    far_a = lh_math_rect_far(a);
    far_b = lh_math_rect_far(b);
    return lh_math_point_min(lh_addr_of(far_a), lh_addr_of(far_b));
}

lh_math_point_t
lh_math_rect_far_max(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_math_point_t far_a;
    lh_math_point_t far_b;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    far_a = lh_math_rect_far(a);
    far_b = lh_math_rect_far(b);
    return lh_math_point_max(lh_addr_of(far_a), lh_addr_of(far_b));
}

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_rect_t
lh_math_rect_intersection(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_math_point_t origin;
    lh_math_point_t far;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_rect_is_empty(a) || lh_math_rect_is_empty(b))
    {
        lh_math_rect_t _lh_tmp;
    lh_math_rect_init_empty(lh_addr_of(_lh_tmp));
    return _lh_tmp;
    }
    origin = lh_math_rect_origin_max(a, b);
    far = lh_math_rect_far_min(a, b);
    return lh_math_rect_from_extent(lh_addr_of(origin), lh_addr_of(far));
}

lh_math_rect_t
lh_math_rect_union(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_math_point_t origin;
    lh_math_point_t far;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_rect_is_empty(a))
    {
        return lh_ptr_deref(b);
    }
    if (lh_math_rect_is_empty(b))
    {
        return lh_ptr_deref(a);
    }
    origin = lh_math_rect_origin_min(a, b);
    far = lh_math_rect_far_max(a, b);
    return lh_math_rect_from_extent(lh_addr_of(origin), lh_addr_of(far));
}

lh_math_rect_t
lh_math_rect_offset(const lh_math_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy)
{
    lh_assert_runtime_ref(self);
    return ({ lh_math_rect_t _v; lh_math_rect_init_origin_size(lh_addr_of(_v), lh_math_point_offset(lh_math_rect_get_origin_as_const(self), dx, dy),
        lh_ptr_deref(lh_math_rect_get_size_as_const(self))); _v; });
}

lh_math_rect_t
lh_math_rect_inset(const lh_math_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy)
{
    lh_assert_runtime_ref(self);
    return ({ lh_math_rect_t _v; lh_math_rect_init_origin_size(lh_addr_of(_v), lh_math_point_offset(lh_math_rect_get_origin_as_const(self), dx, dy),
        lh_math_size_inset(lh_math_rect_get_size_as_const(self), dx, dy)); _v; });
}
