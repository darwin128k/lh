/**
 * @file radix.h
 * @brief Parse an unsigned integer in any base from 2 to 16 from text.
 *
 * The digit loop under ::lh_str_ptr_parse_uint_digits (base 10) and
 * ::lh_str_ptr_parse_hex (base 16): one loop, the base a parameter.
 */

#ifndef LH_STR_PARSE_RADIX_H
#define LH_STR_PARSE_RADIX_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How many characters at the start of @p str are digits of base @p radix.
 *
 * @param str      Buffer to look at (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param radix    Base, `2`..`16` (letters `a`-`f` / `A`-`F` above ten).
 *
 * @return Length of the leading digit run, `0` when @p str does not start with one.
 */
lh_usize_t
lh_str_ptr_count_radix_digits(const lh_str_ptr str, lh_usize_t str_size, lh_uint_t radix);

/**
 * @brief Parse an unsigned integer in base @p radix, rejecting values above @p max.
 *
 * All of @p str_size must be digits of the base; empty input is rejected.
 * Leading zeros are allowed. No sign and no `0x`-style prefix. Safe against
 * ::lh_uint_t overflow regardless of @p max.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param radix    Base, `2`..`16`.
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as an integer not
 *         exceeding @p max, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_str_ptr_parse_radix(const lh_str_ptr str, lh_usize_t str_size, lh_uint_t radix, lh_uint_t max,
                       lh_uint_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_RADIX_H */
