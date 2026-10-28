/**
 * @file swar.h
 * @brief SWAR — byte-lane tests on a whole machine word at once.
 *
 * "SIMD within a register": a ::lh_usize_t holds 4 or 8 bytes ("lanes"), and
 * a few adds, subtracts and masks test all of them in one step — is any lane
 * zero, below a bound, above a bound. String and memory scans read a word,
 * ask one of these, and only look at single bytes once a lane is marked.
 *
 * A test answers with a mask: bit 7 of every marked lane set
 * (::LH_BIT_SWAR_HIGH selects exactly those bits), everything else clear.
 * ::lh_bit_swar_first_lane / ::lh_bit_swar_last_lane turn a mask back into an
 * address-ordered lane index, whatever the byte order.
 *
 * Each trick used to be spelled out by hand at every scan; like
 * `lh/bit/rotate.h` and `lh/bit/scan.h`, this names it once.
 */

#ifndef LH_BIT_SWAR_H
#define LH_BIT_SWAR_H

#include <lh/bit/scan.h>

/**
 * @def LH_BIT_SWAR_BIG_ENDIAN
 * @brief 1 when the lowest address holds the most significant byte.
 */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    define LH_BIT_SWAR_BIG_ENDIAN 1
#else
#    define LH_BIT_SWAR_BIG_ENDIAN 0
#endif

/**
 * @def LH_BIT_SWAR_ONES
 * @brief 0x0101…01: the byte 1 in every lane.
 */
#define LH_BIT_SWAR_ONES (lh_bit_not(lh_cast_static(lh_usize_t, 0U)) / 0xFFU)

/**
 * @def LH_BIT_SWAR_HIGH
 * @brief 0x8080…80: bit 7 of every lane — where the masks below mark a lane.
 */
#define LH_BIT_SWAR_HIGH lh_bit_shl(LH_BIT_SWAR_ONES, 7U)

/**
 * @def LH_BIT_SWAR_RAMP
 * @brief The bytes 0, 1, 2, … in address order: lane @c i holds @c i.
 */
#if LH_BIT_SWAR_BIG_ENDIAN
#    define LH_BIT_SWAR_RAMP                                                                       \
        (sizeof(lh_usize_t) == 8U ? lh_cast_static(lh_usize_t, 0x0001020304050607ULL)              \
                                  : lh_cast_static(lh_usize_t, 0x00010203UL))
#else
#    define LH_BIT_SWAR_RAMP                                                                       \
        (sizeof(lh_usize_t) == 8U ? lh_cast_static(lh_usize_t, 0x0706050403020100ULL)              \
                                  : lh_cast_static(lh_usize_t, 0x03020100UL))
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p b repeated in every lane.
 *
 * Takes a word, not a ::lh_byte_t, so a byte the caller computed (`127 - hi`)
 * is not narrowed again on the way in.
 *
 * @param b Byte value to repeat, 0 to 0xFF.
 * @return The word whose every byte is @p b.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_broadcast(lh_usize_t b)
{
    return lh_math_mul(LH_BIT_SWAR_ONES, b);
}

/**
 * @brief Mark the lanes of @p v whose byte is below a bound.
 *
 * Exact for the first marked lane in address order on a little-endian
 * machine; a borrow out of a marked lane may also mark lanes after it, so a
 * caller that needs more than the first one rechecks bytes.
 *
 * @param v      Word to test.
 * @param bound  ::lh_bit_swar_broadcast of the bound, at most 0x80.
 * @return Mask with bit 7 set in the lanes whose byte is below the bound.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_below_mask(lh_usize_t v, lh_usize_t bound)
{
    return lh_bit_and(lh_bit_and(lh_math_sub(v, bound), lh_bit_not(v)), LH_BIT_SWAR_HIGH);
}

/**
 * @brief Mark the lanes of @p v whose byte is above a bound, or 0x80 or more.
 *
 * Exact as long as no lane of @p v has bit 7 set (a carry out of such a lane
 * may spill into the next one); callers test that first.
 *
 * @param v    Word to test.
 * @param lift ::lh_bit_swar_broadcast of `127 - bound`, bound below 0x80.
 * @return Mask with bit 7 set in the lanes whose byte is above the bound.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_above_mask(lh_usize_t v, lh_usize_t lift)
{
    return lh_bit_and(lh_bit_or(lh_math_add(v, lift), v), LH_BIT_SWAR_HIGH);
}

/**
 * @brief Mark the lanes of @p v outside the range [lo, hi], @c hi below 0x80.
 *
 * ::lh_bit_swar_below_mask or ::lh_bit_swar_above_mask, with their limits:
 * exact while no lane has bit 7 set, and past the first marked lane a borrow
 * may mark more.
 *
 * With lo = 0 nothing can be below, and the below test is skipped. Inside a
 * scan @p below does not change, so the compiler splits the loop in two and
 * neither copy pays for the test: measured, testing anyway made a lo = 0 scan
 * about twice as slow.
 *
 * @param v     Word to test.
 * @param below ::lh_bit_swar_broadcast(lo) — 0 for lo = 0.
 * @param lift  ::lh_bit_swar_broadcast(127 - hi).
 * @return Mask with bit 7 set in the lanes below @c lo or above @c hi.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_outside_mask(lh_usize_t v, lh_usize_t below, lh_usize_t lift)
{
    lh_usize_t out = 0U;

    if (lh_math_ne(below, 0U))
    {
        out = lh_bit_swar_below_mask(v, below);
    }
    return lh_bit_or(out, lh_bit_swar_above_mask(v, lift));
}

/**
 * @brief The lanes @p mask leaves unmarked, as a mask of their own.
 *
 * @param mask Mask from one of the tests here.
 * @return Mask with bit 7 set exactly in the lanes @p mask does not mark.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_unmarked(lh_usize_t mask)
{
    return lh_bit_and(lh_bit_not(mask), LH_BIT_SWAR_HIGH);
}

/**
 * @brief Mark the lanes of @p v that are zero — the word-wide `strlen` test.
 *
 * Same exactness as ::lh_bit_swar_below_mask (it is that test with bound 1).
 *
 * @param v Word to test.
 * @return Mask with bit 7 set in the zero lanes; zero when no lane is zero.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_zero_mask(lh_usize_t v)
{
    return lh_bit_swar_below_mask(v, LH_BIT_SWAR_ONES);
}

/**
 * @brief Bit 7 of the lane at address offset @p lane (0 = lowest address).
 *
 * @param lane Lane index, below `sizeof(lh_usize_t)`.
 * @return The mask bit of that lane.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_lane_bit(lh_usize_t lane)
{
#if LH_BIT_SWAR_BIG_ENDIAN
    lane = lh_math_sub(sizeof(lh_usize_t) - 1U, lane);
#endif
    return lh_bit_shl(lh_cast_static(lh_usize_t, 0x80U), lh_bit_shl(lane, 3U));
}

/**
 * @brief Address offset of the first marked lane of @p mask.
 *
 * @param mask Nonzero mask from one of the tests above.
 * @return Lane index, 0 = lowest address.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_first_lane(lh_usize_t mask)
{
#if LH_BIT_SWAR_BIG_ENDIAN
    const lh_usize_t top = lh_math_sub(lh_bit_shl(sizeof(lh_usize_t), 3U), 1U);
    const lh_usize_t bit = sizeof(lh_usize_t) == 8U
                               ? lh_bit_scan_reverse_u64(mask)
                               : lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, mask));
    return lh_bit_shr(lh_math_sub(top, bit), 3U);
#else
    return lh_bit_shr(sizeof(lh_usize_t) == 8U
                          ? lh_bit_scan_forward_u64(mask)
                          : lh_bit_scan_forward_u32(lh_cast_static(lh_u32_t, mask)),
                      3U);
#endif
}

/**
 * @brief Address offset of the last marked lane of @p mask.
 *
 * @param mask Nonzero mask from one of the tests above.
 * @return Lane index, 0 = lowest address.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_swar_last_lane(lh_usize_t mask)
{
#if LH_BIT_SWAR_BIG_ENDIAN
    return lh_bit_shr(sizeof(lh_usize_t) == 8U
                          ? lh_bit_scan_forward_u64(mask)
                          : lh_bit_scan_forward_u32(lh_cast_static(lh_u32_t, mask)),
                      3U);
#else
    return lh_bit_shr(sizeof(lh_usize_t) == 8U
                          ? lh_bit_scan_reverse_u64(mask)
                          : lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, mask)),
                      3U);
#endif
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_BIT_SWAR_H */
