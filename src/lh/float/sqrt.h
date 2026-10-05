/**
 * @file sqrt.h
 * @brief Library-private: single-precision square root without libm.
 *
 * GCC and Clang get `__builtin_sqrtf`, which becomes the CPU's own square
 * root instruction (SSE `sqrtss`, AArch64 `fsqrt`, ...) as long as the
 * calling file is built with `-fno-math-errno` (CMakeLists.txt sets it for the
 * files including this header): otherwise the compiler keeps a call to libm's
 * `sqrtf` for setting `errno` on negative input, and a freestanding build
 * has no libm. MSVC treats `sqrtf` as an intrinsic and always has its CRT.
 */

#ifndef LH_SRC_FLOAT_SQRT_H
#define LH_SRC_FLOAT_SQRT_H

#include <lh/attribute/static.h>
#include <lh/compiler/type.h>
#include <lh/numeric/float.h>

#if !LH_COMPILER_TYPE_IS_GCC_LIKE
#    include <math.h>
#endif

LH_ATTRIBUTE_STATIC
lh_float_t
lh_float_sqrt(lh_float_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return __builtin_sqrtf(x);
#else
    return sqrtf(x);
#endif
}

#endif /* LH_SRC_FLOAT_SQRT_H */
