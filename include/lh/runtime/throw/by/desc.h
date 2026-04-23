/**
 * @file desc.h
 * @brief Throw helper that fixes the runtime error code and supplies a description.
 */

#ifndef LH_RUNTIME_THROW_BY_DESC_H
#define LH_RUNTIME_THROW_BY_DESC_H

#include <lh/runtime/error/code.h>
#include <lh/runtime/throw.h>

/**
 * @def lh_runtime_throw_by_desc(desc)
 * @brief Throw a runtime exception with code ::lh_runtime_error_code_interrupt and @p desc.
 *
 * Expands to ::lh_runtime_throw(::lh_runtime_error_code_interrupt, @p desc).
 * The description is stored in the embedded ::lh_error_t (see ::lh_exception_initializer).
 *
 * @param desc C string passed through to the exception payload
 *             (same conventions as ::lh_runtime_throw).
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_throw_by_desc("user cancelled");
 * @endcode
 *
 * @see lh_runtime_throw
 * @see lh_runtime_error_code_interrupt
 */
#define lh_runtime_throw_by_desc(desc) lh_runtime_throw(lh_runtime_error_code_interrupt, desc)

#endif // LH_RUNTIME_THROW_BY_DESC_H
