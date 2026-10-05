/**
 * @file isqrt.h
 * @brief The exact integer square root.
 */

#ifndef LH_MATH_ISQRT_H
#define LH_MATH_ISQRT_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/util/numeric.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The largest integer whose square is not greater than @p value, or 0
 *        for @p value 0 and below.
 *
 * A rounded shape asks how far a pixel is from its rim, and the answer decides
 * whether the pixel is painted, so "nearly right" is not available: a pixel one
 * side of the rim is inside it and a pixel on the other side is not. The float
 * square root says where the answer is and this closes the gap exactly, which
 * is the whole reason it is a function of its own rather than a call through.
 *
 * Newton from the value itself divides its way down by halves, which for a
 * squared distance is a dozen 64-bit divisions per pixel; this is one root and a
 * couple of multiplies.
 *
 * @param value The square to root, at most ::LH_MATH_ISQRT_MAX.
 */
lh_int_t
lh_math_isqrt(lh_sllong_t value);

/**
 * @def LH_MATH_ISQRT_MAX
 * @brief The largest @p value ::lh_math_isqrt answers for.
 *
 * The square of the largest ::lh_int_t, and not a round number by accident: the
 * answer has to fit in what it is returned as, and one past this root the
 * product that corrects the float root would stop fitting as well. A squared
 * distance a screen asks about is many orders of magnitude below it.
 */
#define LH_MATH_ISQRT_MAX                                                                        \
    ((lh_sllong_t)lh_numeric_limit_smax(lh_int_t) *                                             \
     (lh_sllong_t)lh_numeric_limit_smax(lh_int_t))

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_ISQRT_H */
