/**
 * @file sllong.h
 * @brief Format a 64-bit signed integer as decimal text.
 *
 * ::lh_str_ptr_format_sint for ::lh_sllong_t: the sign, then the magnitude
 * through ::lh_str_ptr_format_ullong.
 */

#ifndef LH_STR_FORMAT_SLLONG_H
#define LH_STR_FORMAT_SLLONG_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format @p value as decimal text — leading `-` for negative values,
 *        no leading zeros, no NUL terminator appended.
 * Safe for the most negative ::lh_sllong_t (the magnitude is computed via
 * unsigned wraparound, not by negating @p value).
 * @param value    Value to format.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 * @return Characters written (at least 1), or 0 if @p str_size was too small.
 */
lh_usize_t
lh_str_ptr_format_sllong(lh_sllong_t value, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_SLLONG_H */
