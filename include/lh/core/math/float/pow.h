/**
 * @file pow.h
 * @brief The power without libm: ::lh_math_float_pow.
 *
 * A whole exponent is answered by repeated squaring, so the answer keeps the
 * digits of the base; a fractional one goes through ::lh_math_float_log and
 * ::lh_math_float_exp. A negative base with a fractional exponent has no
 * answer, and the brick says so with a NaN.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FLOAT.
 */

#ifndef LH_MATH_FLOAT_POW_H
#define LH_MATH_FLOAT_POW_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p x raised to the power @p y.
 *
 * @param x The base; any value, including a negative one and a zero.
 * @param y The exponent; any value, including a fractional one.
 * @return `x^y`: one for a zero @p y, zero for a zero @p x and a positive
 *         @p y, an infinity for a zero @p x and a negative @p y, a NaN for a
 *         negative @p x and a fractional @p y.
 */
lh_float_t
lh_math_float_pow(lh_float_t x, lh_float_t y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_POW_H */
