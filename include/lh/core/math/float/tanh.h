/**
 * @file tanh.h
 * @brief The hyperbolic tangent without libm: ::lh_math_float_tanh.
 *
 * Composed of ::lh_math_float_expm1: the tangent of the hyperbola is the
 * difference of the exponential and one over the sum of the exponential and
 * one, and the difference is taken before the sum, so a small argument keeps
 * its digits.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FLOAT.
 */

#ifndef LH_MATH_FLOAT_TANH_H
#define LH_MATH_FLOAT_TANH_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The hyperbolic tangent of @p x.
 *
 * @param x The argument; any value, including a NaN.
 * @return `tanh(x)`: one (or minus one) for a large @p x, the NaN of @p x for
 *         a NaN.
 */
lh_float_t
lh_math_float_tanh(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_TANH_H */
