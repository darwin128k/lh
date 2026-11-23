/**
 * @file exp.h
 * @brief The exponential without libm: ::lh_math_float_exp.
 *
 * The argument is cut into a whole number of turns of ln(2) and a remainder
 * below half a turn; the remainder is a series that converges fast there, and
 * the whole part is a power of two, built from the bits of the format
 * (::lh/core/math/float/bits.h). Nothing here calls libm.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FLOAT.
 */

#ifndef LH_MATH_FLOAT_EXP_H
#define LH_MATH_FLOAT_EXP_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief e raised to the power @p x.
 *
 * @param x The exponent; any value, including a NaN.
 * @return `e^x`: an infinity when the result does not fit in a float, zero when
 *         it is smaller than the smallest float, the NaN of @p x for a NaN.
 */
lh_float_t
lh_math_float_exp(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_EXP_H */
