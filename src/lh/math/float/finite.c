/**
 * @file finite.c
 * @brief Implementation of `lh/math/float/finite.h`.
 */

#include <lh/compiler/type.h>
#include <lh/math/float/finite.h>

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
