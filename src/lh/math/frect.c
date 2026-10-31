/**
 * @file frect.c
 * @brief Implementation of lh/math/frect.h.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/math/frect.h>
#include <lh/util/return.h>

/* -- Constructors ---------------------------------------------------------- */

lh_math_fpoint_t *
lh_math_frect_get_origin_mut(lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

const lh_math_fpoint_t *
lh_math_frect_get_origin(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_math_fsize_t *
lh_math_frect_get_size_mut(lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

const lh_math_fsize_t *
lh_math_frect_get_size(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

/* -- Accessors ------------------------------------------------------------- */

lh_void
lh_math_frect_set_origin(lh_math_frect_t *self, lh_math_fpoint_t origin)
{
    lh_ptr_deref(lh_math_frect_get_origin_mut(self)) = origin;
}

lh_void
lh_math_frect_set_size(lh_math_frect_t *self, lh_math_fsize_t size)
{
    lh_ptr_deref(lh_math_frect_get_size_mut(self)) = size;
}

lh_void
lh_math_frect_init_origin_size(lh_math_frect_t *self, lh_math_fpoint_t origin, lh_math_fsize_t size)
{
    lh_math_frect_set_origin(self, origin);
    lh_math_frect_set_size(self, size);
}

lh_void
lh_math_frect_init(lh_math_frect_t *self, lh_math_fscalar_t x, lh_math_fscalar_t y,
                   lh_math_fscalar_t width, lh_math_fscalar_t height)
{
    lh_math_fpoint_t origin;
    lh_math_fpoint_init(lh_addr_of(origin), x, y);
    lh_math_fsize_t size;
    lh_math_fsize_init(lh_addr_of(size), width, height);
    lh_math_frect_init_origin_size(self, origin, size);
}

lh_void
lh_math_frect_init_empty(lh_math_frect_t *self)
{
    lh_math_frect_init(self, 0, 0, 0, 0);
}

lh_void
lh_math_frect_from_extent(const lh_math_fpoint_t *min, const lh_math_fpoint_t *max,
                          lh_math_frect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_fsize_t size;

    lh_math_fsize_from_extent(min, max, lh_addr_of(size));
    if (lh_math_fsize_is_empty(lh_addr_of(size)))
    {
        lh_math_frect_t _lh_tmp;
        lh_math_frect_init_empty(lh_addr_of(_lh_tmp));
        lh_ptr_deref(out) = _lh_tmp;
        return;
    }
    lh_math_frect_t result;
    lh_math_frect_init_origin_size(lh_addr_of(result), lh_ptr_deref(min), size);
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_frect_far(const lh_math_frect_t *self, lh_math_fpoint_t *out)
{
    lh_math_fpoint_offset_size(lh_math_frect_get_origin(self), lh_math_frect_get_size(self), out);
}

lh_void
lh_math_frect_origin_min(const lh_math_frect_t *a, const lh_math_frect_t *b, lh_math_fpoint_t *out)
{
    lh_math_fpoint_min(lh_math_frect_get_origin(a), lh_math_frect_get_origin(b), out);
}

lh_void
lh_math_frect_origin_max(const lh_math_frect_t *a, const lh_math_frect_t *b, lh_math_fpoint_t *out)
{
    lh_math_fpoint_max(lh_math_frect_get_origin(a), lh_math_frect_get_origin(b), out);
}

lh_void
lh_math_frect_far_min(const lh_math_frect_t *a, const lh_math_frect_t *b, lh_math_fpoint_t *out)
{
    lh_math_fpoint_t far_a;

    lh_math_frect_far(a, lh_addr_of(far_a));
    lh_math_fpoint_t far_b;

    lh_math_frect_far(b, lh_addr_of(far_b));
    lh_math_fpoint_min(lh_addr_of(far_a), lh_addr_of(far_b), out);
}

lh_void
lh_math_frect_far_max(const lh_math_frect_t *a, const lh_math_frect_t *b, lh_math_fpoint_t *out)
{
    lh_math_fpoint_t far_a;

    lh_math_frect_far(a, lh_addr_of(far_a));
    lh_math_fpoint_t far_b;

    lh_math_frect_far(b, lh_addr_of(far_b));
    lh_math_fpoint_max(lh_addr_of(far_a), lh_addr_of(far_b), out);
}

/* -- Conversions ----------------------------------------------------------- */

lh_void
lh_math_frect_to_rect(lh_math_frect_t self, lh_math_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_point_t point;

    lh_math_fpoint_to_point(lh_ptr_deref(lh_math_frect_get_origin(lh_addr_of(self))),
                            lh_addr_of(point));
    lh_math_rect_t r;

    lh_math_rect_set_origin(lh_addr_of(r), point);
    lh_math_size_t size;

    lh_math_fsize_to_size(lh_ptr_deref(lh_math_frect_get_size(lh_addr_of(self))), lh_addr_of(size));
    lh_math_rect_set_size(lh_addr_of(r), size);
    lh_ptr_deref(out) = r;
}

lh_void
lh_math_rect_to_frect(lh_math_rect_t self, lh_math_frect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_fpoint_t fpoint;

    lh_math_point_to_fpoint(lh_ptr_deref(lh_math_rect_get_origin(lh_addr_of(self))),
                            lh_addr_of(fpoint));
    lh_math_frect_t r;

    lh_math_frect_set_origin(lh_addr_of(r), fpoint);
    lh_math_fsize_t fsize;

    lh_math_size_to_fsize(lh_ptr_deref(lh_math_rect_get_size(lh_addr_of(self))), lh_addr_of(fsize));
    lh_math_frect_set_size(lh_addr_of(r), fsize);
    lh_ptr_deref(out) = r;
}

/* -- Queries --------------------------------------------------------------- */

lh_bool_t
lh_math_frect_is_empty(const lh_math_frect_t *self)
{
    return lh_math_fsize_is_empty(lh_math_frect_get_size(self));
}

lh_bool_t
lh_math_frect_contains_point(const lh_math_frect_t *self, lh_math_fpoint_t point)
{
    if (lh_math_frect_is_empty(self))
    {
        return lh_bool_false;
    }
    lh_math_fpoint_t far;

    lh_math_frect_far(self, lh_addr_of(far));
    return lh_math_fpoint_in_extent(lh_addr_of(point), lh_math_frect_get_origin(self),
                                    lh_addr_of(far));
}

lh_bool_t
lh_math_frect_eq(const lh_math_frect_t *a, const lh_math_frect_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    return lh_math_fpoint_eq(lh_math_frect_get_origin(a), lh_math_frect_get_origin(b)) &&
           lh_math_fsize_eq(lh_math_frect_get_size(a), lh_math_frect_get_size(b));
}

lh_bool_t
lh_math_frect_equals(const lh_math_frect_t *self, const lh_math_frect_t *other)
{
    return lh_math_frect_eq(self, other);
}

lh_bool_t
lh_math_frect_is_at_least(const lh_math_frect_t *self, const lh_math_frect_t *minimum)
{
    const lh_math_fpoint_t *self_origin = lh_math_frect_get_origin(self);
    const lh_math_fpoint_t *minimum_origin = lh_math_frect_get_origin(minimum);

    if (!lh_math_fpoint_is_at_least(self_origin, minimum_origin))
    {
        return lh_bool_false;
    }
    if (lh_math_fpoint_is_greater(self_origin, minimum_origin))
    {
        return lh_bool_true;
    }
    return lh_math_fsize_is_at_least(lh_math_frect_get_size(self), lh_math_frect_get_size(minimum));
}

lh_bool_t
lh_math_frect_is_less(const lh_math_frect_t *self, const lh_math_frect_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_math_frect_is_at_least(self, other));
}

lh_bool_t
lh_math_frect_is_greater(const lh_math_frect_t *self, const lh_math_frect_t *other)
{
    return lh_math_frect_is_less(other, self);
}

lh_void
lh_math_frect_intersection(const lh_math_frect_t *a, const lh_math_frect_t *b, lh_math_frect_t *out)
{
    lh_assert_runtime_ref(out);
    if (lh_math_frect_is_empty(a) || lh_math_frect_is_empty(b))
    {
        lh_math_frect_t _lh_tmp;
        lh_math_frect_init_empty(lh_addr_of(_lh_tmp));
        lh_ptr_deref(out) = _lh_tmp;
        return;
    }
    lh_math_fpoint_t origin;

    lh_math_frect_origin_max(a, b, lh_addr_of(origin));
    lh_math_fpoint_t far;

    lh_math_frect_far_min(a, b, lh_addr_of(far));
    lh_math_frect_from_extent(lh_addr_of(origin), lh_addr_of(far), out);
}

/* -- Set ops --------------------------------------------------------------- */

lh_bool_t
lh_math_frect_intersects(const lh_math_frect_t *a, const lh_math_frect_t *b)
{
    lh_math_frect_t overlap;

    lh_math_frect_intersection(a, b, lh_addr_of(overlap));
    return !lh_math_frect_is_empty(lh_addr_of(overlap));
}

lh_void
lh_math_frect_union(const lh_math_frect_t *a, const lh_math_frect_t *b, lh_math_frect_t *out)
{
    lh_assert_runtime_ref(out);
    /* An empty `a` copies `b` as it is, so `b` is read before any check. */
    lh_assert_runtime_ref(b);
    if (lh_math_frect_is_empty(a))
    {
        lh_ptr_deref(out) = lh_ptr_deref(b);
        return;
    }
    if (lh_math_frect_is_empty(b))
    {
        lh_ptr_deref(out) = lh_ptr_deref(a);
        return;
    }
    lh_math_fpoint_t origin;

    lh_math_frect_origin_min(a, b, lh_addr_of(origin));
    lh_math_fpoint_t far;

    lh_math_frect_far_max(a, b, lh_addr_of(far));
    lh_math_frect_from_extent(lh_addr_of(origin), lh_addr_of(far), out);
}

lh_void
lh_math_frect_offset(const lh_math_frect_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy,
                     lh_math_frect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_fpoint_t fpoint;

    lh_math_fpoint_offset(lh_math_frect_get_origin(self), dx, dy, lh_addr_of(fpoint));
    lh_math_frect_t result;

    lh_math_frect_init_origin_size(lh_addr_of(result), fpoint,
                                   lh_ptr_deref(lh_math_frect_get_size(self)));
    lh_ptr_deref(out) = result;
}

lh_void
lh_math_frect_inset(const lh_math_frect_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy,
                    lh_math_frect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_math_fsize_t fsize;

    lh_math_fsize_inset(lh_math_frect_get_size(self), dx, dy, lh_addr_of(fsize));
    lh_math_fpoint_t fpoint;

    lh_math_fpoint_offset(lh_math_frect_get_origin(self), dx, dy, lh_addr_of(fpoint));
    lh_math_frect_t result;

    lh_math_frect_init_origin_size(lh_addr_of(result), fpoint, fsize);
    lh_ptr_deref(out) = result;
}
