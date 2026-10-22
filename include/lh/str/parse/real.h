/**
 * @file real.h
 * @brief Parse a decimal number into an ::lh_real_t.
 *
 * What `strtod` does, without libc, locale or floating point arithmetic:
 * ::lh_real_decimal_parse takes the text apart, ::lh_real_decimal_to_real
 * rounds it to the nearest ::lh_real_t (Eisel–Lemire). Only with
 * ::LH_REAL_ENABLED.
 */

#ifndef LH_STR_PARSE_REAL_H
#define LH_STR_PARSE_REAL_H

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <lh/bool.h>
#    include <lh/compiler/extern/c.h>
#    include <lh/size.h>
#    include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Parse all of @p str_size characters at @p str as a decimal number.
 *
 * `-? digits? (. digits?)? ([eE] [+-]? digits)?`, at least one digit before
 * the exponent (see ::lh_real_decimal_parse). Rounded to nearest, ties to
 * even; exact up to 19 significant digits.
 *
 * @param out Receives the value; untouched on failure.
 * @return ::lh_bool_false when the text is not such a number or the value
 *         is too big for ::lh_real_t.
 */
lh_bool_t
lh_str_ptr_parse_real(const lh_str_ptr str, lh_usize_t str_size, lh_real_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_STR_PARSE_REAL_H */
