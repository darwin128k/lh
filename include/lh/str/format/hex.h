/**
 * @file hex.h
 * @brief Format an unsigned integer as hexadecimal text.
 */

#ifndef LH_STR_FORMAT_HEX_H
#define LH_STR_FORMAT_HEX_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/util/byte/ptr.h>

/**
 * @def LH_STR_FORMAT_HEX_RADIX
 * @brief Base ::lh_str_ptr_format_hex works with (16).
 *
 * The one named spot for the `16` that would otherwise show up as a bare
 * literal everywhere a hex digit is extracted (divide/modulo).
 */
#define LH_STR_FORMAT_HEX_RADIX 16U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format @p value as hexadecimal text — no `0x` prefix, no leading
 *        zeros, no NUL terminator appended.
 *
 * @param value     Value to format.
 * @param uppercase ::lh_bool_true for `A`-`F`, ::lh_bool_false for `a`-`f`.
 * @param str       Destination buffer.
 * @param str_size  Capacity of @p str in characters.
 *
 * @return Characters written (at least 1, since `0` itself takes one digit),
 *         or 0 if @p str_size was too small to hold the result.
 */
lh_usize_t
lh_str_ptr_format_hex(lh_uint_t value, lh_bool_t uppercase, lh_str_ptr str, lh_usize_t str_size);

/**
 * @brief Write @p size bytes as hexadecimal text — two characters per byte,
 *        high nibble first — into @p str.
 *
 * A buffer too small for the whole text takes the part that fits and stops.
 * The cut is at a byte boundary: a room too small for a whole byte is not
 * used. What was written is NUL-terminated whenever @p str has room for the
 * terminator.
 *
 * Example usage:
 * @code{.c}
 * lh_char_t text[8];
 * lh_usize_t n = lh_str_ptr_format_hex_bytes(bytes, 3, text, sizeof(text), lh_bool_false);
 * // n == 6, text == "0a1b2c"
 * @endcode
 *
 * @param bytes     Bytes to write; may be ::lh_null when @p size is 0.
 * @param size      Number of bytes to write.
 * @param str       Destination buffer.
 * @param str_size  Capacity of @p str in characters, the NUL included.
 * @param uppercase ::lh_bool_true for `A`-`F`, ::lh_bool_false for `a`-`f`.
 *
 * @return Characters written, the NUL not counted; 0 when @p bytes is
 *         ::lh_null, @p size is 0, or @p str is ::lh_null or has no room.
 */
lh_usize_t
lh_str_ptr_format_hex_bytes(const lh_byte_ptr bytes, lh_usize_t size, lh_str_ptr str,
                            lh_usize_t str_size, lh_bool_t uppercase);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_HEX_H */
