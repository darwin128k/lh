/**
 * @file real.h
 * @brief Format an ::lh_real_t as the shortest decimal text that reads back
 *        as the same number.
 *
 * Grisu2 (lh/real/grisu.h) finds the digits, laid out the way JavaScript's
 * `Number.prototype.toString` lays them out — so the text is valid JSON and
 * what a browser would print:
 *
 * - plain notation for `1e-7 <= |x| < 1e21`: `0.001`, `123.456`, `100`;
 * - exponent notation outside it: `1e+21`, `1.5e-7`;
 * - `-0` keeps its sign (JavaScript drops it), so it reads back as `-0`.
 *
 * Only with ::LH_REAL_ENABLED; no FPU arithmetic, no libc.
 */

#ifndef LH_STR_FORMAT_REAL_H
#define LH_STR_FORMAT_REAL_H

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <lh/compiler/extern/c.h>
#    include <lh/numeric/fixed/types.h>
#    include <lh/size.h>
#    include <lh/str/ptr.h>

/**
 * @brief Characters the longest result takes: `-1.2345678901234567e-308`
 *        is 24; with a margin.
 */
#    define LH_STR_FORMAT_REAL_SIZE_MAX 32U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Lay out the @p count digits at @p digits, whose decimal point sits
 *        @p point digits from the left (`point <= 0`: before them), in
 *        JavaScript's notation. No sign.
 *
 * @return Characters written, or 0 when they do not all fit in @p str_size
 *         (nothing is written then).
 */
lh_usize_t
lh_str_ptr_format_real_layout(const lh_str_ptr digits, lh_usize_t count, lh_s32_t point,
                              lh_str_ptr str, lh_usize_t str_size);

/**
 * @brief Format @p value as the shortest text that reads back as it.
 *
 * @return Characters written (no NUL), or 0 when @p value is an infinity or
 *         a NaN — which no number text can hold — or the text does not fit
 *         in @p str_size (::LH_STR_FORMAT_REAL_SIZE_MAX always does).
 */
lh_usize_t
lh_str_ptr_format_real(lh_real_t value, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_STR_FORMAT_REAL_H */
