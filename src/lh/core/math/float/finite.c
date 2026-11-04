/**
 * @file finite.c
 * @brief Implementation of `lh/core/math/float/finite.h`.
 */

#include <lh/core/compiler/type.h>
#include <lh/core/math/float/finite.h>

#if !LH_COMPILER_TYPE_IS_GCC_LIKE
#    include <math.h>
#endif

lh_bool_t
lh_math_float_is_finite(lh_float_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return __builtin_isfinite(x) ? lh_bool_true : lh_bool_false;
#else
    return isfinite(x) ? lh_bool_true : lh_bool_false;
#endif
}
