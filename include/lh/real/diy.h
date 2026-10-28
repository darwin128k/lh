/**
 * @file diy.h
 * @brief "Do it yourself" floating point: `f × 2^e` with a 64-bit
 *        significand (::lh_real_diy_t), the arithmetic under Grisu2
 *        (lh/real/grisu.h).
 *
 * Wider than any ::lh_real_t and exact in integers: an ::lh_real_t, its
 * rounding boundaries and a cached power of ten are all held as one of
 * these, multiplied with ::lh_numeric_mul_u64_full. A plain math value, like
 * the vectors in lh/math: the formulas read its two fields directly.
 */

#ifndef LH_REAL_DIY_H
#define LH_REAL_DIY_H

#include <lh/real.h>
#include <lh/compiler/extern/c.h>
#include <lh/void.h>

#if LH_REAL_ENABLED

#    include <lh/compiler/extern/c.h>
#    include <lh/void.h>
#    include <lh/numeric/fixed/types.h>
#    include <lh/real/diy/fields.h>

/**
 * @struct lh_real_diy
 * @brief Significand and exponent. Fields via ::lh_real_diy_fields.
 */
struct lh_real_diy
{
    lh_real_diy_fields(lh_u64_t, lh_s32_t);
};
/** @copybrief lh_real_diy */
typedef struct lh_real_diy lh_real_diy_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The significand: the number is `f × 2^e`. */
lh_u64_t
lh_real_diy_get_f(const lh_real_diy_t *self);

/** @brief Store the significand. Level-1 setter: no normalization. */
lh_void
lh_real_diy_set_f(lh_real_diy_t *self, lh_u64_t f);

/** @brief The binary exponent: the number is `f × 2^e`. */
lh_s32_t
lh_real_diy_get_e(const lh_real_diy_t *self);

/** @brief Store the binary exponent. Level-1 setter. */
lh_void
lh_real_diy_set_e(lh_real_diy_t *self, lh_s32_t e);

/** The number `f × 2^e`. */
lh_real_diy_t
lh_real_diy_make(lh_u64_t f, lh_s32_t e);

/** @p value, finite and not negative, exactly: its significand with the implicit 1. */
lh_real_diy_t
lh_real_diy_from_real(lh_real_t value);

/** @p self with the top bit of `f` set (`f` must not be zero). */
lh_real_diy_t
lh_real_diy_normalize(lh_real_diy_t self);

/**
 * @brief @p self — a boundary, at most two bits wider than a significand —
 *        with `f` shifted up as far as it goes.
 */
lh_real_diy_t
lh_real_diy_normalize_boundary(lh_real_diy_t self);

/** `a - b` for two numbers on the same exponent, `a >= b`. */
lh_real_diy_t
lh_real_diy_sub(lh_real_diy_t a, lh_real_diy_t b);

/** `a × b`, the top 64 bits of the product, rounded. */
lh_real_diy_t
lh_real_diy_mul(lh_real_diy_t a, lh_real_diy_t b);

/**
 * @brief The halfway points between @p value and its neighbours, both on
 *        the exponent of the upper one: every number strictly between them
 *        reads back as @p value.
 */
lh_void
lh_real_diy_get_boundaries(lh_real_t value, lh_real_diy_t *minus, lh_real_diy_t *plus);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_REAL_DIY_H */
