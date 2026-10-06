/**
 * @file frect.c
 * @brief Implementation of lh/math/frect.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/math.h>
#include <lh/math/frect.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_frect_t
lh_math_frect_make(lh_math_fscalar_t x, lh_math_fscalar_t y, lh_math_fscalar_t width, lh_math_fscalar_t height)
{
    lh_math_frect_t r;
    lh_math_frect_init(lh_addr_of(r), x, y, width, height);
    return r;
}

lh_math_frect_t
lh_math_frect_make_origin_size(lh_math_fpoint_t origin, lh_math_fsize_t size)
{
    lh_math_frect_t r;
    lh_math_frect_init_origin_size(lh_addr_of(r), origin, size);
    return r;
}

lh_math_frect_t
lh_math_frect_from_extent(const lh_math_fpoint_t *min, const lh_math_fpoint_t *max)
{
    lh_math_fsize_t size;
    lh_assert_runtime_ref(min);
    lh_assert_runtime_ref(max);
    size = lh_math_fsize_from_extent(min, max);
    if (lh_math_fsize_is_empty(lh_addr_of(size)))
    {
        return lh_math_frect_make_empty();
    }
    return lh_math_frect_make_origin_size(lh_ptr_deref(min), size);
}

lh_math_frect_t
lh_math_frect_make_empty(void)
{
    return lh_math_frect_make(0, 0, 0, 0);
}

lh_void
lh_math_frect_init(lh_math_frect_t *self, lh_math_fscalar_t x, lh_math_fscalar_t y, lh_math_fscalar_t width,
                   lh_math_fscalar_t height)
{
    lh_math_frect_init_origin_size(self, lh_math_fpoint_make(x, y), lh_math_fsize_make(width, height));
}

lh_void
lh_math_frect_init_origin_size(lh_math_frect_t *self, lh_math_fpoint_t origin, lh_math_fsize_t size)
{
    lh_assert_runtime_ref(self);
    lh_math_frect_set_origin(self, origin);
    lh_math_frect_set_size(self, size);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_fpoint_t *
lh_math_frect_get_origin(lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

const lh_math_fpoint_t *
lh_math_frect_get_origin_as_const(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_math_fsize_t *
lh_math_frect_get_size(lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

const lh_math_fsize_t *
lh_math_frect_get_size_as_const(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

lh_void
lh_math_frect_set_origin(lh_math_frect_t *self, lh_math_fpoint_t origin)
{
    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_math_frect_get_origin(self)) = origin;
}

lh_void
lh_math_frect_set_size(lh_math_frect_t *self, lh_math_fsize_t size)
{
    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_math_frect_get_size(self)) = size;
}

lh_math_fpoint_t
lh_math_frect_far(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_fpoint_offset_size(lh_math_frect_get_origin_as_const(self),
                                      lh_math_frect_get_size_as_const(self));
}

/* ── Conversions ─────────────────────────────────────────────────────────── */

lh_math_rect_t
lh_math_frect_to_rect(lh_math_frect_t self)
{
    lh_math_rect_t r;
    lh_math_rect_set_origin(
        lh_addr_of(r), lh_math_fpoint_to_point(lh_ptr_deref(
                           lh_math_frect_get_origin_as_const(lh_addr_of(self)))));
    lh_math_rect_set_size(lh_addr_of(r),
                          lh_math_fsize_to_size(lh_ptr_deref(
                              lh_math_frect_get_size_as_const(lh_addr_of(self)))));
    return r;
}

lh_math_frect_t
lh_math_rect_to_frect(lh_math_rect_t self)
{
    lh_math_frect_t r;
    lh_math_frect_set_origin(
        lh_addr_of(r), lh_math_point_to_fpoint(lh_ptr_deref(
                           lh_math_rect_get_origin_as_const(lh_addr_of(self)))));
    lh_math_frect_set_size(lh_addr_of(r),
                           lh_math_size_to_fsize(lh_ptr_deref(
                               lh_math_rect_get_size_as_const(lh_addr_of(self)))));
    return r;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_frect_is_empty(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_fsize_is_empty(lh_math_frect_get_size_as_const(self));
}

lh_bool_t
lh_math_frect_contains_point(const lh_math_frect_t *self, lh_math_fpoint_t point)
{
    lh_math_fpoint_t far;
    lh_assert_runtime_ref(self);
    if (lh_math_frect_is_empty(self))
    {
        return lh_bool_false;
    }
    far = lh_math_frect_far(self);
    return lh_math_fpoint_in_extent(lh_addr_of(point), lh_math_frect_get_origin_as_const(self),
                                    lh_addr_of(far));
}

lh_bool_t
lh_math_frect_intersects(const lh_math_frect_t *a, const lh_math_frect_t *b)
{
    lh_math_fpoint_t origin;
    lh_math_fpoint_t far_a;
    lh_math_fpoint_t far_b;
    lh_math_fpoint_t far;
    lh_math_fsize_t size;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_frect_is_empty(a) || lh_math_frect_is_empty(b))
    {
        return lh_bool_false;
    }
    origin = lh_math_fpoint_max(lh_math_frect_get_origin_as_const(a), lh_math_frect_get_origin_as_const(b));
    far_a = lh_math_frect_far(a);
    far_b = lh_math_frect_far(b);
    far = lh_math_fpoint_min(lh_addr_of(far_a), lh_addr_of(far_b));
    size = lh_math_fsize_from_extent(lh_addr_of(origin), lh_addr_of(far));
    return !lh_math_fsize_is_empty(lh_addr_of(size));
}

lh_bool_t
lh_math_frect_eq(const lh_math_frect_t *a, const lh_math_frect_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_fpoint_eq(lh_math_frect_get_origin_as_const(a), lh_math_frect_get_origin_as_const(b))
        && lh_math_fsize_eq(lh_math_frect_get_size_as_const(a), lh_math_frect_get_size_as_const(b));
}

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_frect_t
lh_math_frect_intersection(const lh_math_frect_t *a, const lh_math_frect_t *b)
{
    lh_math_fpoint_t origin;
    lh_math_fpoint_t far_a;
    lh_math_fpoint_t far_b;
    lh_math_fpoint_t far;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_frect_is_empty(a) || lh_math_frect_is_empty(b))
    {
        return lh_math_frect_make_empty();
    }
    origin = lh_math_fpoint_max(lh_math_frect_get_origin_as_const(a), lh_math_frect_get_origin_as_const(b));
    far_a = lh_math_frect_far(a);
    far_b = lh_math_frect_far(b);
    far = lh_math_fpoint_min(lh_addr_of(far_a), lh_addr_of(far_b));
    return lh_math_frect_from_extent(lh_addr_of(origin), lh_addr_of(far));
}

lh_math_frect_t
lh_math_frect_union(const lh_math_frect_t *a, const lh_math_frect_t *b)
{
    lh_math_fpoint_t origin;
    lh_math_fpoint_t far_a;
    lh_math_fpoint_t far_b;
    lh_math_fpoint_t far;
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_frect_is_empty(a))
    {
        return lh_ptr_deref(b);
    }
    if (lh_math_frect_is_empty(b))
    {
        return lh_ptr_deref(a);
    }
    origin = lh_math_fpoint_min(lh_math_frect_get_origin_as_const(a), lh_math_frect_get_origin_as_const(b));
    far_a = lh_math_frect_far(a);
    far_b = lh_math_frect_far(b);
    far = lh_math_fpoint_max(lh_addr_of(far_a), lh_addr_of(far_b));
    return lh_math_frect_from_extent(lh_addr_of(origin), lh_addr_of(far));
}

lh_math_frect_t
lh_math_frect_offset(const lh_math_frect_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy)
{
    lh_assert_runtime_ref(self);
    return lh_math_frect_make_origin_size(
        lh_math_fpoint_offset(lh_math_frect_get_origin_as_const(self), dx, dy),
        lh_ptr_deref(lh_math_frect_get_size_as_const(self)));
}

lh_math_frect_t
lh_math_frect_inset(const lh_math_frect_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy)
{
    lh_assert_runtime_ref(self);
    return lh_math_frect_make_origin_size(
        lh_math_fpoint_offset(lh_math_frect_get_origin_as_const(self), dx, dy),
        lh_math_fsize_inset(lh_math_frect_get_size_as_const(self), dx, dy));
}
