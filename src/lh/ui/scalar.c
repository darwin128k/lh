/**
 * @file scalar.c
 * @brief Implementation of `lh/ui/scalar.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/scalar.h>

lh_void
lh_ui_scalar_trunc(lh_ui_scalar_t v, lh_ui_scalar_t *out)
{
    lh_assert_runtime_ref(out);
#if LH_LIBRARY_OPTION_MATH_FPU
    *out = lh_ui_scalar_of(lh_cast_static(lh_s32_t, v));
#else
    *out = v;
#endif
}
