/**
 * @file ullong.h
 * @brief Format a 64-bit unsigned integer as decimal text.
 *
 * Delegates to ::lh_str_ptr_format_uint for each 9-digit chunk. The digit
 * kernel is 32-bit; this is not a second extractor.
 */

#ifndef LH_STR_FORMAT_ULLONG_H
#define LH_STR_FORMAT_ULLONG_H

#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format @p value as decimal text — no leading zeros, no sign, no
 *        NUL terminator appended.
 *
 * @return Characters written (at least 1), or 0 if @p str_size was too small.
 */
lh_usize_t
lh_str_ptr_format_ullong(lh_ullong_t value, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_ULLONG_H */
