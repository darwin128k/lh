/**
 * @file addr.h
 * @brief Address / pointer utilities: address-of wrapper (::lh_addr_of),
 * integer addresses, ranges, and alignment.
 *
 * Alignment helpers use bitmask formulas (`align - 1`, complement)
 * and therefore require `align` to be a positive power of two.
 *
 * Operands are passed through ::lh_math_* and
 * ::lh_bit_* so overflow and semantics follow those utilities.
 *
 * @note In ISO C, bitwise `&` / `~` require integer operands. For a `void *` (or other pointer),
 *       convert to ::lh_uaddr_t first, then apply the alignment helpers, then cast back if needed.
 */

#ifndef LH_UTIL_ADDR_H
#define LH_UTIL_ADDR_H

#include <lh/addr.h>

/**
 * @def lh_addr_of
 * @brief Address-of wrapper: expands to @c &expr.
 * @param expr Lvalue (or other expression valid for unary @c &) whose address is required.
 *
 * @code{.c}
 * int n = 0;
 * int *p = lh_addr_of(n); // same as &n; useful inside macro-heavy expressions
 * @endcode
 */
#define lh_addr_of(expr) &expr

/**
 * @def lh_addr_eq(a, b)
 * @brief Non-zero if @p a is equal to @p b (same as ::lh_math_eq).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x1000;
 * lh_uaddr_t b = (lh_uaddr_t)0x1000;
 * if (lh_addr_eq(a, b)) {
 *     // true
 * }
 * @endcode
 */
#define lh_addr_eq(a, b) lh_math_eq(a, b)

/**
 * @def lh_addr_ne(a, b)
 * @brief Non-zero if @p a is not equal to @p b (same as ::lh_math_ne).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x1000;
 * lh_uaddr_t b = (lh_uaddr_t)0x2000;
 * if (lh_addr_ne(a, b)) {
 *     // true
 * }
 * @endcode
 */
#define lh_addr_ne(a, b) lh_math_ne(a, b)

/**
 * @def lh_addr_lt(a, b)
 * @brief Non-zero if @p a is strictly less than @p b (same as ::lh_math_lt).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x1000;
 * lh_uaddr_t b = (lh_uaddr_t)0x2000;
 * if (lh_addr_lt(a, b)) {
 *     // true
 * }
 * @endcode
 */
#define lh_addr_lt(a, b) lh_math_lt(a, b)

/**
 * @def lh_addr_le(a, b)
 * @brief Non-zero if @p a is less than or equal to @p b (same as ::lh_math_le).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x1000;
 * lh_uaddr_t b = (lh_uaddr_t)0x2000;
 * if (lh_addr_le(a, b)) {
 *     // true
 * }
 * @endcode
 */
#define lh_addr_le(a, b) lh_math_le(a, b)

/**
 * @def lh_addr_gt(a, b)
 * @brief Non-zero if @p a is strictly greater than @p b (same as ::lh_math_gt).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x2000;
 * lh_uaddr_t b = (lh_uaddr_t)0x1000;
 * if (lh_addr_gt(a, b)) {
 *     // true
 * }
 * @endcode
 */
#define lh_addr_gt(a, b) lh_math_gt(a, b)

/**
 * @def lh_addr_ge(a, b)
 * @brief Non-zero if @p a is greater than or equal to @p b (same as ::lh_math_ge).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x2000;
 * lh_uaddr_t b = (lh_uaddr_t)0x1000;
 * if (lh_addr_ge(a, b)) {
 *     // true
 * }
 * @endcode
 */
#define lh_addr_ge(a, b) lh_math_ge(a, b)

/**
 * @def lh_addr_diff(lhs, rhs)
 * @brief Difference between two address values (::lh_math_sub).
 *
 * @param lhs First operand (minuend).
 * @param rhs Second operand (subtrahend).
 *
 * @return Result of subtraction per ::lh_math_sub.
 *
 * @code{.c}
 * lh_uaddr_t lo = (lh_uaddr_t)0x1000;
 * lh_uaddr_t hi = (lh_uaddr_t)0x1020;
 * lh_uaddr_t span = lh_addr_diff(hi, lo); // 0x20
 * @endcode
 */
#define lh_addr_diff(lhs, rhs) lh_math_sub(lhs, rhs)

/**
 * @def lh_addr_add(addr, delta)
 * @brief Add `delta` to an address-sized integer (::lh_math_add).
 *
 * @param addr  Base address as an integer.
 * @param delta Byte (or unit) offset to add.
 *
 * @return Sum, with wraparound rules of the promoted integer type.
 *
 * @code{.c}
 * lh_uaddr_t ptr = (lh_uaddr_t)0x1000;
 * lh_uaddr_t next = lh_addr_add(ptr, 32); // 0x1020
 * @endcode
 */
#define lh_addr_add(addr, delta) lh_math_add((addr), (delta))

/**
 * @def lh_addr_sub(addr, delta)
 * @brief Subtract `delta` from an address-sized integer (::lh_math_sub).
 *
 * @param addr  Address as an integer.
 * @param delta Offset to subtract.
 *
 * @return Difference, with wraparound rules of the promoted integer type.
 *
 * @code{.c}
 * lh_uaddr_t ptr = (lh_uaddr_t)0x1020;
 * lh_uaddr_t prev = lh_addr_sub(ptr, 32); // 0x1000
 * @endcode
 */
#define lh_addr_sub(addr, delta) lh_math_sub((addr), (delta))

/**
 * @def lh_addr_min(a, b)
 * @brief Smaller of two address-sized values (::lh_math_min).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x1000;
 * lh_uaddr_t b = (lh_uaddr_t)0x2000;
 * lh_uaddr_t min = lh_addr_min(a, b); // 0x1000
 * @endcode
 */
#define lh_addr_min(a, b) lh_math_min((a), (b))

/**
 * @def lh_addr_max(a, b)
 * @brief Larger of two address-sized values (::lh_math_max).
 *
 * @code{.c}
 * lh_uaddr_t a = (lh_uaddr_t)0x1000;
 * lh_uaddr_t b = (lh_uaddr_t)0x2000;
 * lh_uaddr_t max = lh_addr_max(a, b); // 0x2000
 * @endcode
 */
#define lh_addr_max(a, b) lh_math_max((a), (b))

/**
 * @def lh_addr_clamp(v, lo, hi)
 * @brief Clamp `v` to the inclusive range `[lo, hi]` (::lh_math_clamp). Requires `lo <= hi`.
 *
 * @code{.c}
 * lh_uaddr_t v = lh_addr_clamp(0x1500, 0x1000, 0x2000); // 0x1500
 * lh_uaddr_t below = lh_addr_clamp(0x0500, 0x1000, 0x2000); // 0x1000
 * lh_uaddr_t above = lh_addr_clamp(0x3000, 0x1000, 0x2000); // 0x2000
 * @endcode
 */
#define lh_addr_clamp(v, lo, hi) lh_math_clamp((v), (lo), (hi))

/**
 * @def lh_addr_align_by_offset(addr, align)
 * @brief Offset of `addr` within a block aligned to `align` (`addr & (align - 1)`).
 *
 * @param addr  Address or integer treated as an address.
 * @param align Alignment; must be a power of two and at least `1`.
 *
 * @return Remainder in the range `[0, align)` when `align` is a power of two.
 *
 * @code{.c}
 * lh_uaddr_t addr = (lh_uaddr_t)0x1034;
 * lh_uaddr_t r = lh_addr_align_by_offset(addr, 16); // 4
 * @endcode
 */
#define lh_addr_align_by_offset(addr, align) lh_bit_and(addr, lh_math_sub_one(align))

/**
 * @def lh_addr_is_aligned(addr, align)
 * @brief True if `addr` is a multiple of `align` (zero offset).
 *
 * @param addr  Address or integer.
 * @param align Power-of-two alignment.
 *
 * @return Non-zero if aligned, zero otherwise.
 *
 * @code{.c}
 * lh_uaddr_t u = (lh_uaddr_t)0x1040;
 * if (lh_addr_is_aligned(u, 64)) {
 *     // u is on a 64-byte boundary
 * }
 * @endcode
 */
#define lh_addr_is_aligned(addr, align) (lh_addr_align_by_offset(addr, align) == 0)

/**
 * @def lh_addr_align_up(addr, align)
 * @brief Smallest value `>= addr` that is a multiple of `align`.
 *
 * @param addr  Address or integer.
 * @param align Power-of-two alignment.
 *
 * @return Rounded-up address.
 *
 * @code{.c}
 * lh_uaddr_t heap = (lh_uaddr_t)0x1003;
 * lh_uaddr_t block = lh_addr_align_up(heap, (lh_uaddr_t)4096); // 0x2000 (example)
 * @endcode
 */
#define lh_addr_align_up(addr, align)                                                              \
    lh_bit_and(lh_math_add(addr, lh_math_sub_one(align)), lh_bit_not(lh_math_sub_one(align)))

/**
 * @def lh_addr_align_down(addr, align)
 * @brief Largest value `<= addr` that is a multiple of `align`.
 *
 * @param addr  Address or integer.
 * @param align Power-of-two alignment.
 *
 * @return Rounded-down address.
 *
 * @code{.c}
 * lh_uaddr_t lo = (lh_uaddr_t)0x1000;
 * lh_uaddr_t hi = (lh_uaddr_t)0x9500;
 * lh_uaddr_t span = lh_addr_diff(hi, lo);
 * lh_uaddr_t chunk = lh_addr_align_down(span, (lh_uaddr_t)4096); // 0x8000
 * @endcode
 */
#define lh_addr_align_down(addr, align) lh_bit_and(addr, lh_bit_not(lh_math_sub_one(align)))

#endif /* LH_UTIL_ADDR_H */