/**
 * @file arg.h
 * @brief Macro argument utilities (token concatenation, count).
 */

#ifndef LH_UTIL_ARG_H
#define LH_UTIL_ARG_H

/**
 * @def lh_arg_concat_x(a, b)
 * @brief Concatenate two preprocessing tokens without expanding arguments first.
 *
 * @param a Left token.
 * @param b Right token.
 */
#define lh_arg_concat_x(a, b) a##b

/**
 * @def lh_arg_concat(a, b)
 * @brief Concatenate two preprocessing tokens after macro argument expansion.
 *
 * @param a Left token.
 * @param b Right token.
 */
#define lh_arg_concat(a, b) lh_arg_concat_x(a, b)

/**
 * @def lh_arg_get_count_x(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)
 * @brief Implementation helper for ::lh_arg_get_count.
 */
#define lh_arg_get_count_x(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

/**
 * @def lh_arg_get_count(...)
 * @brief Count macro arguments in the range 1..10.
 *
 * Expands to the number of supplied arguments. Empty argument lists are not
 * supported.
 */
#define lh_arg_get_count(...) lh_arg_get_count_x(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#endif // LH_UTIL_ARG_H
