/**
 * @file cached.h
 * @brief Cached powers of ten for Grisu2 (lh/real/grisu.h): `10^k` as an
 *        ::lh_real_diy_t, every eighth `k` from `-348` to `340`.
 *
 * Grisu scales a number by a power of ten chosen so that the product's
 * binary exponent lands in `[-60, -32]`, where its digits can be cut out
 * with 32- and 64-bit integers. Eighty-seven powers, eight decades apart,
 * always leave one that fits (1.4 KB).
 */

#ifndef LH_REAL_CACHED_H
#define LH_REAL_CACHED_H

#include <lh/real.h>

#if LH_REAL_ENABLED

#    include <lh/compiler/extern/c.h>
#    include <lh/real/diy.h>
#    include <lh/size.h>

/** @brief How many powers the table holds. */
#    define LH_REAL_CACHED_COUNT 87
/** @brief The `k` of the first power, `10^-348`. */
#    define LH_REAL_CACHED_K_MIN (-348)
/** @brief How many decades apart two powers are. */
#    define LH_REAL_CACHED_K_STEP 8

LH_COMPILER_EXTERN_C_BEGIN

/** `10^k` at @p index, normalized. */
lh_real_diy_t
lh_real_cached_get(lh_usize_t index);

/** The `k` of the power at @p index. */
lh_s32_t
lh_real_cached_get_k(lh_usize_t index);

/**
 * @brief The index of the power that brings a number with binary exponent
 *        @p e into `[-60, -32]` once multiplied (::lh_real_diy_mul).
 */
lh_usize_t
lh_real_cached_find(lh_s32_t e);

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_ENABLED */

#endif /* LH_REAL_CACHED_H */
