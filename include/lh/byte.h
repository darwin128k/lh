/**
 * @file byte.h
 * @brief The 8-bit unsigned byte type: ::lh_byte_t.
 *
 * The semantic "one byte" name, layered on the numeric base: ::lh_byte_t is
 * the ::lh_u8_t alias from <lh/numeric/fixed/types.h>, which is in turn the
 * base 8-bit type from <lh/char.h>. Use this name for raw bytes and binary
 * data — buffers, memory, packed fields, hash blocks, color channels,
 * addresses; use ::lh_char_t for text and ::lh_schar_t for a character with
 * explicit signedness.
 *
 * Limits (::LH_BYTE_T_MIN, ::LH_BYTE_T_MAX, ::LH_BYTE_T_SIZE) live in
 * <lh/byte/limits.h>.
 */

#ifndef LH_BYTE_H
#define LH_BYTE_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_byte_t
 * @brief 8-bit unsigned integer (exactly 1 byte).
 *
 * Always unsigned, range: 0 to 255.
 * Suitable for raw bytes, binary data, and unsigned 8-bit arithmetic.
 *
 * Alias for: ::lh_u8_t (`unsigned char`).
 */
typedef lh_u8_t lh_byte_t;

#endif /* LH_BYTE_H */
