/**
 * @file lemire.h
 * @brief Eisel–Lemire: `w × 10^q` to the nearest ::lh_real_t, in integers.
 *
 * The algorithm of Daniel Lemire's `fast_float` (also in Go, Rust and
 * C++17's `from_chars`): multiply the normalized 64-bit mantissa by the
 * 128-bit `5^q` (lh/real/power5.h), take the top bits as the binary
 * mantissa, round to nearest, ties to even. For a mantissa that holds every
 * digit of the decimal, the result is the correctly rounded value (proved in
 * Mushtak & Lemire, "Fast number parsing without fallback", 2023).
 */

#ifndef LH_REAL_LEMIRE_H
#define LH_REAL_LEMIRE_H

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <lh/compiler/extern/c.h>
#    include <lh/void.h>
#    include <lh/numeric/fixed/types.h>

#    if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE
/** @brief Smallest `q` for which a halfway tie can be exact: -4 or -17. */
#        define LH_REAL_LEMIRE_ROUND_EVEN_MIN (-4)
/** @brief Largest `q` for which a halfway tie can be exact: 23 or 10. */
#        define LH_REAL_LEMIRE_ROUND_EVEN_MAX 23
#    else
/** @brief Smallest `q` for which a halfway tie can be exact: -4 or -17. */
#        define LH_REAL_LEMIRE_ROUND_EVEN_MIN (-17)
/** @brief Largest `q` for which a halfway tie can be exact: 23 or 10. */
#        define LH_REAL_LEMIRE_ROUND_EVEN_MAX 10
#    endif

LH_COMPILER_EXTERN_C_BEGIN

/** `floor(q × log2(10)) + 63`: the binary exponent of `10^q`, offset as the algorithm needs. */
lh_s32_t
lh_real_lemire_get_power2(lh_s64_t q);

/**
 * @brief The top 128 bits of `w × 5^q` into @p high and @p low, with the
 *        second table word only when the first leaves the needed bits in
 *        doubt. @p w must be normalized (top bit set).
 */
lh_void
lh_real_lemire_multiply(lh_s64_t q, lh_u64_t w, lh_u64_t *high, lh_u64_t *low);

/**
 * @brief The stored @p fraction and biased @p exponent of the ::lh_real_t
 *        nearest to `w × 10^q`.
 *
 * Zero gives `0, 0`; too small gives a subnormal or zero; too big gives an
 * @p exponent of ::LH_REAL_EXPONENT_MAX (infinity), which the caller checks.
 */
lh_void
lh_real_lemire_compute(lh_s64_t q, lh_u64_t w, lh_u64_t *fraction, lh_s32_t *exponent);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_REAL_LEMIRE_H */
