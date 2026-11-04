/**
 * @file sqrt.c
 * @brief Implementation of `lh/core/math/float/sqrt.h`.
 *
 * Built with `-fno-math-errno` on GCC / Clang (CMakeLists.txt): without it the
 * builtin keeps a call to libm's `sqrtf` beside the CPU instruction.
 */

#include <lh/core/compiler/type.h>
#include <lh/core/math/float/sqrt.h>

#if !LH_COMPILER_TYPE_IS_GCC_LIKE
#    include <math.h>
#endif

lh_float_t
lh_math_float_sqrt(lh_float_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return __builtin_sqrtf(x);
#else
    return sqrtf(x);
#endif
}
