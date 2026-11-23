/**
 * @file log.h
 * @brief The natural logarithm without libm: ::lh_math_float_log.
 *
 * The number is taken apart into its exponent and its mantissa (the bits of
 * the format, ::lh/core/math/float/bits.h): the exponent gives the whole
 * turns of ln(2), and the mantissa -- between one and two -- is answered by a
 * series that converges fast there. Nothing here calls libm.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FLOAT.
 */

#ifndef LH_MATH_FLOAT_LOG_H
#define LH_MATH_FLOAT_LOG_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The natural logarithm of @p x.
 *
 * @param x The number; any value, including zero, a negative one and a NaN.
 * @return `ln(x)`: a NaN for a negative @p x, minus an infinity for zero, the
 *         NaN of @p x for a NaN.
 */
lh_float_t
lh_math_float_log(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_LOG_H */
