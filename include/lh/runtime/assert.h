/**
 * @file assert.h
 * @brief Runtime assertion macros that throw by description on failure.
 */

#ifndef LH_RUNTIME_ASSERT_H
#define LH_RUNTIME_ASSERT_H

#include <lh/runtime/throw/by/desc.h>

/**
 * @def lh_runtime_assert_if(expr, desc)
 * @brief Throw when @p expr evaluates to true.
 *
 * Expands to a plain @c if statement
 * that calls ::lh_runtime_throw_by_desc(@p desc).
 *
 * @param expr Condition that triggers the throw when true.
 * @param desc C string description forwarded to ::lh_runtime_throw_by_desc.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_assert_if(ptr == lh_null, "null pointer");
 * @endcode
 *
 * @see lh_runtime_throw_by_desc
 */
#define lh_runtime_assert_if(expr, desc)                                                           \
    if (expr)                                                                                      \
    lh_runtime_throw_by_desc(desc)

/**
 * @def lh_runtime_assert_ifn(expr, desc)
 * @brief Throw when @p expr evaluates to false.
 *
 * Equivalent to ::lh_runtime_assert_if(!(@p expr), @p desc).
 *
 * @param expr Condition that must be true to continue.
 * @param desc C string description used when assertion fails.
 *
 * @see lh_runtime_assert_if
 */
#define lh_runtime_assert_ifn(expr, desc) lh_runtime_assert_if(!(expr), desc)

/**
 * @def lh_runtime_assert(...)
 * @brief Variadic alias for ::lh_runtime_assert_ifn(expr, desc).
 *
 * Accepts two arguments: @c expr and @c desc.
 */
#define lh_runtime_assert(...) lh_runtime_assert_ifn(__VA_ARGS__)

#endif // LH_RUNTIME_ASSERT_H