/**
 * @file size.c
 * @brief Implementation of `lh/gx/size.h`.
 *
 * Every function here is a thin layer over the chosen math size: it calls
 * that brick and adds nothing, so the contract of each one is the contract of
 * the math size it names.
 */

#include <lh/gx/size.h>

lh_void
lh_gx_size_init(lh_gx_size_t *self, lh_gx_scalar_t width, lh_gx_scalar_t height)
{
    LH_GX_SIZE_FN(init)(self, width, height);
}

lh_void
lh_gx_size_init_empty(lh_gx_size_t *self)
{
    LH_GX_SIZE_FN(init_empty)(self);
}

lh_void
lh_gx_size_from_extent(const lh_gx_point_t *min, const lh_gx_point_t *max, lh_gx_size_t *out)
{
    LH_GX_SIZE_FN(from_extent)(min, max, out);
}

lh_gx_scalar_t
lh_gx_size_get_width(const lh_gx_size_t *self)
{
    return LH_GX_SIZE_FN(get_width)(self);
}

lh_gx_scalar_t
lh_gx_size_get_height(const lh_gx_size_t *self)
{
    return LH_GX_SIZE_FN(get_height)(self);
}

lh_void
lh_gx_size_set_width(lh_gx_size_t *self, lh_gx_scalar_t width)
{
    LH_GX_SIZE_FN(set_width)(self, width);
}

lh_void
lh_gx_size_set_height(lh_gx_size_t *self, lh_gx_scalar_t height)
{
    LH_GX_SIZE_FN(set_height)(self, height);
}

lh_void
lh_gx_size_to_size(const lh_gx_size_t *self, lh_math_size_t *out)
{
    lh_assert_runtime_ref(self);
#if LH_LIBRARY_OPTION_MATH_FPU
    lh_math_fsize_to_size(*self, out);
#else
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = *self;
#endif
}

lh_void
lh_gx_size_from_size(const lh_math_size_t *self, lh_gx_size_t *out)
{
    lh_assert_runtime_ref(self);
#if LH_LIBRARY_OPTION_MATH_FPU
    lh_math_size_to_fsize(*self, out);
#else
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = *self;
#endif
}

lh_bool_t
lh_gx_size_eq(const lh_gx_size_t *a, const lh_gx_size_t *b)
{
    return LH_GX_SIZE_FN(eq)(a, b);
}

lh_bool_t
lh_gx_size_equals(const lh_gx_size_t *self, const lh_gx_size_t *other)
{
    return LH_GX_SIZE_FN(equals)(self, other);
}

lh_bool_t
lh_gx_size_is_at_least(const lh_gx_size_t *self, const lh_gx_size_t *minimum)
{
    return LH_GX_SIZE_FN(is_at_least)(self, minimum);
}

lh_bool_t
lh_gx_size_is_less(const lh_gx_size_t *self, const lh_gx_size_t *other)
{
    return LH_GX_SIZE_FN(is_less)(self, other);
}

lh_bool_t
lh_gx_size_is_greater(const lh_gx_size_t *self, const lh_gx_size_t *other)
{
    return LH_GX_SIZE_FN(is_greater)(self, other);
}

lh_bool_t
lh_gx_size_is_empty(const lh_gx_size_t *self)
{
    return LH_GX_SIZE_FN(is_empty)(self);
}

lh_void
lh_gx_size_min(const lh_gx_size_t *a, const lh_gx_size_t *b, lh_gx_size_t *out)
{
    LH_GX_SIZE_FN(min)(a, b, out);
}

lh_void
lh_gx_size_max(const lh_gx_size_t *a, const lh_gx_size_t *b, lh_gx_size_t *out)
{
    LH_GX_SIZE_FN(max)(a, b, out);
}

lh_gx_scalar_t
lh_gx_size_min_side(const lh_gx_size_t *self)
{
    return LH_GX_SIZE_FN(min_side)(self);
}

lh_void
lh_gx_size_inset(const lh_gx_size_t *self, lh_gx_scalar_t dx, lh_gx_scalar_t dy,
                 lh_gx_size_t *out)
{
    LH_GX_SIZE_FN(inset)(self, dx, dy, out);
}
