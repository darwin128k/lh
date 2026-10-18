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

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/size/limits.h>
#include <lh/str/ptr.h>
#include <lh/str/format/uint.h>
#include <lh/str/scanf/spec.h>
#include <lh/math/ops.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Bytes of the local buffer one value conversion is built in: a sign
 *        and the longest ::lh_uint_t, so the most negative ::lh_sint_t fits
 *        whatever `int`'s width. `%s` never goes through it.
 */
#define LH_STR_FORMAT_TEXT_VALUE_BUF_MAX lh_math_add_one(LH_STR_FORMAT_UINT_DIGITS_MAX)

/**
 * @brief Write @p n characters of @p text at `*pos` and move `*pos` past them.
 *
 * With a ::lh_null @p str nothing is written: `*pos` only counts.
 *
 * @return ::lh_bool_false, writing nothing, when they do not fit in @p str_size.
 */
lh_bool_t
lh_str_ptr_format_text_put_text(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                                const lh_str_ptr text, lh_usize_t n);

/**
 * @brief Write @p n copies of @p ch at `*pos` and move `*pos` past them.
 *
 * With a ::lh_null @p str nothing is written: `*pos` only counts.
 *
 * @return ::lh_bool_false, writing nothing, when they do not fit in @p str_size.
 */
lh_bool_t
lh_str_ptr_format_text_put_run(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos, lh_char_t ch,
                               lh_usize_t n);

/**
 * @brief The text one conversion token turns its argument into.
 *
 * Numbers and characters are built in @p buf; a `%s` argument is used in
 * place, cut to the precision.
 *
 * @param spec     A conversion token (not a literal).
 * @param args     The remaining arguments; the token's one is taken.
 * @param buf      ::LH_STR_FORMAT_TEXT_VALUE_BUF_MAX bytes to build in.
 * @param content  Receives where the text is.
 * @param size     Receives its length.
 * @return ::lh_bool_false for a conversion the engine does not know, or one
 *         that came out empty (anything but `%s`).
 */
lh_bool_t
lh_str_ptr_format_text_get_value(const lh_str_scanf_spec_t *spec, va_list *args, lh_str_ptr buf,
                                 const lh_str_ptr *content, lh_usize_t *size);

/**
 * @brief Write @p content padded to the token's width, the way its flags say:
 *        `-` pads on the right, `0` pads with zeros after a sign.
 *
 * With a ::lh_null @p str nothing is written: `*pos` only counts.
 *
 * @return ::lh_bool_false, writing nothing, when the field does not fit.
 */
lh_bool_t
lh_str_ptr_format_text_put_field(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                                 const lh_str_scanf_spec_t *spec, const lh_str_ptr content,
                                 lh_usize_t size);

/**
 * @brief The engine both forms run on: format @p fmt with @p args into @p str,
 *        or, when @p str is ::lh_null, only count the characters it would take.
 *
 * @param str      Destination, or ::lh_null to count.
 * @param str_size Capacity of @p str (ignored when counting).
 * @param fmt      NUL-terminated format mask.
 * @param args     The values, in mask order.
 * @return Characters written (or needed); @c 0 when they do not fit, and
 *         ::LH_STR_PTR_INVALID for a malformed mask when counting.
 */
lh_usize_t
lh_str_ptr_format_text_walk(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, va_list args);

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
 *                 are const lh_str_ptr, NUL-terminated).
 *
 * @return Characters written, or 0 on failure (see above).
 */
lh_usize_t
lh_str_ptr_format_text_v(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, va_list args);

/**
 * @brief Characters ::lh_str_ptr_format_text_v would write for @p fmt.
 *
 * Does not write. On a malformed mask returns ::LH_USIZE_T_MAX.
 * A valid empty mask returns 0.
 */
lh_usize_t
lh_str_ptr_format_text_size_v(const lh_str_ptr fmt, va_list args);

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
lh_usize_t
lh_str_ptr_format_text(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, ...);

/**
 * @brief Variadic wrapper for ::lh_str_ptr_format_text_size_v.
 */
lh_usize_t
lh_str_ptr_format_text_size(const lh_str_ptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_TEXT_H */
