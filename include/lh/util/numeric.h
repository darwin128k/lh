/**
 * @file limit.h
 * @brief Compile-time numeric limit macros.
 *
 * Provides macros for computing the minimum and maximum representable
 * values of any integer type at compile time, plus sign-detection
 * predicates:
 *
 * - ::lh_numeric_limit_smin / ::lh_numeric_limit_smax - signed bounds.
 * - ::lh_numeric_limit_umin / ::lh_numeric_limit_umax - unsigned bounds.
 * - ::lh_numeric_limit_is_signed / ::lh_numeric_limit_is_unsigned.
 */

#ifndef LH_NUMERIC_LIMIT_H
#define LH_NUMERIC_LIMIT_H

#include <lh/util/type.h>

/**
 * @def lh_numeric_limit_smin
 * @brief Returns the minimum value for a signed numeric type `T`.
 *
 * This macro computes the minimum value of a signed type `T`
 * by casting the sign bit of `T` to `T`.
 *
 * @param T The numeric type (must be a signed integer type).
 * @return The minimum value of type T.
 */
#define lh_numeric_limit_smin(T) lh_type_cast(T, lh_type_msb(T))

/**
 * @def lh_numeric_limit_smax
 * @brief Returns the maximum value for a signed numeric type `T`.
 *
 * This macro computes the maximum value of a signed type `T`
 * by applying bitwise NOT to the minimum value of type `T`.
 *
 * @param T The numeric type (must be a signed integer type).
 * @return The maximum value of type T.
 */
#define lh_numeric_limit_smax(T) lh_bit_not(lh_numeric_limit_smin(T))

/**
 * @def lh_numeric_limit_umin
 * @brief Returns the minimum value for an unsigned numeric type `T`.
 *
 * This macro returns the zero value for type `T`,
 * which is the minimum value for an unsigned type.
 *
 * @param T The numeric type (must be an unsigned integer type).
 * @return The minimum value of type T (0).
 */
#define lh_numeric_limit_umin(T) lh_type_zero_v(T)

/**
 * @def lh_numeric_limit_umax
 * @brief Returns the maximum value for an unsigned numeric type `T`.
 *
 * This macro returns the maximum value of an unsigned type `T`
 * by getting the value with all bits set to 1.
 *
 * @param T The numeric type (must be an unsigned integer type).
 * @return The maximum value of type T.
 */
#define lh_numeric_limit_umax(T) lh_type_neg_one_v(T)

/**
 * @def lh_numeric_limit_is_signed
 * @brief Determines if the numeric type `T` is signed.
 *
 * This macro checks whether the type `T` is signed using
 * the underlying utility from "bit_util.h".
 *
 * @param T The numeric type to check.
 * @return `true` if `T` is a signed type, `false` if `T` is an unsigned type.
 */
#define lh_numeric_limit_is_signed(T) lh_type_is_signed(T)

/**
 * @def lh_numeric_limit_is_unsigned
 * @brief Determines if the numeric type `T` is unsigned.
 *
 * This macro checks whether the type `T` is unsigned using
 * the underlying utility from "bit_util.h".
 *
 * @param T The numeric type to check.
 *          Must be a fundamental arithmetic type.
 * @return `true` if `T` is an unsigned type, `false` if `T` is a signed type.
 *
 * @note This is an alias for `lh_type_is_unsigned(T)`
 *       and inherits all of its behavior and limitations.
 * @warning Not suitable for floating-point types or non-arithmetic types.
 */
#define lh_numeric_limit_is_unsigned(T) lh_type_is_unsigned(T)

/**
 * @def lh_numeric_limit_min
 * @brief Returns the minimum representable value for integer type `T`.
 *
 * This macro automatically selects the signed or unsigned branch based on
 * ::lh_numeric_limit_is_signed(T). It is useful for types whose signedness
 * may vary by target, such as plain `char`.
 *
 * @param T The integer type to evaluate.
 * @return The minimum value representable by type `T`.
 *
 * @code
 * lh_numeric_limit_min(char);            // auto-detected: signed or unsigned
 * lh_numeric_limit_min(unsigned short);  // 0
 * @endcode
 */
#define lh_numeric_limit_min(T)                                                                    \
    lh_type_cast(                                                                                  \
        T, (lh_numeric_limit_is_signed(T) ? lh_numeric_limit_smin(T) : lh_numeric_limit_umin(T)))

/**
 * @def lh_numeric_limit_max
 * @brief Returns the maximum representable value for integer type `T`.
 *
 * This macro automatically selects the signed or unsigned branch based on
 * ::lh_numeric_limit_is_signed(T). It is useful for types whose signedness
 * may vary by target, such as plain `char`.
 *
 * @param T The integer type to evaluate.
 * @return The maximum value representable by type `T`.
 *
 * @code
 * lh_numeric_limit_max(char);            // auto-detected: signed or unsigned
 * lh_numeric_limit_max(unsigned short);  // USHRT_MAX
 * @endcode
 */
#define lh_numeric_limit_max(T)                                                                    \
    lh_type_cast(                                                                                  \
        T, (lh_numeric_limit_is_signed(T) ? lh_numeric_limit_smax(T) : lh_numeric_limit_umax(T)))

#endif /* LH_NUMERIC_LIMIT_H */
