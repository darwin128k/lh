/**
 * @file endian.h
 * @brief Pack/unpack an integer to/from big-endian ("network order") bytes.
 *
 * Writing `value >> 8` / `value & 0xFF` by hand at every call site that needs
 * to hand a 16-bit field to a wire format (a socket address's port today; any
 * future length-prefixed or fixed-header protocol tomorrow) is exactly the
 * kind of small, easy-to-subtly-reimplement primitive this library prefers to
 * name once — see `lh/util/bit/scan.h` for the same reasoning applied to
 * bit-scan.
 *
 * Named `pack`/`unpack` to match this project's own established shape for
 * "value <-> its serialized bytes" (::lh_net_ip4_pack, ::lh_io_reader_pack,
 * ...), not `to_be16`/`from_be16` or similar.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE: a two-or-four-shift
 * sequence like this is exactly the kind of tiny, hot operation a real
 * cross-TU call would cost more than.
 */

#ifndef LH_UTIL_BIT_ENDIAN_H
#define LH_UTIL_BIT_ENDIAN_H

#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write @p value into @p out as two big-endian bytes.
 *
 * @param value Value to pack.
 * @param out   Destination for exactly 2 bytes (`out[0]` is the most
 *              significant byte).
 */
LH_ATTRIBUTE_FORCE_INLINE
void
lh_bit_pack_be16(lh_u16_t value, lh_uchar_t *out)
{
    lh_assert_runtime_ref(out);
    out[0] = lh_cast_static(lh_uchar_t, value >> 8);
    out[1] = lh_cast_static(lh_uchar_t, value & 0xFFU);
}

/**
 * @brief Read two big-endian bytes back into a value.
 *
 * Inverse of ::lh_bit_pack_be16.
 *
 * @param in Source of exactly 2 bytes (`in[0]` is the most significant byte).
 * @return The packed value.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u16_t
lh_bit_unpack_be16(const lh_uchar_t *in)
{
    lh_assert_runtime_ref(in);
    return lh_cast_static(lh_u16_t, (lh_cast_static(lh_u16_t, in[0]) << 8) | in[1]);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_BIT_ENDIAN_H */
