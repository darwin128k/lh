/**
 * @file if.h
 * @brief Defines macros for conditional early returns based on runtime checks.
 *
 * This header provides a set of macros to simplify conditional returns
 * from functions when specific runtime conditions are met or not met.
 *
 * - `lh_runtime_return_if`:
 *   Returns specified values if the given expression evaluates to true.
 *
 * - `lh_runtime_return_ifn`:
 *   Returns specified values if the given expression evaluates to false.
 *
 * These macros help reduce boilerplate code in functions requiring frequent condition checks,
 * providing a concise and readable way to handle early exits.
 *
 * @note When using these macros, ensure the return values match
 *       the expected return type of the enclosing function
 *       to prevent compilation issues.
 */
#ifndef LH_RUNTIME_RETURN_IF_H
#define LH_RUNTIME_RETURN_IF_H

#include <lh/runtime/return.h>

/**
 * @def lh_runtime_return_if
 * @brief Checks a condition and returns specified values if the condition is true.
 *
 * This macro evaluates the `expr` and immediately
 * returns the specified arguments (`__VA_ARGS__`)
 * if the expression evaluates to true.
 *
 * Typically used for early exits in functions
 * where certain conditions must be met to continue execution.
 *
 * @param expr Expression to evaluate.
 *             If true, the macro triggers a return.
 *
 * @param ... Values or expressions to return if `expr` is true.
 *            These must be valid return values/expressions for the host function.
 *
 * Example usage:
 * @code{.c}
 * lh_usize_t find_index(lh_usize_t const* arr, lh_usize_t len, lh_usize_t target) {
 *     for (lh_usize_t i = 0; i < len; ++i) {
 *         lh_runtime_return_if(arr[i] == target, i);
 *     }
 *     return len;
 * }
 * @endcode
 *
 * @note This macro simplifies early exits from functions when specific conditions are met,
 *       combining condition checks and return statements to reduce boilerplate.
 *
 * @warning Ensure return values (`__VA_ARGS__`) match the function's return type
 *          to avoid compilation errors or undefined behavior.
 */
#define lh_runtime_return_if(expr, ...)                                                            \
    if (expr)                                                                                      \
    lh_runtime_return(__VA_ARGS__)

/**
 * @def lh_runtime_return_ifn
 * @brief Checks if a condition is false and returns specified values in that case.
 *
 * This macro evaluates the `expr` and immediately
 * returns the specified arguments (`__VA_ARGS__`)
 * if the expression evaluates to false.
 *
 * Typically used for early exits when required conditions are not satisfied.
 *
 * @param expr Expression to evaluate.
 *             If false, the macro triggers a return.
 *
 * @param ... Values or expressions to return if `expr` is false.
 *            These must be valid return values/expressions for the host function.
 *
 * Example usage:
 * @code{.c}
 * lh_str_ptr parse_token(lh_str_ptr input) {
 *     lh_runtime_return_ifn(input.len > 0, lh_str_ptr_invalid());
 *     // ... continue parsing
 *     return result;
 * }
 * @endcode
 *
 * @note Useful for validating prerequisites in functions
 *       - if the condition fails (expr is false),
 *       the function returns the specified values.
 *
 * @warning Ensure return values (`__VA_ARGS__`)
 *          match the function's return type
 *          to prevent compilation errors or undefined behavior.
 */
#define lh_runtime_return_ifn(expr, ...) lh_runtime_return_if(!(expr), __VA_ARGS__)

#endif // LH_RUNTIME_RETURN_IF_H