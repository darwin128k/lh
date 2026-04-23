/**
 * @file code.h
 * @brief Throw helper that supplies only an error code to ::lh_runtime_throw.
 */

#ifndef LH_RUNTIME_THROW_BY_CODE_H
#define LH_RUNTIME_THROW_BY_CODE_H

#include <lh/runtime/throw.h>

/**
 * @def lh_runtime_throw_with_code(code)
 * @brief Throw a runtime exception whose ::lh_error_t carries @p code.
 *
 * Expands to ::lh_runtime_throw(@p code).
 * Arguments beyond the code (e.g. a description) are not passed;
 * use ::lh_runtime_throw for a full initializer list or ::lh_runtime_throw_by_desc
 * when only a message is needed with a fixed interrupt code.
 *
 * @param code Error code (::lh_error_code_t / ::lh_runtime_error_code_t)
 *             stored in the thrown exception.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_throw_with_code(lh_runtime_error_code_null_pointer);
 * @endcode
 *
 * @see lh_runtime_throw
 * @see lh_runtime_throw_by_desc
 */
#define lh_runtime_throw_with_code(code) lh_runtime_throw(code)

#endif // LH_RUNTIME_THROW_BY_CODE_H
