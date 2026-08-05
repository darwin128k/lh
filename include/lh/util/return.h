/**
 * @file return.h
 * @brief Macros for immediate function return with a specified value.
 *
 * Provides utilities for early exit from functions with a return value,
 * useful for error handling and early return scenarios.
 *
 * - `lh_return` — immediate return with a value
 * - `lh_return_if` — return if condition is true
 * - `lh_return_ifn` — return if condition is false
 */

#ifndef LH_UTIL_RETURN_H
#define LH_UTIL_RETURN_H

/**
 * @def lh_return
 * @brief Terminate execution with a specified return value.
 *
 * Performs an immediate exit from the current function while returning
 * the specified value from any point in the code.
 *
 * @param ... Value to be returned from the function.
 *
 * @note Use for early function termination with a return value,
 *       particularly useful for error handling.
 */
#define lh_return(...) return __VA_ARGS__

/**
 * @def lh_return_if(expr, ...)
 * @brief Return if @p expr is true.
 *
 * Evaluates @p expr and immediately returns the specified value
 * if the expression evaluates to true.
 *
 * @param expr Condition; if true, triggers return.
 * @param ... Value to return if @p expr is true.
 *
 * Example:
 * @code{.c}
 * lh_usize_t find_index(lh_usize_t const* arr, lh_usize_t len, lh_usize_t target) {
 *     for (lh_usize_t i = 0; i < len; ++i) {
 *         lh_return_if(arr[i] == target, i);
 *     }
 *     return len;
 * }
 * @endcode
 */
#define lh_return_if(expr, ...)                                                                    \
    if (expr)                                                                                      \
    lh_return(__VA_ARGS__)

/**
 * @def lh_return_ifn(expr, ...)
 * @brief Return if @p expr is false.
 *
 * Evaluates @p expr and immediately returns the specified value
 * if the expression evaluates to false.
 *
 * @param expr Condition; if false, triggers return.
 * @param ... Value to return if @p expr is false.
 *
 * Example:
 * @code{.c}
 * lh_str_ptr parse_token(lh_str_ptr input) {
 *     lh_return_ifn(input.len > 0, lh_str_ptr_invalid());
 *     // ... continue parsing
 *     return result;
 * }
 * @endcode
 */
#define lh_return_ifn(expr, ...) lh_return_if(!(expr), __VA_ARGS__)

#endif /* LH_UTIL_RETURN_H */