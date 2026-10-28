/**
 * @file ullong.h
 * @brief Parse a 64-bit unsigned decimal integer from text.
 *
 * The ::lh_ullong_t counterpart of lh/str/parse/uint.h, for numbers past
 * ::lh_uint_t: the same rules, a wider accumulator
 * (::lh_char_digit_accumulate_ullong).
 */

#ifndef LH_STR_PARSE_ULLONG_H
#define LH_STR_PARSE_ULLONG_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse an unsigned decimal integer, rejecting values above @p max.
 *
 * Accepts only ASCII digits `0`-`9`; all of @p str_size must be consumed.
 * Leading zeros are allowed (`09`). Empty input is rejected. Safe against
 * ::lh_ullong_t overflow regardless of @p max.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 * @return ::lh_bool_true if all of @p str_size was consumed as a decimal
 *         integer not exceeding @p max, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_str_ptr_parse_ullong_digits(const lh_str_ptr str, lh_usize_t str_size, lh_ullong_t max,
                               lh_ullong_t *out);

/**
 * @brief Parse an unsigned decimal integer without padded leading zeros.
 *
 * ::lh_str_ptr_parse_ullong_digits, then rejects a leading zero on input
 * longer than one digit (e.g. `007`).
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 * @return ::lh_bool_true if the text is a decimal integer not exceeding
 *         @p max and not zero-padded, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_str_ptr_parse_ullong(const lh_str_ptr str, lh_usize_t str_size, lh_ullong_t max,
                        lh_ullong_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_ULLONG_H */
