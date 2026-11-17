/**
 * @file scalar.c
 * @brief Implementation of `lh/gx/scalar.h`.
 *
 * Only the float path has an out-of-line ::lh_gx_scalar_trunc; the int path
 * is a force-inline copy in the header.
 */

#include <lh/core/math.h>
#include <lh/core/math/scalar/centre.h>
#include <lh/gx/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/core/assert/runtime.h>
#    include <lh/core/numeric/fixed/types.h>

lh_void
lh_gx_scalar_trunc(lh_gx_scalar_t v, lh_gx_scalar_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_gx_scalar_of(lh_cast_static(lh_s32_t, v));
}
#endif

lh_gx_scalar_t
lh_gx_scalar_slack(lh_gx_scalar_t room, lh_gx_scalar_t own)
{
    return lh_math_slack(room, own);
}

lh_gx_scalar_t
lh_gx_scalar_centre(lh_gx_scalar_t room, lh_gx_scalar_t own)
{
    lh_gx_scalar_t half;

    lh_gx_scalar_trunc(lh_math_centre_half(room, own), lh_addr_of(half));
    return lh_math_max(half, lh_gx_scalar(0));
}
