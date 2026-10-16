/**
 * @file div_round.h
 * @brief Integer division rounded to nearest (half up for non-negative values).
 *
 * `(numer + denom / 2) / denom`: the same rounding ::lh_math_rescale_u32 uses
 * inside its map. Prefer this over open-coding the half-add at every blur /
 * coverage / fade site.
 *
 * Force-inline (like `lh/math/floor.h`): no `.c` file. Defined for non-negative
 * numerators and a positive denominator; negative inputs keep C's toward-zero
 * truncating division after the half-add and are not "nearest".
 *
 * Not pulled in by `lh/math.h` (that header's operator macros would cycle the
 * same way ::lh_math_floor_div would); include this where the round is needed.
 */

#ifndef LH_MATH_DIV_ROUND_H
#define LH_MATH_DIV_ROUND_H

#include <lh/attribute/force_inline.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @def lh_math_div_round(numer, denom)
 * @brief Same half-up quotient as ::lh_math_div_round_u32, for any arithmetic
 *        type: `(numer + denom / 2) / denom`.
 *
 * @p numer and @p denom are each evaluated more than once — do not pass
 * expressions with side effects.
 */
#define lh_math_div_round(numer, denom) (((numer) + ((denom) / 2)) / (denom))

/**
 * @brief `round(numer / denom)` for non-negative @p numer and positive @p denom:
 *        `(numer + denom / 2) / denom`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_math_div_round_u32(lh_u32_t numer, lh_u32_t denom)
{
    return lh_math_div_round(numer, denom);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_DIV_ROUND_H */
