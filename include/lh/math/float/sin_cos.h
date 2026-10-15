/**
 * @file sin_cos.h
 * @brief Single-precision sine and cosine without libm: ::lh_math_float_sin_cos.
 *
 * `__builtin_sinf` / `__builtin_cosf` are not instructions on any target lh
 * builds for: the compiler emits calls to libm, which a freestanding build
 * does not have. So lh computes both itself, in float only:
 *
 * 1. Range reduction: `x = k * pi/2 + r` with `|r| <= pi/4`. `pi/2` is split
 *    into three floats (Cody-Waite) so `x - k * pi/2` loses no precision for
 *    `|k|` up to 2^16, i.e. `|x|` up to about 1e5 radians; beyond that the
 *    result degrades. Angles in a game or UI stay far below that.
 * 2. On `[-pi/4, pi/4]`, the minimax polynomials of Cephes' `sinf` / `cosf`
 *    (error about 1 ulp).
 * 3. The quadrant `k mod 4` picks which of `sin r`, `cos r` and their
 *    negations is the answer.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FPU.
 */

#ifndef LH_MATH_FLOAT_SIN_COS_H
#define LH_MATH_FLOAT_SIN_COS_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Both `sin(x)` and `cos(x)` of @p x radians, sharing one range
 *        reduction.
 *
 * @param x       Angle in radians, accurate for `|x|` up to about 1e5.
 * @param sin_out Receives `sin(x)`.
 * @param cos_out Receives `cos(x)`.
 */
lh_void
lh_math_float_sin_cos(lh_float_t x, lh_float_t *sin_out, lh_float_t *cos_out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_SIN_COS_H */
