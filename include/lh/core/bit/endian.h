/**
 * @file endian.h
 * @brief Pack/unpack an integer to/from big-endian or little-endian bytes.
 *
 * Writing `value >> 8` / `value & 0xFF` by hand at every call site that needs
 * to hand a 16-bit field to a wire format (a socket address's port today; any
 * future length-prefixed or fixed-header protocol tomorrow) is exactly the
 * kind of small, easy-to-subtly-reimplement primitive this library prefers to
 * name once — see `lh/core/bit/scan.h` for the same reasoning applied to
 * bit-scan.
 *
 * Named `pack`/`unpack` for the wire-byte conversion itself
 * (integer value <-> serialized bytes), not `to_be16`/`from_be16`.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE: a two-or-four-shift
 * sequence like this is exactly the kind of tiny, hot operation a real
 * cross-TU call would cost more than.
 */

#ifndef LH_BIT_ENDIAN_H
#define LH_BIT_ENDIAN_H

#include <lh/core/assert/runtime.h>
#include <lh/core/bit/half.h>
#include <lh/core/byte/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write @p value into @p out as two big-endian bytes.
 *
 * @param value Value to pack.
 * @param out   Destination for exactly 2 bytes (`out[0]` is the most
 *              significant byte).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_pack_be16(lh_u16_t value, lh_byte_ptr out)
{
    lh_assert_runtime_ref(out);
    out[0] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 8));
    out[1] = lh_cast_static(lh_byte_t, lh_bit_and(value, 0xFFU));
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
lh_bit_unpack_be16(const lh_byte_ptr in)
{
    lh_assert_runtime_ref(in);
    return lh_cast_static(lh_u16_t,
                          lh_bit_or(lh_bit_shl(lh_cast_static(lh_u16_t, in[0]), 8), in[1]));
}

/**
 * @brief Write @p value into @p out as two little-endian bytes.
 *
 * @param value Value to pack.
 * @param out   Destination for exactly 2 bytes (`out[0]` is the least
 *              significant byte).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_pack_le16(lh_u16_t value, lh_byte_ptr out)
{
    lh_assert_runtime_ref(out);
    out[0] = lh_cast_static(lh_byte_t, value);
    out[1] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 8));
}

/**
 * @brief Read two little-endian bytes back into a value.
 *
 * Inverse of ::lh_bit_pack_le16.
 *
 * @param in Source of exactly 2 bytes (`in[0]` is the least significant byte).
 * @return The packed value.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u16_t
lh_bit_unpack_le16(const lh_byte_ptr in)
{
    lh_assert_runtime_ref(in);
    return lh_cast_static(lh_u16_t,
                          lh_bit_or(in[0], lh_bit_shl(lh_cast_static(lh_u16_t, in[1]), 8)));
}

/**
 * @brief Read three little-endian bytes into the low 24 bits of a value.
 *
 * For a 24-bit field inside a longer encoding, where a 32-bit read would run
 * past the end (the last limb of a curve25519 field element).
 *
 * @param in Source of exactly 3 bytes (`in[0]` is the least significant byte).
 * @return The value, below 2^24.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_unpack_le24(const lh_byte_ptr in)
{
    lh_assert_runtime_ref(in);
    return lh_bit_or(
        lh_bit_or(lh_cast_static(lh_u32_t, in[0]), lh_bit_shl(lh_cast_static(lh_u32_t, in[1]), 8)),
        lh_bit_shl(lh_cast_static(lh_u32_t, in[2]), 16));
}

/**
 * @brief Write @p value into @p out as four big-endian bytes.
 *
 * @param value Value to pack.
 * @param out   Destination for exactly 4 bytes (`out[0]` is the most
 *              significant byte).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_pack_be32(lh_u32_t value, lh_byte_ptr out)
{
    lh_assert_runtime_ref(out);
    out[0] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 24));
    out[1] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 16));
    out[2] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 8));
    out[3] = lh_cast_static(lh_byte_t, value);
}

/**
 * @brief Read four big-endian bytes back into a value.
 *
 * Inverse of ::lh_bit_pack_be32.
 *
 * @param in Source of exactly 4 bytes (`in[0]` is the most significant byte).
 * @return The packed value.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_unpack_be32(const lh_byte_ptr in)
{
    lh_assert_runtime_ref(in);
    return lh_bit_or(lh_bit_or(lh_bit_shl(lh_cast_static(lh_u32_t, in[0]), 24),
                              lh_bit_shl(lh_cast_static(lh_u32_t, in[1]), 16)),
                     lh_bit_or(lh_bit_shl(lh_cast_static(lh_u32_t, in[2]), 8),
                               lh_cast_static(lh_u32_t, in[3])));
}

/**
 * @brief Write @p value into @p out as four little-endian bytes.
 *
 * @param value Value to pack.
 * @param out   Destination for exactly 4 bytes (`out[0]` is the least
 *              significant byte).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_pack_le32(lh_u32_t value, lh_byte_ptr out)
{
    lh_assert_runtime_ref(out);
    out[0] = lh_cast_static(lh_byte_t, value);
    out[1] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 8));
    out[2] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 16));
    out[3] = lh_cast_static(lh_byte_t, lh_bit_shr(value, 24));
}

/**
 * @brief Read four little-endian bytes back into a value.
 *
 * Inverse of ::lh_bit_pack_le32.
 *
 * @param in Source of exactly 4 bytes (`in[0]` is the least significant byte).
 * @return The packed value.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_unpack_le32(const lh_byte_ptr in)
{
    lh_assert_runtime_ref(in);
    return lh_bit_or(lh_bit_or(lh_cast_static(lh_u32_t, in[0]),
                              lh_bit_shl(lh_cast_static(lh_u32_t, in[1]), 8)),
                     lh_bit_or(lh_bit_shl(lh_cast_static(lh_u32_t, in[2]), 16),
                               lh_bit_shl(lh_cast_static(lh_u32_t, in[3]), 24)));
}

/**
 * @brief Write @p value into @p out as eight big-endian bytes.
 *
 * @param value Value to pack.
 * @param out   Destination for exactly 8 bytes (`out[0]` is the most
 *              significant byte).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_pack_be64(lh_u64_t value, lh_byte_ptr out)
{
    lh_assert_runtime_ref(out);
    lh_bit_pack_be32(lh_bit_get_high_u32(value), out);
    lh_bit_pack_be32(lh_bit_get_low_u32(value), out + 4);
}

/**
 * @brief Read eight big-endian bytes back into a value.
 *
 * Inverse of ::lh_bit_pack_be64.
 *
 * @param in Source of exactly 8 bytes (`in[0]` is the most significant byte).
 * @return The packed value.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_bit_unpack_be64(const lh_byte_ptr in)
{
    lh_assert_runtime_ref(in);
    return lh_bit_make_u64(lh_bit_unpack_be32(in), lh_bit_unpack_be32(in + 4));
}

/**
 * @brief Write @p value into @p out as eight little-endian bytes.
 *
 * @param value Value to pack.
 * @param out   Destination for exactly 8 bytes (`out[0]` is the least
 *              significant byte).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_pack_le64(lh_u64_t value, lh_byte_ptr out)
{
    lh_assert_runtime_ref(out);
    lh_bit_pack_le32(lh_bit_get_low_u32(value), out);
    lh_bit_pack_le32(lh_bit_get_high_u32(value), out + 4);
}

/**
 * @brief Read eight little-endian bytes back into a value.
 *
 * Inverse of ::lh_bit_pack_le64.
 *
 * @param in Source of exactly 8 bytes (`in[0]` is the least significant byte).
 * @return The packed value.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_bit_unpack_le64(const lh_byte_ptr in)
{
    lh_assert_runtime_ref(in);
    return lh_bit_make_u64(lh_bit_unpack_le32(in + 4), lh_bit_unpack_le32(in));
}

/**
 * @brief Read @p count little-endian 32-bit words from @p in, the first word from
 *        the first four bytes.
 *
 * A big number kept as 32-bit limbs, lowest first (a scalar modulo L).
 *
 * @param out   Destination of @p count words; must not be null.
 * @param in    Source of 4 * @p count bytes; must not be null.
 * @param count Words to read.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_unpack_le32_words(lh_u32_t *out, const lh_byte_ptr in, lh_usize_t count)
{
    lh_assert_runtime_ref(out);
    for (lh_usize_t i = 0U; i < count; i = lh_math_add_one(i))
    {
        out[i] = lh_bit_unpack_le32(in + i * 4U);
    }
}

/**
 * @brief Write @p count 32-bit words into @p out as little-endian bytes, the first
 *        word into the first four bytes. Inverse of ::lh_bit_unpack_le32_words.
 *
 * @param words Source of @p count words; must not be null.
 * @param count Words to write.
 * @param out   Destination of 4 * @p count bytes; must not be null.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_bit_pack_le32_words(const lh_u32_t *words, lh_usize_t count, lh_byte_ptr out)
{
    lh_assert_runtime_ref(words);
    for (lh_usize_t i = 0U; i < count; i = lh_math_add_one(i))
    {
        lh_bit_pack_le32(words[i], out + i * 4U);
    }
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_BIT_ENDIAN_H */
