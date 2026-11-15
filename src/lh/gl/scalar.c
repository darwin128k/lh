/**
 * @file scalar.c
 * @brief Implementation of `lh/gl/scalar.h`.
 *
 * Only the float path has an out-of-line ::lh_gl_scalar_trunc; the int path
 * is a force-inline copy in the header.
 */

#include <lh/core/math.h>
#include <lh/core/math/scalar/centre.h>
#include <lh/gl/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/core/assert/runtime.h>
#    include <lh/core/numeric/fixed/types.h>

lh_void
lh_gl_scalar_trunc(lh_gl_scalar_t v, lh_gl_scalar_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_gl_scalar_of(lh_cast_static(lh_s32_t, v));
}
#endif

lh_gl_scalar_t
lh_gl_scalar_slack(lh_gl_scalar_t room, lh_gl_scalar_t own)
{
    return lh_math_slack(room, own);
}

lh_gl_scalar_t
lh_gl_scalar_centre(lh_gl_scalar_t room, lh_gl_scalar_t own)
{
    lh_gl_scalar_t half;

    lh_gl_scalar_trunc(lh_math_centre_half(room, own), lh_addr_of(half));
    return lh_math_max(half, lh_gl_scalar(0));
}
