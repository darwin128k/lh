/**
 * @file uint.h
 * @brief Format an unsigned integer as decimal text.
 */

#ifndef LH_STR_FORMAT_UINT_H
#define LH_STR_FORMAT_UINT_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format @p value as decimal text — no leading zeros, no sign, no
 *        NUL terminator appended.
 *
 * @param value    Value to format.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written (at least 1, since `0` itself takes one digit),
 *         or 0 if @p str_size was too small to hold the result.
 */
lh_usize_t
lh_str_ptr_format_uint(lh_uint_t value, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_UINT_H */
