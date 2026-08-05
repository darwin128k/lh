/**
 * @file type.h
 * @brief Generic type inspection and casting utilities.
 *
 * Provides a small set of macros for querying properties of a type
 * (size, bit width, signedness) and producing typed constant values,
 * without relying on C11 _Generic or compiler extensions.
 */

#ifndef LH_UTIL_TYPE_H
#define LH_UTIL_TYPE_H

#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/util/bit.h>

/**
 * @def lh_type_size(T)
 * @brief Size of type @p T in bytes.
 *
 * Thin wrapper around `sizeof`.
 * Expands to a `size_t` constant expression.
 *
 * @param T Any complete type.
 *
 * @code{.c}
 * lh_type_size(int);      // → 4  (on most platforms)
 * lh_type_size(double);   // → 8
 * @endcode
 */
#define lh_type_size(T) sizeof(T)

/**
 * @def lh_type_bits(T)
 * @brief Bit width of type @p T.
 *
 * Equivalent to `sizeof(T) * LH_CHAR_BITS` (see `lh/config.h`).
 * Expands to a `size_t` constant expression.
 *
 * @param T Any complete type.
 *
 * @code{.c}
 * lh_type_bits(lh_uchar_t);   // → 8
 * lh_type_bits(lh_uint_t);    // → 32
 * @endcode
 */
#define lh_type_bits(T) (lh_type_size(T) * LH_CHAR_BITS)

/**
 * @def lh_type_cast(T, expr)
 * @brief Cast @p expr to type @p T.
 *
 * Uses `static_cast<T>` in C++ and a C-style cast in C.
 * Both @p T and @p expr are evaluated exactly once.
 *
 * @param T    Target type.
 * @param expr Expression to cast.
 *
 * @code{.c}
 * lh_type_cast(lh_uint_t, -1);   // → 4294967295u
 * lh_type_cast(float, 42);       // → 42.0f
 * @endcode
 */
#define lh_type_cast(T, expr) lh_cast_static(T, expr)

/**
 * @def lh_type_zero_v(T)
 * @brief Zero constant of type @p T.
 *
 * @param T Any scalar type (integer, floating-point, or pointer).
 *
 * @code{.c}
 * lh_type_zero_v(lh_uint_t);    // → (lh_uint_t)(0)
 * lh_type_zero_v(float);        // → (float)(0)
 * lh_type_zero_v(void *);       // → (void *)(0)  — null pointer
 * @endcode
 */
#define lh_type_zero_v(T) lh_type_cast(T, 0)

/**
 * @def lh_type_one_v(T)
 * @brief One constant of type @p T.
 *
 * @param T Any numeric type.
 *
 * @code{.c}
 * lh_type_one_v(lh_uint_t);    // → (lh_uint_t)(1)
 * lh_type_one_v(float);        // → (float)(1)
 * @endcode
 */
#define lh_type_one_v(T) lh_type_cast(T, 1)

/**
 * @def lh_type_neg_one_v(T)
 * @brief Negative one constant of type @p T.
 *
 * For signed types expands to -1.
 * For unsigned types the value wraps to the maximum representable value (e.g. 255 for
 * `lh_uchar_t`).
 *
 * @param T Any numeric type.
 *
 * @code{.c}
 * lh_type_neg_one_v(lh_sint_t);    // → (lh_sint_t)(-1)  →  -1
 * lh_type_neg_one_v(lh_uchar_t);   // → (lh_uchar_t)(-1) → 255
 * @endcode
 */
#define lh_type_neg_one_v(T) lh_type_cast(T, -1)

/**
 * @def lh_type_is_signed(T)
 * @brief Compile-time check whether type @p T is signed.
 *
 * Expands to a compile-time constant integer expression: 1 if @p T is
 * a signed type, 0 otherwise.
 * Based on the fact that casting -1 to an unsigned type yields a value greater than 0.
 *
 * @note The result is a constant expression usable in `if`, `static_assert`,
 *       and array sizes, but NOT in `#if` directives.
 *
 * @param T Any numeric type.
 *
 * @code{.c}
 * lh_type_is_signed(lh_sint_t);    // → 1
 * lh_type_is_signed(lh_uchar_t);   // → 0
 * @endcode
 */
#define lh_type_is_signed(T) lh_math_gt(lh_type_zero_v(T), lh_type_neg_one_v(T))

/**
 * @def lh_type_is_unsigned(T)
 * @brief Compile-time check whether type @p T is unsigned.
 *
 * Logical negation of ::lh_type_is_signed.
 * Expands to 1 if @p T is unsigned, 0 otherwise.
 *
 * @note The result is a constant expression usable in `if`, `static_assert`,
 *       and array sizes, but NOT in `#if` directives.
 *
 * @param T Any numeric type.
 *
 * @code{.c}
 * lh_type_is_unsigned(lh_uchar_t);   // → 1
 * lh_type_is_unsigned(lh_sint_t);    // → 0
 * @endcode
 */
#define lh_type_is_unsigned(T) (!lh_type_is_signed(T))

/**
 * @def lh_type_msb_pos(T)
 * @brief Bit index of the most significant bit of type @p T.
 *
 * Equivalent to `lh_type_bits(T) - 1`.
 * For signed types this is the position of the sign bit.
 *
 * @param T Any complete numeric type.
 *
 * @code{.c}
 * lh_type_msb_pos(lh_uchar_t);   // → 7
 * lh_type_msb_pos(lh_uint_t);    // → 31
 * lh_type_msb_pos(lh_ullong_t);  // → 63
 * @endcode
 */
#define lh_type_msb_pos(T) lh_math_sub_one(lh_type_bits(T))

/**
 * @def lh_type_msb(T)
 * @brief Value of type @p T with only the most significant bit set.
 *
 * Equivalent to `(T)1 << lh_type_msb_pos(T)`.
 * For signed types this is the minimum representable value (the sign bit);
 * for unsigned types it is half the range plus one.
 *
 * @param T Any complete numeric type.
 *
 * @code{.c}
 * lh_type_msb(lh_uchar_t);    // → 0x80       (128u)
 * lh_type_msb(lh_uint_t);     // → 0x80000000 (2147483648u)
 * lh_type_msb(lh_sint_t);     // → 0x80000000 (INT_MIN, -2147483648)
 * @endcode
 */
#define lh_type_msb(T) lh_bit_shl(lh_type_one_v(T), lh_type_msb_pos(T))

#endif /* LH_UTIL_TYPE_H */
