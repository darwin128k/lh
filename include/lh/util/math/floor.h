/**
 * @file floor.h
 * @brief Floor division and modulo on ::lh_s64_t.
 *
 * ::lh_math_div / ::lh_math_mod (`lh/util/math.h`) are plain wrappers around
 * `/` and `%`, which truncate toward zero - the wrong rounding for anything
 * indexed by whole units relative to an origin that can go negative (day
 * offsets before an epoch, seconds-of-day for a negative timestamp, etc).
 * ::lh_math_floor_div / ::lh_math_floor_mod round toward negative infinity
 * instead, so the remainder always has the sign of the divisor.
 *
 * Force-inline (like `lh/util/interval/wrap.h`): defined once here, no `.c`
 * file, so this stays a shared primitive instead of every caller hand-rolling
 * (or duplicating) the same truncate-then-adjust logic.
 */

#ifndef LH_UTIL_MATH_FLOOR_H
#define LH_UTIL_MATH_FLOOR_H

#include <lh/attribute/force_inline.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/math.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Floor division: rounds toward negative infinity, unlike ::lh_math_div.
 *
 * @param a Dividend.
 * @param b Divisor (nonzero, any sign).
 * @return `floor(a / b)`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_s64_t
lh_math_floor_div(lh_s64_t a, lh_s64_t b)
{
    const lh_s64_t q = lh_math_div(a, b);
    const lh_s64_t r = lh_math_mod(a, b);

    return (lh_math_ne(r, 0) && lh_math_ne(lh_math_is_negative(r), lh_math_is_negative(b)))
               ? lh_math_sub_one(q)
               : q;
}

/**
 * @brief Floor modulo: always has the sign of @p b, unlike ::lh_math_mod.
 *
 * @param a Dividend.
 * @param b Divisor (nonzero, any sign).
 * @return `a - b * lh_math_floor_div(a, b)`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_s64_t
lh_math_floor_mod(lh_s64_t a, lh_s64_t b)
{
    return lh_math_sub(a, lh_math_mul(b, lh_math_floor_div(a, b)));
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_MATH_FLOOR_H */
