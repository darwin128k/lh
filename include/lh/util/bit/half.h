/**
 * @file half.h
 * @brief Join a 64-bit word from two 32-bit halves, and split it back.
 *
 * Operating systems and wire formats hand 64-bit values over as a high and
 * a low 32-bit half (Win32 `FILETIME`, `LARGE_INTEGER` parts, big/little
 * endian packing). Writing `((lh_u64_t)high << 32) | low` by hand at every
 * such place is the kind of small primitive this library names once — see
 * `lh/util/bit/rotate.h` for the same reasoning.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE: each is one or two
 * machine instructions, and a real cross-TU call would cost more than the
 * operation itself.
 */

#ifndef LH_UTIL_BIT_HALF_H
#define LH_UTIL_BIT_HALF_H

#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/math.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Join a 64-bit word from its @p high and @p low 32-bit halves.
 *
 * Inverse of ::lh_bit_get_high_u32 / ::lh_bit_get_low_u32.
 *
 * Example usage:
 * @code{.c}
 * lh_u64_t x = lh_bit_make_u64(0x01234567u, 0x89ABCDEFu); // 0x0123456789ABCDEF
 * @endcode
 *
 * @param high Bits 63..32 of the result.
 * @param low  Bits 31..0 of the result.
 * @return `high:low` as one 64-bit word.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_bit_make_u64(lh_u32_t high, lh_u32_t low)
{
    return lh_math_bit_or(lh_math_bit_shl(lh_cast_static(lh_u64_t, high), 32),
                          lh_cast_static(lh_u64_t, low));
}

/**
 * @brief The high 32-bit half (bits 63..32) of @p x.
 *
 * @param x 64-bit word.
 * @return Bits 63..32 of @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_get_high_u32(lh_u64_t x)
{
    return lh_cast_static(lh_u32_t, lh_math_bit_shr(x, 32));
}

/**
 * @brief The low 32-bit half (bits 31..0) of @p x.
 *
 * @param x 64-bit word.
 * @return Bits 31..0 of @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_get_low_u32(lh_u64_t x)
{
    return lh_cast_static(lh_u32_t, x);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_BIT_HALF_H */
