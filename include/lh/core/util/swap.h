/**
 * @file swap.h
 * @brief Swap two values in place.
 */

#ifndef LH_UTIL_SWAP_H
#define LH_UTIL_SWAP_H

/**
 * @brief Swap two values using a temporary variable.
 *
 * @param T Type of both values.
 * @param a First lvalue to swap.
 * @param b Second lvalue to swap.
 *
 * @code
 * int x = 10;
 * int y = 20;
 * lh_swap(int, x, y);
 * // x == 20, y == 10
 * @endcode
 */
#define lh_swap(T, a, b)                                                                           \
    do                                                                                             \
    {                                                                                              \
        T temp = (a);                                                                              \
        (a) = (b);                                                                                 \
        (b) = temp;                                                                                \
    } while (0)

/**
 * @brief Swap two integer-like values using XOR (without a temporary variable).
 *
 * @param a First integer lvalue to swap.
 * @param b Second integer lvalue to swap.
 *
 * @warning
 * Do not use when @p a and @p b refer to the same object.
 * Only valid for integer-like types that support XOR operations.
 *
 * @code
 * unsigned a = 1U;
 * unsigned b = 2U;
 * lh_swap_no_temp(a, b);
 * // a == 2U, b == 1U
 * @endcode
 */
#define lh_swap_no_temp(a, b)                                                                      \
    do                                                                                             \
    {                                                                                              \
        (a) ^= (b);                                                                                \
        (b) ^= (a);                                                                                \
        (a) ^= (b);                                                                                \
    } while (0)

#endif /* LH_UTIL_SWAP_H */
