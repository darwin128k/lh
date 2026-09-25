/**
 * @file hex.h
 * @brief Format an unsigned integer as hexadecimal text.
 */

#ifndef LH_STR_FORMAT_HEX_H
#define LH_STR_FORMAT_HEX_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_HEX_H */
