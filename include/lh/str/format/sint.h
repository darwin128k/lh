/**
 * @file sint.h
 * @brief Format a signed integer as decimal text.
 */

#ifndef LH_STR_FORMAT_SINT_H
#define LH_STR_FORMAT_SINT_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Format @p value as decimal text — leading `-` for negative values,
 *        no leading zeros, no NUL terminator appended.
 *
 * Safe for ::LH_SINT_T_MIN (negation would overflow ::lh_sint_t; the
 * magnitude is computed via unsigned wraparound instead).
 *
 * @param value    Value to format.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written (at least 1), or 0 if @p str_size was too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_ptr_format_sint(lh_sint_t value, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_SINT_H */
