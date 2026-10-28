/**
 * @file tan.h
 * @brief Single-precision tangent without libm: ::lh_math_float_tan.
 *
 * Composed of ::lh_math_float_sin_cos: the sine over the cosine of the same
 * angle, so it shares that brick's accuracy and range.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FPU.
 */

#ifndef LH_MATH_FLOAT_TAN_H
#define LH_MATH_FLOAT_TAN_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Tangent of @p x.
 *
 * @param x Angle in radians, accurate for `|x|` up to about 1e5.
 * @return `sin(x) / cos(x)`; an infinity or a huge value where the cosine is
 *         zero or nearly so (odd multiples of pi/2).
 */
lh_float_t
lh_math_float_tan(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_TAN_H */
