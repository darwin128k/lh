/**
 * @file scalar.c
 * @brief Implementation of `lh/ui/scalar.h`.
 *
 * Only the float path has an out-of-line ::lh_ui_scalar_trunc; the int path
 * is a force-inline copy in the header.
 */

#include <lh/math.h>
#include <lh/math/scalar/centre.h>
#include <lh/ui/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/assert/runtime.h>
#    include <lh/numeric/fixed/types.h>

lh_void
lh_ui_scalar_trunc(lh_ui_scalar_t v, lh_ui_scalar_t *out)
{
    lh_assert_runtime_ref(out);
    *out = lh_ui_scalar_of(lh_cast_static(lh_s32_t, v));
}
#endif

lh_ui_scalar_t
lh_ui_scalar_slack(lh_ui_scalar_t room, lh_ui_scalar_t own)
{
    return lh_math_slack(room, own);
}

lh_ui_scalar_t
lh_ui_scalar_centre(lh_ui_scalar_t room, lh_ui_scalar_t own)
{
    lh_ui_scalar_t half;

    lh_ui_scalar_trunc(lh_math_centre_half(room, own), lh_addr_of(half));
    return lh_math_max(half, lh_ui_scalar(0));
}
