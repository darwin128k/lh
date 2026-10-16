/**
 * @file isqrt.h
 * @brief Integer square root on ::lh_u64_t, no FPU.
 *
 * Force-inline (like `lh/math/floor.h`): one shared primitive for fixed-point
 * code (distances in `lh/ui/radius`, …) instead of a private copy per caller.
 */

#ifndef LH_MATH_ISQRT_H
#define LH_MATH_ISQRT_H

#include <lh/bit/scan.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Largest power of four not above @p n (`0` for `0`).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_math_isqrt_u64_top_bit(lh_u64_t n)
{
    const lh_u32_t high = lh_cast_static(lh_u32_t, n >> 32);
    lh_usize_t index;

    if (n == 0U)
    {
        return 0U;
    }
    /* The highest set bit, rounded down to an even one: a scan, not a walk down from
       2^62 that took thirty steps for a corner's few-thousand-squared distances. */
    index = high != 0U ? 32U + lh_bit_scan_reverse_u32(high)
                       : lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, n));
    return lh_cast_static(lh_u64_t, 1) << (index & ~lh_cast_static(lh_usize_t, 1));
}

/**
 * @brief `floor(sqrt(n))`, digit by digit.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_math_isqrt_u64(lh_u64_t n)
{
    lh_u64_t root = 0;
    lh_u64_t bit;
    for (bit = lh_math_isqrt_u64_top_bit(n); bit != 0U; bit >>= 2)
    {
        const lh_u64_t take = n >= root + bit;
        n -= take ? root + bit : 0U;
        root = (root >> 1) + (take ? bit : 0U);
    }
    return root;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_ISQRT_H */
