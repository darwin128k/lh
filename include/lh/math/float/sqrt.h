/**
 * @file sqrt.h
 * @brief Single-precision square root without libm: ::lh_math_float_sqrt.
 *
 * GCC and Clang get `__builtin_sqrtf`, which becomes the CPU's own square root
 * instruction (SSE `sqrtss`, AArch64 `fsqrt`, the Cortex-M4F `vsqrt.f32`, ...).
 * The library builds its own source so that the compiler does not add a call to
 * libm's `sqrtf` only to set `errno` on negative input (see `src/lh/math/float/sqrt.c`);
 * the caller's own flags do not matter, because the call goes through this
 * function. MSVC treats `sqrtf` as an intrinsic and always has its CRT.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FPU.
 */

#ifndef LH_MATH_FLOAT_SQRT_H
#define LH_MATH_FLOAT_SQRT_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Square root of @p x.
 *
 * @param x Value; `0` gives `0`, a negative value gives NaN (`errno` is not set).
 * @return `sqrt(x)`, correctly rounded where the CPU's instruction is.
 */
lh_float_t
lh_math_float_sqrt(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_SQRT_H */
