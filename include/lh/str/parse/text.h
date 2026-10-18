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

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/str/scanf/spec.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/*
 * The steps ::lh_str_ptr_parse_text_v is built from. Each reads one token's
 * part of @p str at `*pos`, stores what it read through the token's argument
 * (taken from @p args) and moves `*pos` past it; ::lh_bool_false means the
 * input does not match and the whole parse fails. @p limit is how much the
 * token may take (::lh_str_scanf_spec_get_limit).
 */

/** @brief Match the literal run of @p spec. */
lh_bool_t
lh_str_ptr_parse_text_literal(const lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                              const lh_str_scanf_spec_t *spec);

/** @brief Match a `%`. */
lh_bool_t
lh_str_ptr_parse_text_percent(const lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos);

/** @brief Read one character into the `lh_char_t *` argument. */
lh_bool_t
lh_str_ptr_parse_text_char(const lh_str_ptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args);

/**
 * @brief Read characters into the `lh_str_ptr, lh_usize_t` buffer argument:
 *        up to the precision, and up to the literal that follows in @p fmt
 *        (at @p fmt_pos) when there is one.
 */
lh_bool_t
lh_str_ptr_parse_text_str(const lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                          const lh_str_scanf_spec_t *spec, lh_usize_t limit, const lh_str_ptr fmt,
                          lh_usize_t fmt_pos, va_list *args);

/** @brief Read a decimal ::lh_uint_t into the `lh_uint_t *` argument. */
lh_bool_t
lh_str_ptr_parse_text_uint(const lh_str_ptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args);

/** @brief Read a decimal ::lh_sint_t, `-` allowed, into the `lh_sint_t *` argument. */
lh_bool_t
lh_str_ptr_parse_text_sint(const lh_str_ptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args);

/** @brief Read a hexadecimal ::lh_uint_t into the `lh_uint_t *` argument. */
lh_bool_t
lh_str_ptr_parse_text_hex(const lh_str_ptr str, lh_usize_t *pos, lh_usize_t limit, va_list *args);

/** @brief The step for @p spec's kind; ::lh_bool_false for an unknown one. */
lh_bool_t
lh_str_ptr_parse_text_field(const lh_str_ptr str, lh_usize_t str_size, lh_usize_t *pos,
                            const lh_str_scanf_spec_t *spec, const lh_str_ptr fmt,
                            lh_usize_t fmt_pos, va_list *args);

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
lh_usize_t
lh_str_ptr_parse_text_v(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt,
                        va_list args);

/**
 * @brief Parse @p str against @p fmt and the following destinations.
 *
 * @return Characters consumed from @p str, or 0 on failure
 *         (see ::lh_str_ptr_parse_text_v).
 */
lh_usize_t
lh_str_ptr_parse_text(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr fmt, ...);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_TEXT_H */
