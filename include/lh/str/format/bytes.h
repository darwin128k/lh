/**
 * @file bytes.h
 * @brief Format a byte span as hexadecimal text.
 */

#ifndef LH_STR_FORMAT_BYTES_H
#define LH_STR_FORMAT_BYTES_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @def LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH
 * @brief Bytes per line of ::lh_str_ptr_format_bytes_hex_dump (16).
 */
#define LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH 16U

/**
 * @def LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS
 * @brief Zero-padded hex digits used for the dump line offset (8).
 */
#define LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS 8U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format @p data as contiguous hex digits — two characters per byte,
 *        no separators, no `0x` prefix, no NUL terminator appended.
 *
 * Empty @p data_size writes nothing and returns 0 (the same sentinel as a
 * too-small buffer; callers that need to tell the two apart already know
 * @p data_size).
 *
 * @param data      Byte span to format. Ignored when @p data_size is 0.
 * @param data_size Number of bytes in @p data.
 * @param uppercase ::lh_bool_true for `A`-`F`, ::lh_bool_false for `a`-`f`.
 * @param str       Destination buffer.
 * @param str_size  Capacity of @p str in characters.
 *
 * @return Characters written (`2 * data_size`), or 0 if @p str_size was too
 *         small (or @p data_size was 0).
 */
lh_usize_t
lh_str_ptr_format_bytes_hex(const lh_ptr data, lh_usize_t data_size, lh_bool_t uppercase,
                            lh_str_ptr str, lh_usize_t str_size);

/**
 * @brief Characters ::lh_str_ptr_format_bytes_hex_dump writes for
 *        @p data_size bytes — the capacity its destination needs.
 *
 * Example usage:
 * @code{.c}
 * lh_usize_t size = lh_str_ptr_format_bytes_hex_dump_size(data_size);
 * lh_str_ptr str = alloc(size);
 * lh_str_ptr_format_bytes_hex_dump(data, data_size, lh_bool_false, str, size);
 * @endcode
 *
 * @param data_size Number of bytes to dump.
 *
 * @return Size of the dump in characters (no terminator), or 0 if
 *         @p data_size is 0 or the dump would not fit in ::lh_usize_t.
 */
lh_usize_t
lh_str_ptr_format_bytes_hex_dump_size(lh_usize_t data_size);

/**
 * @brief Format @p data as a classic hex dump — offset, then grouped bytes.
 *
 * Each line is `hhhhhhhh: xx xx ... xx\\n` with a
 * ::LH_STR_FORMAT_BYTES_HEX_DUMP_OFFSET_DIGITS-digit offset and up to
 * ::LH_STR_FORMAT_BYTES_HEX_DUMP_WIDTH bytes. All-or-nothing: nothing is
 * written if @p str_size cannot hold the full dump
 * (::lh_str_ptr_format_bytes_hex_dump_size).
 *
 * @param data      Byte span to format. Ignored when @p data_size is 0.
 * @param data_size Number of bytes in @p data.
 * @param uppercase ::lh_bool_true for `A`-`F`, ::lh_bool_false for `a`-`f`.
 * @param str       Destination buffer.
 * @param str_size  Capacity of @p str in characters.
 *
 * @return Characters written, or 0 if @p str_size was too small (or
 *         @p data_size was 0).
 */
lh_usize_t
lh_str_ptr_format_bytes_hex_dump(const lh_ptr data, lh_usize_t data_size, lh_bool_t uppercase,
                                 lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_BYTES_H */
