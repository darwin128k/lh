/**
 * @file math.h
 * @brief Arithmetic, comparison, and bitwise operation macros.
 *
 * Provides expression-level wrappers for common operations.
 * All operands are fully parenthesized to avoid precedence issues at call sites.
 */

#ifndef LH_UTIL_MATH_H
#define LH_UTIL_MATH_H

/* ── arithmetic ────────────────────────────────────────────────────────── */

/**
 * @def lh_math_add(a, b)
 * @brief Addition of two values
 * @param a first operand
 * @param b second operand
 * @return sum of a and b  (a + b)
 *
 * Equivalent to standard C operator `+`
 */
#define lh_math_add(a, b) ((a) + (b))

/**
 * @def lh_math_sub(a, b)
 * @brief Subtraction of two values
 * @param a minuend
 * @param b subtrahend
 * @return difference  (a - b)
 *
 * Equivalent to standard C operator `-`
 */
#define lh_math_sub(a, b) ((a) - (b))

/**
 * @def lh_math_mul(a, b)
 * @brief Multiplication of two values
 * @param a first factor
 * @param b second factor
 * @return product  (a * b)
 *
 * Equivalent to standard C operator `*`
 */
#define lh_math_mul(a, b) ((a) * (b))

/**
 * @def lh_math_div(a, b)
 * @brief Division of two values
 * @param a dividend
 * @param b divisor
 * @return quotient  (a / b)
 * @warning Division by zero causes undefined behavior (same as native / operator)
 *
 * Equivalent to standard C operator `/`
 */
#define lh_math_div(a, b) ((a) / (b))

/**
 * @def lh_math_mod(a, b)
 * @brief Modulo (remainder) operation
 * @param a dividend
 * @param b divisor
 * @return remainder after division  (a % b)
 * @warning Division by zero causes undefined behavior
 * @note The sign of the result follows the dividend (implementation-defined before C99,
 *       but in C99 and later — sign matches dividend)
 *
 * Equivalent to standard C operator `%`
 */
#define lh_math_mod(a, b) ((a) % (b))

/**
 * @def lh_math_neg(a)
 * @brief Unary negation (change sign)
 * @param a value to negate
 * @return negated value  (-a)
 *
 * Equivalent to standard C unary operator `-`
 */
#define lh_math_neg(a) (-(a))

/**
 * @def lh_math_abs(a)
 * @brief Absolute value (magnitude)
 * @param a value to get absolute value of
 * @return absolute value of @p a
 *
 * Returns @p a if non-negative, otherwise returns negated @p a.
 *
 * @see lh_math_neg(), lh_math_is_negative()
 */
#define lh_math_abs(a) lh_math_is_negative(a) ? lh_math_neg(a) : (a)

/* ── increment / decrement ─────────────────────────────────────────────── */

/**
 * @def lh_math_add_one(a)
 * @brief Increments the value by one
 * @param a value to increment
 * @return a + 1
 *
 * Equivalent to the expression `(a) + 1`
 *
 * @see lh_math_add(), lh_math_sub_one()
 */
#define lh_math_add_one(a) ((a) + 1)

/**
 * @def lh_math_sub_one(a)
 * @brief Decrements the value by one
 * @param a value to decrement
 * @return a - 1
 *
 * Equivalent to the expression `(a) - 1`
 *
 * @see lh_math_sub(), lh_math_add_one()
 */
#define lh_math_sub_one(a) ((a) - 1)

/* ── comparison / selection ────────────────────────────────────────────── */

/**
 * @def lh_math_eq(a, b)
 * @brief Non-zero if @p a is equal to @p b (`a == b`).
 */
#define lh_math_eq(a, b) ((a) == (b))

/**
 * @def lh_math_ne(a, b)
 * @brief Non-zero if @p a is not equal to @p b (`a != b`).
 */
#define lh_math_ne(a, b) ((a) != (b))

/**
 * @def lh_math_lt(a, b)
 * @brief Non-zero if @p a is strictly less than @p b (`a < b`).
 */
#define lh_math_lt(a, b) ((a) < (b))

/**
 * @def lh_math_le(a, b)
 * @brief Non-zero if @p a is less than or equal to @p b (`a <= b`).
 */
#define lh_math_le(a, b) ((a) <= (b))

/**
 * @def lh_math_gt(a, b)
 * @brief Non-zero if @p a is strictly greater than @p b (`a > b`).
 */
#define lh_math_gt(a, b) ((a) > (b))

/**
 * @def lh_math_ge(a, b)
 * @brief Non-zero if @p a is greater than or equal to @p b (`a >= b`).
 */
#define lh_math_ge(a, b) ((a) >= (b))

/**
 * @def lh_math_min(a, b)
 * @brief Returns the smaller of two values
 * @param a first value to compare
 * @param b second value to compare
 * @return the smaller of a and b
 *
 * Equivalent to the ternary expression `lh_math_lt((a), (b)) ? (a) : (b)`
 *
 * @note If a and b are equal, returns either value (typically a)
 * @see lh_math_max(), lh_math_clamp()
 */
#define lh_math_min(a, b) (lh_math_lt((a), (b)) ? (a) : (b))

/**
 * @def lh_math_max(a, b)
 * @brief Returns the larger of two values
 * @param a first value to compare
 * @param b second value to compare
 * @return the larger of a and b
 *
 * Equivalent to the ternary expression `lh_math_gt((a), (b)) ? (a) : (b)`
 *
 * @note If a and b are equal, returns either value (typically a)
 * @see lh_math_min(), lh_math_clamp()
 */
#define lh_math_max(a, b) (lh_math_gt((a), (b)) ? (a) : (b))

/**
 * @def lh_math_clamp(v, lo, hi)
 * @brief Constrains a value to be within the range [lo, hi]
 * @param v value to clamp
 * @param lo lower bound (inclusive)
 * @param hi upper bound (inclusive)
 * @return v if lo ≤ v ≤ hi, lo if v < lo, hi if v > hi
 *
 * Implemented as: `lh_math_min(lh_math_max((v), (lo)), (hi))`
 *
 * @note Assumes lo ≤ hi.
 *       Behavior is undefined if lo > hi.
 * @warning Arguments may be evaluated multiple times — avoid expressions
 *          with side effects (e.g. `lh_math_clamp(x++, 0, 100)`).
 *
 * @see lh_math_min(), lh_math_max()
 */
#define lh_math_clamp(v, lo, hi) lh_math_min(lh_math_max((v), (lo)), (hi))

/**
 * @def lh_math_is_zero(a)
 * @brief Non-zero if @p a compares equal to zero (`(a) == 0`).
 * @param a Value after usual arithmetic conversions.
 * @return Non-zero if @p a is zero.
 *
 * @warning @p a may be evaluated more than once if used inside other macros that repeat it.
 *
 * @see lh_math_bit_disjoint()
 */
#define lh_math_is_zero(a) lh_math_eq((a), 0)

/**
 * @def lh_math_is_positive(a)
 * @brief Non-zero if @p a is strictly greater than zero (`(a) > 0`).
 * @param a Value after usual arithmetic conversions.
 * @return Non-zero if @p a compares greater than zero.
 *
 * For unsigned integer types, this is equivalent to non-zero (the only non-positive value is zero).
 * For signed integers, zero and negatives (including two’s-complement minimum) yield false.
 *
 * @warning @p a may be evaluated more than once if used inside other macros that repeat it.
 * @note Prefer this over `!lh_math_is_zero(a)` in predicates like ::lh_math_is_power_of_two when
 *       @p a may be signed: a single high bit in a negative value can satisfy the `n & (n-1)` trick
 *       while still not being a positive power of two.
 *
 * @see lh_math_is_negative(), lh_math_is_zero()
 */
#define lh_math_is_positive(a) lh_math_gt((a), 0)

/**
 * @def lh_math_is_negative(a)
 * @brief Non-zero if @p a is strictly less than zero (`(a) < 0`).
 * @param a Value after usual arithmetic conversions.
 * @return Non-zero if @p a compares less than zero.
 *
 * For unsigned integer types, the result is always false (no value is less than zero).
 * For signed integers, true exactly for negative values.
 *
 * @warning @p a may be evaluated more than once if used inside other macros that repeat it.
 *
 * @see lh_math_is_positive(), lh_math_is_zero()
 */
#define lh_math_is_negative(a) lh_math_lt((a), 0)

/* ── bitwise ───────────────────────────────────────────────────────────── */

/**
 * @def lh_math_bit_mask(n)
 * @brief Unsigned mask with a single one-bit at index @p n (`(1u << (n))`).
 * @param n Bit index (0 = lowest bit). If @p n is negative
 * or not less than the width of @c unsigned,
 * the shift has undefined behavior.
 *
 * @see lh_math_bit_shl()
 */
#define lh_math_bit_mask(n) ((1u) << (n))

/**
 * @def lh_math_bit_and(a, b)
 * @brief Bitwise AND of two values
 * @param a first operand
 * @param b second operand
 * @return bitwise AND of a and b  (a & b)
 *
 * Equivalent to the standard C bitwise AND operator `&`
 *
 * @see lh_math_bit_or(), lh_math_bit_xor(), lh_math_bit_not(), lh_math_bit_disjoint(),
 *      lh_math_bit_mask()
 */
#define lh_math_bit_and(a, b) ((a) & (b))

/**
 * @def lh_math_bit_disjoint(a, b)
 * @brief Non-zero if @p a and @p b share no one-bits (`(a & b) == 0`).
 *
 * Typical use: test that @p a has none of the bits from mask @p b set.
 *
 * @see lh_math_bit_and(), lh_math_is_zero()
 */
#define lh_math_bit_disjoint(a, b) lh_math_is_zero(lh_math_bit_and((a), (b)))

/**
 * @def lh_math_is_power_of_two(n)
 * @brief Non-zero if @p n is a positive integral power of two (1, 2, 4, …).
 * @param n Integer value after usual arithmetic conversions. Zero yields false.
 * @return Non-zero if @p n is strictly positive and `n & (n - 1)` is zero (one set bit in value).
 *
 * Typical use: validating an alignment value before masking with `align - 1`.
 *
 * @warning @p n may be evaluated more than once — avoid side effects.
 *
 * @see lh_math_is_positive(), lh_math_bit_disjoint(), lh_math_sub_one()
 */
#define lh_math_is_power_of_two(n)                                                                 \
    (lh_math_is_positive((n)) && lh_math_bit_disjoint((n), lh_math_sub_one(n)))

/**
 * @def lh_math_is_multiple_of(a, b)
 * @brief Non-zero if @p a is an exact multiple of @p b (`(a % b) == 0`).
 * @param a Dividend value.
 * @param b Divisor value.
 * @return Non-zero if remainder of @p a divided by @p b is zero.
 *
 * Typical use: validate size/alignment divisibility conditions.
 *
 * @warning Division by zero is undefined behavior; ensure @p b is non-zero.
 * @warning Arguments may be evaluated more than once through nested macros.
 *
 * @see lh_math_mod(), lh_math_is_zero()
 */
#define lh_math_is_multiple_of(a, b) lh_math_is_zero(lh_math_mod((a), (b)))

/**
 * @def lh_math_bit_or(a, b)
 * @brief Bitwise OR of two values
 * @param a first operand
 * @param b second operand
 * @return bitwise OR of a and b  (a | b)
 *
 * Equivalent to the standard C bitwise OR operator `|`
 *
 * @see lh_math_bit_and(), lh_math_bit_xor(), lh_math_bit_not(), lh_math_bit_set(),
 *      lh_math_bit_disjoint()
 */
#define lh_math_bit_or(a, b) ((a) | (b))

/**
 * @def lh_math_bit_set(a, b)
 * @brief Set bits in @p a using mask @p b (`a |= b`).
 * @param a Modifiable lvalue updated in place.
 * @param b Bits to turn on in @p a.
 * @return Value of @p a after assignment (same as the built-in `|=` operator).
 *
 * @see lh_math_bit_or()
 */
#define lh_math_bit_set(a, b) ((a) |= (b))

/**
 * @def lh_math_bit_xor(a, b)
 * @brief Bitwise XOR of two values
 * @param a first operand
 * @param b second operand
 * @return bitwise exclusive OR of a and b  (a ^ b)
 *
 * Equivalent to the standard C bitwise XOR operator `^`
 *
 * @see lh_math_bit_and(), lh_math_bit_or(), lh_math_bit_not()
 */
#define lh_math_bit_xor(a, b) ((a) ^ (b))

/**
 * @def lh_math_bit_not(a)
 * @brief Bitwise NOT (one's complement)
 * @param a value to invert
 * @return bitwise inversion of a  (~a)
 *
 * Equivalent to the standard C bitwise NOT operator `~`
 *
 * @note Result depends on the integer promotion rules and the width of the type.
 *       On most platforms inverts all bits in the promoted type.
 *
 * @see lh_math_bit_and(), lh_math_bit_or(), lh_math_bit_xor()
 */
#define lh_math_bit_not(a) (~(a))

/**
 * @def lh_math_bit_shl(a, n)
 * @brief Left shift (towards more significant bits)
 * @param a value to be shifted
 * @param n number of bit positions to shift left
 * @return a shifted left by n positions  (a << n)
 *
 * Equivalent to the standard C left shift operator `<<`
 *
 * @warning Behavior is undefined if n < 0 or n ≥ width of the promoted type of @p a.
 * @note For signed types, left-shifting into the sign bit is undefined behavior
 *       in C (prior to C23).
 *
 * @see lh_math_bit_shr()
 */
#define lh_math_bit_shl(a, n) ((a) << (n))

/**
 * @def lh_math_bit_shr(a, n)
 * @brief Right shift
 * @param a value to be shifted
 * @param n number of bit positions to shift right
 * @return a shifted right by n positions  (a >> n)
 *
 * Equivalent to the standard C right shift operator `>>`
 *
 * @warning Behavior is undefined if n < 0 or n ≥ width of the promoted type of @p a.
 * @note For signed integer types:
 *       - arithmetic (sign-extending) shift on most compilers when a is signed
 *       - logical (zero-filling) shift when a is unsigned
 *
 * @see lh_math_bit_shl()
 */
#define lh_math_bit_shr(a, n) ((a) >> (n))

/**
 * @def lh_math_add_over_max_exclusive(a, b, max)
 * @brief Non-zero if `a + b` exceeds strict upper bound @p max.
 *
 * Intended for overflow-style checks where @p max is exclusive.
 */
#define lh_math_add_over_max_exclusive(a, b, max)                                                  \
    (lh_math_is_positive(b) && lh_math_lt(lh_math_sub((max), (a)), (b)))

/**
 * @def lh_math_add_over_max_inclusive(a, b, max)
 * @brief Non-zero if `a + b` exceeds inclusive upper bound @p max.
 *
 * Inclusive variant: equality at @p max is treated as overflow.
 */
#define lh_math_add_over_max_inclusive(a, b, max)                                                  \
    (!lh_math_is_negative(b) && lh_math_le(lh_math_sub((max), (a)), (b)))

/**
 * @def lh_math_add_below_min_exclusive(a, b, min)
 * @brief Non-zero if `a + b` goes below strict lower bound @p min.
 *
 * Intended for checks where @p min is exclusive.
 */
#define lh_math_add_below_min_exclusive(a, b, min)                                                 \
    (lh_math_is_negative(b) && lh_math_gt(lh_math_sub((min), (a)), (b)))

/**
 * @def lh_math_add_below_min_inclusive(a, b, min)
 * @brief Non-zero if `a + b` goes below inclusive lower bound @p min.
 *
 * Inclusive variant: equality at @p min is treated as underflow.
 */
#define lh_math_add_below_min_inclusive(a, b, min)                                                 \
    (!lh_math_is_positive(b) && lh_math_ge(lh_math_sub((min), (a)), (b)))

#endif /* LH_UTIL_MATH_H */
