/**
 * @file bit.h
 * @brief Bitwise helpers under the `lh_bit_*` name.
 *
 * **Bitwise** operations work on the binary representation of integers:
 * which bits are 0 or 1, and how to combine or move them.
 *
 * Typical uses are flags (set/clear/test bits in a mask),
 * cheap integer scaling by powers of two via shifts,
 * and low-level protocols or hardware registers.
 *
 * Each macro here is exactly the corresponding `lh_math_bit_*` macro from
 * `lh/util/math.h` (same expansion, same evaluation and undefined-behavior rules).
 *
 * Include this header when you prefer the `lh_bit_*` prefix;
 * you still pull in `lh/util/math.h` transitively.
 */

#ifndef LH_UTIL_BIT_H
#define LH_UTIL_BIT_H

#include <lh/util/math.h>

/**
 * @def lh_bit_mask(n)
 * @brief Unsigned mask with a single one-bit at index @p n (`(1u << (n))`).
 * @param n Bit index; same constraints as ::lh_math_bit_mask().
 *
 * @code{.c}
 * unsigned f = lh_bit_or(lh_bit_mask(2), lh_bit_mask(0)); // 0x05u
 * @endcode
 *
 * @see lh_math_bit_mask()
 */
#define lh_bit_mask(n) lh_math_bit_mask(n)

/**
 * @def lh_bit_and(a, b)
 * @brief Bitwise AND (`a & b`).
 * @param a First operand.
 * @param b Second operand.
 *
 * Example usage:
 * @code{.c}
 * unsigned flags = 0x0Cu;
 * unsigned mask  = 0x04u;
 * unsigned only_masked_bits = lh_bit_and(flags, mask); // 0x04u
 * @endcode
 *
 * @see lh_math_bit_and(), lh_bit_disjoint()
 */
#define lh_bit_and(a, b) lh_math_bit_and(a, b)

/**
 * @def lh_bit_disjoint(a, b)
 * @brief Non-zero if @p a and @p b share no one-bits (`(a & b) == 0`).
 *
 * Example usage:
 * @code{.c}
 * lh_byte_t state = ...;
 * if (lh_bit_disjoint(state, lh_memory_view_state_uninitialized)) { ... }
 * @endcode
 *
 * @see lh_math_bit_disjoint()
 */
#define lh_bit_disjoint(a, b) lh_math_bit_disjoint(a, b)

/**
 * @def lh_bit_or(a, b)
 * @brief Bitwise OR (`a | b`).
 * @param a First operand.
 * @param b Second operand.
 *
 * Example usage:
 * @code{.c}
 * unsigned flags = 0x08u;
 * flags = lh_bit_or(flags, 0x01u); // set low bit
 * @endcode
 *
 * @see lh_math_bit_or(), lh_bit_set()
 */
#define lh_bit_or(a, b) lh_math_bit_or(a, b)

/**
 * @def lh_bit_set(a, b)
 * @brief Set bits in @p a using mask @p b (`a |= b`).
 * @param a Modifiable lvalue updated in place.
 * @param b Bits to turn on in @p a.
 *
 * Example usage:
 * @code{.c}
 * unsigned flags = 0x08u;
 * lh_bit_set(flags, 0x01u); // set low bit; flags is now 0x09u
 * @endcode
 *
 * @see lh_math_bit_set()
 */
#define lh_bit_set(a, b) lh_math_bit_set(a, b)

/**
 * @def lh_bit_xor(a, b)
 * @brief Bitwise exclusive OR (`a ^ b`).
 * @param a First operand.
 * @param b Second operand.
 *
 * Example usage:
 * @code{.c}
 * unsigned x = 0x0Fu;
 * x = lh_bit_xor(x, 0x03u); // flip the two lowest bits
 * @endcode
 *
 * @see lh_math_bit_xor()
 */
#define lh_bit_xor(a, b) lh_math_bit_xor(a, b)

/**
 * @def lh_bit_not(a)
 * @brief Bitwise NOT / one's complement (`~a`).
 * @param a Value to invert (promotion rules apply; see ::lh_math_bit_not()).
 *
 * Example usage:
 * @code{.c}
 * unsigned mask = 0x00FFu;
 * unsigned inv  = lh_bit_not(mask); // inverts bits in promoted width
 * @endcode
 *
 * @see lh_math_bit_not()
 */
#define lh_bit_not(a) lh_math_bit_not(a)

/**
 * @def lh_bit_shl(a, n)
 * @brief Left shift by `n` bits (`a << n`).
 * @param a Value to shift.
 * @param n Shift count; invalid ranges are undefined behavior (see ::lh_math_bit_shl()).
 *
 * Example usage:
 * @code{.c}
 * unsigned one = 1u;
 * unsigned bit3 = lh_bit_shl(one, 3); // 8: single-bit mask for bit 3
 * @endcode
 *
 * @see lh_math_bit_shl()
 */
#define lh_bit_shl(a, n) lh_math_bit_shl(a, n)

/**
 * @def lh_bit_shr(a, n)
 * @brief Right shift by `n` bits (`a >> n`).
 * @param a Value to shift.
 * @param n Shift count; invalid ranges are undefined behavior (see ::lh_math_bit_shr()).
 *
 * Example usage:
 * @code{.c}
 * unsigned v = 64u;
 * unsigned q = lh_bit_shr(v, 4); // 4: unsigned logical shift
 * @endcode
 *
 * @see lh_math_bit_shr()
 */
#define lh_bit_shr(a, n) lh_math_bit_shr(a, n)

#endif // LH_UTIL_BIT_H
