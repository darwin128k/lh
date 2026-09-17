/**
 * @file uint.h
 * @brief Parse an unsigned decimal integer from text.
 */

#ifndef LH_STR_PARSE_UINT_H
#define LH_STR_PARSE_UINT_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse an unsigned decimal integer, rejecting values above @p max.
 *
 * Accepts only ASCII digits `0`-`9`; all of @p str_size must be consumed.
 * Leading zeros are allowed (`09`). Empty input is rejected. Safe against
 * ::lh_uint_t overflow regardless of @p max.
 *
 * The digit loop shared by date/time (`09/05`) and by ::lh_str_ptr_parse_uint
 * (which then rejects padding). Digit accumulate itself lives in `lh/char/digit.h`.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a decimal
 *         integer not exceeding @p max, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_ptr_parse_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out);

/**
 * @brief Parse an unsigned decimal integer without padded leading zeros.
 *
 * Same digit loop as ::lh_str_ptr_parse_uint_digits, then rejects a leading
 * zero on input longer than one digit (e.g. `0080`). Used by IPv4/port.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if the field is a decimal integer not exceeding
 *         @p max and not zero-padded, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_ptr_parse_uint(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out);

/**
 * @brief Next @p delim-separated field, parsed as a padded unsigned integer.
 *
 * ::lh_str_ptr_split_next then ::lh_str_ptr_parse_uint_digits. Date and time
 * parse walk `Y/M/D` and `H:M:S` through this: middle fields pass
 * ::lh_bool_true for @p want_delim, the last field ::lh_bool_false.
 *
 * @param str        Buffer to scan.
 * @param str_size   Total characters in @p str.
 * @param delim      Field delimiter (`/` or `:`).
 * @param pos        In/out cursor. Start at `0` for the first field.
 * @param max        Inclusive maximum for this field.
 * @param out        Receives the parsed value on success.
 * @param want_delim Whether a delimiter must follow this field.
 *
 * @return ::lh_bool_true if a field was split, parsed, and ended as
 *         @p want_delim requires.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_ptr_split_next_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim,
                                  lh_usize_t *pos, lh_uint_t max, lh_uint_t *out,
                                  lh_bool_t want_delim);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_UINT_H */
