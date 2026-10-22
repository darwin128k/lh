/**
 * @file sllong.h
 * @brief Parse a 64-bit signed decimal integer from text.
 *
 * An optional `-`, then digits as ::lh_str_ptr_parse_ullong takes them. No
 * `+`, no spaces, no padding: the form a number has in JSON or in a log.
 */

#ifndef LH_STR_PARSE_SLLONG_H
#define LH_STR_PARSE_SLLONG_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse a signed decimal integer in `[min, max]`.
 *
 * All of @p str_size must be consumed: an optional `-` and at least one
 * digit, without a leading zero on more than one digit (`-0` is `0`).
 * The whole ::lh_sllong_t range parses, `-9223372036854775808` included.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param min      Smallest value considered valid (inclusive).
 * @param max      Largest value considered valid (inclusive).
 * @param out      Receives the parsed value on success; untouched on failure.
 * @return ::lh_bool_true if the text is such an integer within
 *         `[min, max]`, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_str_ptr_parse_sllong(const lh_str_ptr str, lh_usize_t str_size, lh_sllong_t min, lh_sllong_t max,
                        lh_sllong_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_PARSE_SLLONG_H */
