/**
 * @file grisu.h
 * @brief Grisu2: the shortest digits that read back as the same
 *        ::lh_real_t, in integers.
 *
 * Florian Loitsch's algorithm ("Printing Floating-Point Numbers Quickly and
 * Accurately with Integers", 2010), in the form RapidJSON prints doubles
 * with. The digits always read back as exactly the number they came from;
 * they are the shortest such digits for all but a few inputs in a thousand,
 * which get one digit more. The boundaries follow ::lh_real_t, so a float
 * build gets float's shortest digits (`0.1f` is `1`, not `100000001490116`).
 */

#ifndef LH_REAL_GRISU_H
#define LH_REAL_GRISU_H

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <lh/char.h>
#    include <lh/compiler/extern/c.h>
#    include <lh/void.h>
#    include <lh/numeric/fixed/types.h>
#    include <lh/real/diy.h>
#    include <lh/size.h>

/** @brief Room ::lh_real_grisu needs for its digits: 17 for a double, with a margin. */
#    define LH_REAL_GRISU_DIGITS_MAX 20U

LH_COMPILER_EXTERN_C_BEGIN

/** How many decimal digits @p value has; `1` for `0`. */
lh_u32_t
lh_real_grisu_count_digits(lh_u32_t value);

/**
 * @brief Walk the last digit of @p digits down towards the exact value while
 *        it stays inside the boundaries and gets closer.
 */
lh_void
lh_real_grisu_round(lh_char_t *digits, lh_usize_t count, lh_u64_t delta, lh_u64_t rest,
                    lh_u64_t ten_kappa, lh_u64_t distance);

/**
 * @brief Cut the digits of @p w out of the scaled upper boundary @p upper,
 *        stopping as soon as they are inside the @p delta wide interval.
 *
 * @param digits Receives the digits, at least ::LH_REAL_GRISU_DIGITS_MAX.
 * @param count  Receives how many.
 * @param k      In: the decimal exponent of the scaling; out: of the last digit.
 */
lh_void
lh_real_grisu_digits(lh_real_diy_t w, lh_real_diy_t upper, lh_u64_t delta, lh_char_t *digits,
                     lh_usize_t *count, lh_s32_t *k);

/**
 * @brief The shortest digits of the finite, positive @p value:
 *        `value = digits × 10^k`, read back exactly.
 *
 * @param digits Receives the ASCII digits, no point, no NUL; room for
 *               ::LH_REAL_GRISU_DIGITS_MAX.
 * @param count  Receives how many digits.
 * @param k      Receives the decimal exponent of the last digit.
 */
lh_void
lh_real_grisu(lh_real_t value, lh_char_t *digits, lh_usize_t *count, lh_s32_t *k);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_REAL_GRISU_H */
