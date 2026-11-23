/**
 * @file expm1.h
 * @brief The exponential minus one, without libm: ::lh_math_float_expm1.
 *
 * `exp(x) - 1` loses the whole significance of a small @p x: the subtraction
 * cancels the leading digit. This brick keeps it, so a caller that works with
 * the difference (a growth rate, a decay factor) gets the digits of @p x and
 * not the digits of the rounding.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FLOAT.
 */

#ifndef LH_MATH_FLOAT_EXPM1_H
#define LH_MATH_FLOAT_EXPM1_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief `e^x - 1`, with the digits of a small @p x kept.
 *
 * @param x The exponent; any value, including a NaN.
 * @return `e^x - 1`: an infinity when the result does not fit in a float, `-1`
 *         for a very negative @p x, the NaN of @p x for a NaN.
 */
lh_float_t
lh_math_float_expm1(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_EXPM1_H */
