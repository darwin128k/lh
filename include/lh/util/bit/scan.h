/**
 * @file scan.h
 * @brief Locate the lowest/highest set bit in a 32-bit word.
 *
 * Portable wrapper over `__builtin_ctz`/`__builtin_clz` (GCC/Clang) and
 * `_BitScanForward`/`_BitScanReverse` (MSVC) — same two operations, different name
 * and calling convention per compiler. Unlike `lh/cpu/simd.h`, this is not x86-
 * specific: both underlying mechanisms are available on every architecture either
 * compiler targets (ARM included), so there is no runtime CPU-feature check here,
 * only a compile-time choice of which compiler intrinsic to call.
 */

#ifndef LH_UTIL_BIT_SCAN_H
#define LH_UTIL_BIT_SCAN_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Index of the lowest set bit in @p x (0 = least significant bit).
 *
 * @param x Value to scan. Must be nonzero — the result is undefined if @p x is 0
 *          (same precondition `__builtin_ctz` / `_BitScanForward` themselves have).
 *
 * @return Index (0-31) of the lowest set bit in @p x.
 *
 * Example usage:
 * @code{.c}
 * lh_usize_t i = lh_bit_scan_forward(0x00000100u); // 8
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_bit_scan_forward(lh_u32_t x);

/**
 * @brief Index of the highest set bit in @p x (0 = least significant bit).
 *
 * @param x Value to scan. Must be nonzero — the result is undefined if @p x is 0
 *          (same precondition `__builtin_clz` / `_BitScanReverse` themselves have).
 *
 * @return Index (0-31) of the highest set bit in @p x.
 *
 * Example usage:
 * @code{.c}
 * lh_usize_t i = lh_bit_scan_reverse(0x00000180u); // 8
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_bit_scan_reverse(lh_u32_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_BIT_SCAN_H */
