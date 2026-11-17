/**
 * @file rect.c
 * @brief Implementation of `lh/gx/rect.h`.
 *
 * Every function here is a thin layer over the chosen math rectangle: it
 * calls that brick and adds nothing, so the contract of each one is the
 * contract of the math rectangle it names.
 */

#include <lh/gx/rect.h>

lh_void
lh_gx_rect_from_extent(const lh_gx_point_t *min, const lh_gx_point_t *max, lh_gx_rect_t *out)
{
    LH_GX_RECT_FN(from_extent)(min, max, out);
}

lh_void
lh_gx_rect_init(lh_gx_rect_t *self, lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t width,
                lh_gx_scalar_t height)
{
    LH_GX_RECT_FN(init)(self, x, y, width, height);
}

lh_void
lh_gx_rect_init_origin_size(lh_gx_rect_t *self, lh_gx_point_t origin, lh_gx_size_t size)
{
    LH_GX_RECT_FN(init_origin_size)(self, origin, size);
}

lh_void
lh_gx_rect_init_empty(lh_gx_rect_t *self)
{
    LH_GX_RECT_FN(init_empty)(self);
}

lh_gx_point_t *
lh_gx_rect_get_origin_mut(lh_gx_rect_t *self)
{
    return LH_GX_RECT_FN(get_origin_mut)(self);
}

const lh_gx_point_t *
lh_gx_rect_get_origin(const lh_gx_rect_t *self)
{
    return LH_GX_RECT_FN(get_origin)(self);
}

lh_gx_size_t *
lh_gx_rect_get_size_mut(lh_gx_rect_t *self)
{
    return LH_GX_RECT_FN(get_size_mut)(self);
}

const lh_gx_size_t *
lh_gx_rect_get_size(const lh_gx_rect_t *self)
{
    return LH_GX_RECT_FN(get_size)(self);
}

lh_void
lh_gx_rect_set_origin(lh_gx_rect_t *self, lh_gx_point_t origin)
{
    LH_GX_RECT_FN(set_origin)(self, origin);
}

lh_void
lh_gx_rect_set_size(lh_gx_rect_t *self, lh_gx_size_t size)
{
    LH_GX_RECT_FN(set_size)(self, size);
}

lh_void
lh_gx_rect_far(const lh_gx_rect_t *self, lh_gx_point_t *out)
{
    LH_GX_RECT_FN(far)(self, out);
}

lh_void
lh_gx_rect_origin_min(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out)
{
    LH_GX_RECT_FN(origin_min)(a, b, out);
}

lh_void
lh_gx_rect_origin_max(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out)
{
    LH_GX_RECT_FN(origin_max)(a, b, out);
}

lh_void
lh_gx_rect_far_min(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out)
{
    LH_GX_RECT_FN(far_min)(a, b, out);
}

lh_void
lh_gx_rect_far_max(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_point_t *out)
{
    LH_GX_RECT_FN(far_max)(a, b, out);
}

lh_void
lh_gx_rect_to_rect(const lh_gx_rect_t *self, lh_math_rect_t *out)
{
    lh_assert_runtime_ref(self);
#if LH_LIBRARY_OPTION_MATH_FPU
    lh_math_frect_to_rect(*self, out);
#else
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = *self;
#endif
}

lh_void
lh_gx_rect_from_rect(const lh_math_rect_t *self, lh_gx_rect_t *out)
{
    lh_assert_runtime_ref(self);
#if LH_LIBRARY_OPTION_MATH_FPU
    lh_math_rect_to_frect(*self, out);
#else
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = *self;
#endif
}

lh_bool_t
lh_gx_rect_is_empty(const lh_gx_rect_t *self)
{
    return LH_GX_RECT_FN(is_empty)(self);
}

lh_bool_t
lh_gx_rect_contains_point(const lh_gx_rect_t *self, lh_gx_point_t point)
{
    return LH_GX_RECT_FN(contains_point)(self, point);
}

lh_bool_t
lh_gx_rect_intersects(const lh_gx_rect_t *a, const lh_gx_rect_t *b)
{
    return LH_GX_RECT_FN(intersects)(a, b);
}

lh_bool_t
lh_gx_rect_eq(const lh_gx_rect_t *a, const lh_gx_rect_t *b)
{
    return LH_GX_RECT_FN(eq)(a, b);
}

lh_bool_t
lh_gx_rect_equals(const lh_gx_rect_t *self, const lh_gx_rect_t *other)
{
    return LH_GX_RECT_FN(equals)(self, other);
}

lh_bool_t
lh_gx_rect_is_at_least(const lh_gx_rect_t *self, const lh_gx_rect_t *minimum)
{
    return LH_GX_RECT_FN(is_at_least)(self, minimum);
}

lh_bool_t
lh_gx_rect_is_less(const lh_gx_rect_t *self, const lh_gx_rect_t *other)
{
    return LH_GX_RECT_FN(is_less)(self, other);
}

lh_bool_t
lh_gx_rect_is_greater(const lh_gx_rect_t *self, const lh_gx_rect_t *other)
{
    return LH_GX_RECT_FN(is_greater)(self, other);
}

lh_void
lh_gx_rect_intersection(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_rect_t *out)
{
    LH_GX_RECT_FN(intersection)(a, b, out);
}

lh_void
lh_gx_rect_union(const lh_gx_rect_t *a, const lh_gx_rect_t *b, lh_gx_rect_t *out)
{
    LH_GX_RECT_FN(union)(a, b, out);
}

lh_void
lh_gx_rect_offset(const lh_gx_rect_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                  lh_gx_rect_t *out)
{
    LH_GX_RECT_FN(offset)(self, dx, dy, out);
}

lh_void
lh_gx_rect_inset(const lh_gx_rect_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                 lh_gx_rect_t *out)
{
    LH_GX_RECT_FN(inset)(self, dx, dy, out);
}
