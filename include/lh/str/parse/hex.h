/**
 * @file hex.h
 * @brief Parse an unsigned hexadecimal integer from text.
 */

#ifndef LH_STR_PARSE_HEX_H
#define LH_STR_PARSE_HEX_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse an unsigned hexadecimal integer, rejecting values above @p max.
 *
 * Accepts ASCII digits `0`-`9` and letters `a`-`f` / `A`-`F`; all of
 * @p str_size must be consumed. Rejects empty input. Unlike
 * ::lh_str_ptr_parse_uint, leading zeros are allowed — padded hex (`00FF`)
 * is the usual written form. No `0x` prefix.
 *
 * Safe against ::lh_uint_t overflow regardless of @p max. Digit fold lives
 * in `lh/char/xdigit.h`.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a hexadecimal
 *         integer not exceeding @p max, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_str_ptr_parse_hex(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_HEX_H */
