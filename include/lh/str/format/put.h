/**
 * @file put.h
 * @brief The all-or-nothing write the str_ptr formatters finish with.
 */

#ifndef LH_STR_FORMAT_PUT_H
#define LH_STR_FORMAT_PUT_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Copy @p n characters of @p text to @p str when they all fit.
 *
 * The formatters' rule: a result is written whole or not at all.
 *
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str.
 * @param text     Characters to write.
 * @param n        Number of characters in @p text.
 * @return @p n, or @c 0 when @p n exceeds @p str_size (nothing is written).
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p str is ::lh_null.
 */
lh_usize_t
lh_str_ptr_format_put(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr text, lh_usize_t n);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_PUT_H */
