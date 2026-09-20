/**
 * @file text.h
 * @brief printf-style text formatting — portable, no libc/OS dependency.
 *
 * The format mask is walked by ::lh_str_ptr_scanf_next (shared with
 * ::lh_str_ptr_parse_text). Values are emitted by ::lh_str_ptr_format_uint /
 * _sint / _hex.
 *
 * Supported conversions: `%%`, `%c`, `%s`, `%d`, `%i`, `%u`, `%x`, `%X`.
 * Supported flags: `0` (zero-pad), `-` (left-justify). Supported field: a
 * decimal width, and — for `%s` only — a `.` precision (max characters).
 *
 * Not supported (rejected — see return value): length modifiers (`l`, `h`,
 * ...; every integer argument is ::lh_sint_t / ::lh_uint_t width), `%f`/`%e`/
 * `%g`, `%p`, `%o`, `+`/space flags, and `*` (width or precision taken from
 * an argument). Add them if a real use needs them — this covers what the
 * library itself formats today.
 */

#ifndef LH_STR_FORMAT_TEXT_H
#define LH_STR_FORMAT_TEXT_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format @p fmt and @p args into @p str (the `va_list` primitive;
 *        see ::lh_str_ptr_format_text for the variadic wrapper).
 *
 * All-or-nothing: on any failure — unknown conversion, malformed format
 * string, or @p str_size too small for the full result — nothing is written
 * and 0 is returned; there is no partial/truncated output.
 *
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 * @param fmt      NUL-terminated format string.
 * @param args     Arguments, one per conversion in @p fmt (`%s` arguments
 *                 are ::lh_str_cptr, NUL-terminated).
 *
 * @return Characters written, or 0 on failure (see above).
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_ptr_format_text_v(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, va_list args);

/**
 * @brief Format @p fmt and the following arguments into @p str.
 *
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 * @param fmt      NUL-terminated format string.
 * @param ...      Arguments, one per conversion in @p fmt.
 *
 * @return Characters written, or 0 on failure (see ::lh_str_ptr_format_text_v).
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_ptr_format_text(lh_str_ptr str, lh_usize_t str_size, lh_str_cptr fmt, ...);

/**
 * @brief Characters ::lh_str_ptr_format_text_v would write for @p fmt.
 *
 * Does not write. On a malformed mask returns ::LH_USIZE_T_MAX.
 * A valid empty mask returns 0.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_ptr_format_text_size_v(lh_str_cptr fmt, va_list args);

/**
 * @brief Variadic wrapper for ::lh_str_ptr_format_text_size_v.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_ptr_format_text_size(lh_str_cptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_TEXT_H */
