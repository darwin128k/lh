/**
 * @file power5.h
 * @brief 128-bit powers of five, the table under Eisel–Lemire
 *        (lh/real/lemire.h).
 *
 * `10^q = 5^q × 2^q`, and the `2^q` is only an exponent, so turning a
 * decimal into binary is one multiplication by `5^q` — known to 128 bits,
 * normalized so the top bit is set, truncated. A double build keeps every
 * `q` a double can need, `[-342, 308]` (10.4 KB); a float build only
 * `[-65, 38]` (1.7 KB).
 */

#ifndef LH_REAL_POWER5_H
#define LH_REAL_POWER5_H

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <lh/compiler/extern/c.h>
#    include <lh/numeric/fixed/types.h>
#    include <lh/size.h>

#    if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE
/** @brief Smallest `q` in the table: below it every 19-digit decimal is zero. */
#        define LH_REAL_POWER5_MIN (-342)
/** @brief Largest `q` in the table: above it every nonzero decimal is infinite. */
#        define LH_REAL_POWER5_MAX 308
#    else
#        define LH_REAL_POWER5_MIN (-65)
#        define LH_REAL_POWER5_MAX 38
#    endif

LH_COMPILER_EXTERN_C_BEGIN

/** Where `5^q` starts in the table (two words per power). */
lh_usize_t
lh_real_power5_get_index(lh_s64_t q);

/** The upper 64 bits of `5^q`, normalized (top bit set). */
lh_u64_t
lh_real_power5_get_high(lh_s64_t q);

/** The next 64 bits of `5^q`, truncated. */
lh_u64_t
lh_real_power5_get_low(lh_s64_t q);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_REAL_POWER5_H */
