/**
 * @file rescale.h
 * @brief Map an unsigned value from `0..from_max` onto `0..to_max`, rounded.
 *
 * A 4-bit glyph sample (0..15) to an 8-bit coverage (0..255), a percentage
 * to a byte, a byte to a track length: `value * to_max / from_max`, rounded
 * to nearest instead of truncated, so both ends map exactly.
 *
 * Force-inline (like `lh/math/floor.h`): defined once here, no `.c` file.
 */

#ifndef LH_MATH_RESCALE_H
#define LH_MATH_RESCALE_H

#include <lh/attribute/force_inline.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p value in `0..from_max` mapped onto `0..to_max`, rounded to
 *        nearest: `(value * to_max + from_max / 2) / from_max`.
 *
 * @param value    Value to map, at most @p from_max.
 * @param from_max Top of the source range (nonzero).
 * @param to_max   Top of the target range; `value * to_max` must fit 32 bits.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_math_rescale_u32(lh_u32_t value, lh_u32_t from_max, lh_u32_t to_max)
{
    return (value * to_max + from_max / 2U) / from_max;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_RESCALE_H */
