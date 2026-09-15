/**
 * @file text.h
 * @brief Inverse of ::lh_str_ptr_format_text — portable, no libc/OS
 *        dependency (built on ::lh_str_ptr_parse_uint / ::lh_str_ptr_parse_hex).
 *
 * Walks @p fmt and pulls matching fragments out of a sized buffer into
 * `va_list` destinations. Same conversions as the formatter except `%s`
 * (rejected): `%%`, `%c`, `%d`, `%i`, `%u`, `%x`, `%X`. Width caps how
 * many characters a field may consume; `0`/`-` flags are accepted and
 * ignored. All of @p str_size must be consumed.
 *
 * Not supported (rejected — see return value): `%s`, length modifiers,
 * `%f`/`%e`/`%g`, `%p`, `%o`, `*` width.
 */

#ifndef LH_STR_PARSE_TEXT_H
#define LH_STR_PARSE_TEXT_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse @p str according to @p fmt (the `va_list` primitive; see
 *        ::lh_str_ptr_parse_text for the variadic wrapper).
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param fmt      NUL-terminated format string.
 * @param args     Destinations, one pointer per conversion (`%u`/`%x`/`%X`
 *                 → ::lh_uint_t *, `%d`/`%i` → ::lh_sint_t *, `%c` →
 *                 ::lh_char_t *).
 *
 * @return ::lh_bool_true if @p fmt matched and all of @p str_size was
 *         consumed, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_ptr_parse_text_v(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args);

/**
 * @brief Parse @p str according to @p fmt.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param fmt      NUL-terminated format string.
 * @param ...      Destinations, one pointer per conversion (see
 *                 ::lh_str_ptr_parse_text_v).
 *
 * @return ::lh_bool_true on a full match, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_ptr_parse_text(lh_str_cptr str, lh_usize_t str_size, lh_str_cptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_TEXT_H */
