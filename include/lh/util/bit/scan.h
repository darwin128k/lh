/**
 * @file scan.h
 * @brief Locate the lowest/highest set bit in an 8/16/32/64-bit word.
 *
 * Portable wrapper over `__builtin_ctz{,l,ll}`/`__builtin_clz{,l,ll}` (GCC/Clang)
 * and `_BitScanForward{,64}`/`_BitScanReverse{,64}` (MSVC) — same two operations,
 * different name and calling convention per compiler. Unlike `lh/cpu/simd.h`, this
 * is not x86-specific: both underlying mechanisms are available on every
 * architecture either compiler targets (ARM included), so there is no runtime
 * CPU-feature check here, only a compile-time choice of which compiler intrinsic
 * to call.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE: mask scans like these show
 * up in tight loops (SIMD compare-mismatch resolution, bitset iteration, flag
 * enumeration), so a real cross-TU call — one or two machine instructions
 * wrapped in call/ret overhead — would cost more than the operation itself.
 *
 * The u8/u16 variants widen to u32 and reuse the u32 intrinsic directly: for
 * both ctz (forward) and clz-based reverse, zero-extending into a wider word
 * never changes the position of the lowest or highest set bit, so no width
 * adjustment is needed on the result either.
 */

#ifndef LH_UTIL_BIT_SCAN_H
#define LH_UTIL_BIT_SCAN_H

#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/compiler/arch.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/type.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>

#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#    include <intrin.h>
#    include <lh/numeric/types.h> /* lh_ulong_t, matching _BitScan{Forward,Reverse}'s own index type */
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Index of the lowest set bit in @p x (0 = least significant bit).
 *
 * @param x Value to scan. Must be nonzero — the result is undefined if @p x is 0
 *          (same precondition the underlying compiler intrinsics have).
 * @return Index (0-31) of the lowest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_forward_u32(lh_u32_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, __builtin_ctz(x));
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    lh_ulong_t index;
    _BitScanForward(&index, x);
    return lh_cast_static(lh_usize_t, index);
#else
    /* Portable fallback for any other standards-conforming compiler: x is nonzero
     * per this function's own precondition, so this always terminates. */
    lh_usize_t i = 0U;
    while (((x >> i) & 1U) == 0U)
    {
        ++i;
    }
    return i;
#endif
}

/**
 * @brief Index of the highest set bit in @p x (0 = least significant bit).
 *
 * @param x Value to scan. Must be nonzero — the result is undefined if @p x is 0
 *          (same precondition the underlying compiler intrinsics have).
 * @return Index (0-31) of the highest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_reverse_u32(lh_u32_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, 31 - __builtin_clz(x));
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    lh_ulong_t index;
    _BitScanReverse(&index, x);
    return lh_cast_static(lh_usize_t, index);
#else
    lh_usize_t i = 31U;
    while (((x >> i) & 1U) == 0U)
    {
        --i;
    }
    return i;
#endif
}

/**
 * @brief Index of the lowest set bit in @p x (0 = least significant bit).
 * @param x Value to scan; must be nonzero.
 * @return Index (0-7) of the lowest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_forward_u8(lh_u8_t x)
{
    return lh_bit_scan_forward_u32(lh_cast_static(lh_u32_t, x));
}

/**
 * @brief Index of the highest set bit in @p x (0 = least significant bit).
 * @param x Value to scan; must be nonzero.
 * @return Index (0-7) of the highest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_reverse_u8(lh_u8_t x)
{
    return lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, x));
}

/**
 * @brief Index of the lowest set bit in @p x (0 = least significant bit).
 * @param x Value to scan; must be nonzero.
 * @return Index (0-15) of the lowest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_forward_u16(lh_u16_t x)
{
    return lh_bit_scan_forward_u32(lh_cast_static(lh_u32_t, x));
}

/**
 * @brief Index of the highest set bit in @p x (0 = least significant bit).
 * @param x Value to scan; must be nonzero.
 * @return Index (0-15) of the highest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_reverse_u16(lh_u16_t x)
{
    return lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, x));
}

/**
 * @brief Index of the lowest set bit in @p x (0 = least significant bit).
 *
 * @param x Value to scan; must be nonzero.
 * @return Index (0-63) of the lowest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_forward_u64(lh_u64_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, __builtin_ctzll(x));
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#    if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64
    lh_ulong_t index;
    _BitScanForward64(&index, x);
    return lh_cast_static(lh_usize_t, index);
#    else
    /* No _BitScanForward64 on 32-bit MSVC targets: compose it from two 32-bit
     * scans instead — check the low half first, only fall back to the high half
     * (offset by 32) if the low half is entirely zero. */
    {
        const lh_u32_t lo = lh_cast_static(lh_u32_t, x & 0xFFFFFFFFULL);
        if (lo != 0U)
        {
            return lh_bit_scan_forward_u32(lo);
        }
        return 32U + lh_bit_scan_forward_u32(lh_cast_static(lh_u32_t, x >> 32));
    }
#    endif
#else
    lh_usize_t i = 0U;
    while (((x >> i) & 1U) == 0U)
    {
        ++i;
    }
    return i;
#endif
}

/**
 * @brief Index of the highest set bit in @p x (0 = least significant bit).
 *
 * @param x Value to scan; must be nonzero.
 * @return Index (0-63) of the highest set bit in @p x.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_usize_t
lh_bit_scan_reverse_u64(lh_u64_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, 63 - __builtin_clzll(x));
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#    if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64
    lh_ulong_t index;
    _BitScanReverse64(&index, x);
    return lh_cast_static(lh_usize_t, index);
#    else
    {
        const lh_u32_t hi = lh_cast_static(lh_u32_t, x >> 32);
        if (hi != 0U)
        {
            return 32U + lh_bit_scan_reverse_u32(hi);
        }
        return lh_bit_scan_reverse_u32(lh_cast_static(lh_u32_t, x & 0xFFFFFFFFULL));
    }
#    endif
#else
    lh_usize_t i = 63U;
    while (((x >> i) & 1U) == 0U)
    {
        --i;
    }
    return i;
#endif
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_BIT_SCAN_H */
