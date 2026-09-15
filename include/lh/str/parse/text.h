/**
 * @file text.h
 * @brief scanf-style text parsing — portable, no libc/OS dependency
 *        (mask walked by ::lh_str_ptr_scanf_next; values via the same
 *        conversions as ::lh_str_ptr_format_text).
 *
 * Supported conversions: `%%`, `%c`, `%s`, `%d`, `%i`, `%u`, `%x`, `%X`.
 * Width is a maximum character count for that conversion. Precision (`.N`)
 * is accepted on `%s` as a max length, matching format. Padding flags `0`
 * and `-` are parsed (same walker as format) and ignored.
 *
 * `%s` reads up to the following literal in the mask (or the rest of the
 * input if the mask ends). Arguments: `%s` takes `lh_str_ptr` then
 * `lh_usize_t` capacity — the field must fit; no NUL is appended.
 */

#ifndef LH_STR_PARSE_TEXT_H
#define LH_STR_PARSE_TEXT_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse @p str against @p fmt (the `va_list` primitive;
 *        see ::lh_str_ptr_parse_text for the variadic wrapper).
 *
 * On failure — unknown conversion, malformed mask, or input that does not
 * match — 0 is returned. Out arguments written before the failure are not
 * rolled back.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param fmt      NUL-terminated format mask.
 * @param args     Destinations, one per conversion (`%s` is pointer then
 *                 capacity; integers are `lh_uint_t *` / `lh_sint_t *`;
 *                 `%c` is `lh_char_t *`).
 *
 * @return Characters consumed from @p str, or 0 on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_ptr_parse_text_v(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args);

/**
 * @brief Parse @p str against @p fmt and the following destinations.
 *
 * @return Characters consumed from @p str, or 0 on failure
 *         (see ::lh_str_ptr_parse_text_v).
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_ptr_parse_text(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_TEXT_H */
