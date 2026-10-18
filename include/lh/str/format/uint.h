/**
 * @file uint.h
 * @brief Format an unsigned integer as decimal text.
 */

#ifndef LH_STR_FORMAT_UINT_H
#define LH_STR_FORMAT_UINT_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/limits.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @def LH_STR_FORMAT_UINT_DIGITS_MAX
 * @brief Room for the longest decimal form of an ::lh_uint_t, whatever its
 *        width on the target: every byte adds at most three digits
 *        (256^k < 10^3k) — 12 for a 32-bit `int`, 6 for a 16-bit one.
 */
#define LH_STR_FORMAT_UINT_DIGITS_MAX (LH_UINT_T_SIZE * 3U)

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
