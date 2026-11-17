/**
 * @file point.c
 * @brief Implementation of `lh/gx/point.h`.
 *
 * Every function here is a thin layer over the chosen math point: it calls
 * that brick and adds nothing, so the contract of each one is the contract of
 * the math point it names.
 */

#include <lh/core/assert/runtime.h>
#include <lh/core/util/ptr.h>
#include <lh/gx/point.h>

lh_void
lh_gx_point_init(lh_gx_point_t *self, lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    LH_GX_POINT_FN(init)(self, x, y);
}

lh_void
lh_gx_point_init_empty(lh_gx_point_t *self)
{
    LH_GX_POINT_FN(init_empty)(self);
}

lh_gx_scalar_t
lh_gx_point_get_x(const lh_gx_point_t *self)
{
    return LH_GX_POINT_FN(get_x)(self);
}

lh_gx_scalar_t
lh_gx_point_get_y(const lh_gx_point_t *self)
{
    return LH_GX_POINT_FN(get_y)(self);
}

lh_void
lh_gx_point_set_x(lh_gx_point_t *self, lh_gx_scalar_t x)
{
    LH_GX_POINT_FN(set_x)(self, x);
}

lh_void
lh_gx_point_set_y(lh_gx_point_t *self, lh_gx_scalar_t y)
{
    LH_GX_POINT_FN(set_y)(self, y);
}

lh_void
lh_gx_point_to_point(const lh_gx_point_t *self, lh_math_point_t *out)
{
    lh_assert_runtime_ref(self);
#if LH_LIBRARY_OPTION_MATH_FPU
    lh_math_fpoint_to_point(*self, out);
#else
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = *self;
#endif
}

lh_void
lh_gx_point_from_point(const lh_math_point_t *self, lh_gx_point_t *out)
{
    lh_assert_runtime_ref(self);
#if LH_LIBRARY_OPTION_MATH_FPU
    lh_math_point_to_fpoint(*self, out);
#else
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = *self;
#endif
}

lh_bool_t
lh_gx_point_eq(const lh_gx_point_t *a, const lh_gx_point_t *b)
{
    return LH_GX_POINT_FN(eq)(a, b);
}

lh_bool_t
lh_gx_point_equals(const lh_gx_point_t *self, const lh_gx_point_t *other)
{
    return LH_GX_POINT_FN(equals)(self, other);
}

lh_bool_t
lh_gx_point_is_at_least(const lh_gx_point_t *self, const lh_gx_point_t *minimum)
{
    return LH_GX_POINT_FN(is_at_least)(self, minimum);
}

lh_bool_t
lh_gx_point_is_less(const lh_gx_point_t *self, const lh_gx_point_t *other)
{
    return LH_GX_POINT_FN(is_less)(self, other);
}

lh_bool_t
lh_gx_point_is_greater(const lh_gx_point_t *self, const lh_gx_point_t *other)
{
    return LH_GX_POINT_FN(is_greater)(self, other);
}

lh_void
lh_gx_point_min(const lh_gx_point_t *a, const lh_gx_point_t *b, lh_gx_point_t *out)
{
    LH_GX_POINT_FN(min)(a, b, out);
}

lh_void
lh_gx_point_max(const lh_gx_point_t *a, const lh_gx_point_t *b, lh_gx_point_t *out)
{
    LH_GX_POINT_FN(max)(a, b, out);
}

lh_bool_t
lh_gx_point_in_extent(const lh_gx_point_t *self, const lh_gx_point_t *min,
                      const lh_gx_point_t *max)
{
    return LH_GX_POINT_FN(in_extent)(self, min, max);
}

lh_void
lh_gx_point_offset(const lh_gx_point_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                   lh_gx_point_t *out)
{
    LH_GX_POINT_FN(offset)(self, dx, dy, out);
}
