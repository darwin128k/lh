/**
 * @file text.h
 * @brief printf-style text formatting — portable, no libc/OS dependency.
 *
 * The format mask is walked by ::lh_str_ptr_scanf_next (shared with
 * ::lh_str_ptr_parse_text). Values are emitted by ::lh_str_ptr_format_uint /
 * _sint / _hex.
 *
 * ::lh_str_ptr_format_text writes no terminator, and nothing at all when the
 * result does not fit; ::lh_str_ptr_format_cstr writes the terminator too, and a
 * buffer that is too small comes out empty, never half a line.
 *
 * Supported conversions: `%%`, `%c`, `%s`, `%d`, `%i`, `%u`, `%x`, `%X`, `%f`, `%F`
 * (fixed notation, ::lh_str_ptr_format_real_fixed) and `%p`. Integer conversions
 * take the length modifiers `hh`, `h`, `l`, `ll`, `j`, `z` and `t`.
 * Supported flags: `0` (zero-pad), `-` (left-justify). Supported field: a
 * decimal width, and — for `%s` only — a `.` precision (max characters).
 *
 * Not supported (rejected — see return value): `%e`/`%g`, `%o`, `+`/space flags,
 * `*` (width or precision taken from an argument) and a `%f` longer than
 * ::LH_STR_FORMAT_TEXT_VALUE_BUF_MAX. Add them if a real use needs them.
 */

#ifndef LH_STR_FORMAT_TEXT_H
#define LH_STR_FORMAT_TEXT_H

#include <lh/core/size/limits.h>
#include <lh/core/str/format/uint.h>
#include <lh/core/str/scanf/spec.h>

#include <stdarg.h>

/**
 * @brief Bytes of the local buffer one value conversion is built in: the widest
 *        `%f` of a double (309 integer digits, a sign, a point and a precision of
 *        up to 40) fits; `%s` never goes through it.
 */
#define LH_STR_FORMAT_TEXT_VALUE_BUF_MAX 352U

LH_COMPILER_EXTERN_C_BEGIN

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
 * @brief The signed integer argument of @p spec, read as its length modifier says.
 */
lh_sllong_t
lh_str_ptr_format_text_read_sint(const lh_str_scanf_spec_t *spec, va_list *args);

/**
 * @brief The unsigned integer argument of @p spec, read as its length modifier says.
 */
lh_ullong_t
lh_str_ptr_format_text_read_uint(const lh_str_scanf_spec_t *spec, va_list *args);

/**
 * @brief How many of @p n characters to write at @p pos: in cut mode the most
 *        that fit in @p str_size, else all @p n.
 *
 * With a ::lh_null @p str (counting) nothing is cut.
 *
 * @param str      Destination, or ::lh_null when counting.
 * @param str_size Capacity of @p str.
 * @param pos      Where the next character goes; at most @p str_size.
 * @param n        Characters to write.
 * @param cut      Whether to cut at the end of @p str.
 */
lh_usize_t
lh_str_ptr_format_text_get_fit(const lh_str_ptr str, lh_usize_t str_size, lh_usize_t pos,
                               lh_usize_t n, lh_bool_t cut);

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
 * @param cut In cut mode the field is written as far as it fits (padding and
 *            content in their order) and `*pos` stops at @p str_size.
 * @return ::lh_bool_false when the field does not fit: nothing is written
 *         then, or, in cut mode, its first characters up to @p str_size.
 */
lh_bool_t
lh_str_ptr_format_text_put_field(lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                                 const lh_str_scanf_spec_t *spec, const lh_str_ptr content,
                                 lh_usize_t size, lh_bool_t cut);

/**
 * @brief The engine both forms run on: format @p fmt with @p args into @p str,
 *        or, when @p str is ::lh_null, only count the characters it would take.
 *
 * @param str      Destination, or ::lh_null to count.
 * @param str_size Capacity of @p str (ignored when counting).
 * @param fmt      NUL-terminated format mask.
 * @param args     The values, in mask order.
 * @param cut      Whether a result that does not fit is cut at @p str_size
 *                 instead of failing.
 * @return Characters written (or needed); @c 0 when they do not fit (in cut
 *         mode @p str_size: the first characters were written), and
 *         ::LH_STR_PTR_INVALID for a malformed mask when counting.
 */
lh_usize_t
lh_str_ptr_format_text_walk(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, va_list args,
                            lh_bool_t cut);

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
 * @brief Format @p fmt and @p args into @p str, cut at @p str_size when the
 *        result is longer.
 *
 * The cut falls on a byte: a multi-byte UTF-8 character at the end may be
 * left incomplete.
 *
 * @param str      Destination buffer; checked by ::lh_assert_runtime_ref.
 * @param str_size Capacity of @p str in characters.
 * @param fmt      NUL-terminated format string.
 * @param args     Arguments, one per conversion in @p fmt.
 * @return Characters written: the whole result, or @p str_size when it was
 *         cut; 0 for an unknown conversion or a malformed @p fmt.
 */
lh_usize_t
lh_str_ptr_format_text_cut_v(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt,
                             va_list args);

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

/**
 * @brief Format @p fmt and @p args into @p str, NUL-terminated.
 *
 * Same conversions as ::lh_str_ptr_format_text.
 *
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters, the NUL included; not 0.
 * @param fmt      NUL-terminated format string.
 * @param args     Arguments, one per conversion in @p fmt.
 * @return Characters written before the NUL; 0, with @p str empty, when the result
 *         does not fit or @p fmt is malformed.
 */
lh_usize_t
lh_str_ptr_format_cstr_v(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, va_list args);

/**
 * @brief Format @p fmt and @p args into @p str, NUL-terminated, cut to
 *        @p str_size - 1 characters when the result is longer
 *        (::lh_str_ptr_format_text_cut_v).
 *
 * @param str      Destination buffer; checked by ::lh_assert_runtime_ref.
 * @param str_size Capacity of @p str in characters, the NUL included; not 0.
 * @param fmt      NUL-terminated format string.
 * @param args     Arguments, one per conversion in @p fmt.
 * @return Characters written before the NUL; 0, with @p str empty, for an
 *         unknown conversion or a malformed @p fmt.
 */
lh_usize_t
lh_str_ptr_format_cstr_cut_v(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt,
                             va_list args);

/**
 * @brief ::lh_str_ptr_format_cstr_v with variadic arguments.
 */
lh_usize_t
lh_str_ptr_format_cstr(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_TEXT_H */
