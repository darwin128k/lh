/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_exception_t.
 */

#ifndef LH_EXCEPTION_INITIALIZER_H
#define LH_EXCEPTION_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_exception_initializer(error, origin)
 * @brief Produces a brace-enclosed initializer for ::lh_exception_t.
 *
 * @param error  Initializer of the ::lh_runtime_error_t
 *               (::lh_runtime_error_initializer).
 * @param origin `const lh_exception_origin_t *`, or ::lh_null.
 *
 * Example usage:
 * @code{.c}
 * static const lh_exception_origin_t origin = {__FILE__, __func__, "x > 0",
 *                                              lh_str_view_empty_initializer(), __LINE__};
 * const lh_exception_t exception = lh_exception_initializer(
 *     lh_runtime_error_initializer(lh_runtime_error_code_invalid_argument,
 *                                  lh_str_view_initializer_lit("x must be positive")),
 *     &origin);
 * @endcode
 */
#define lh_exception_initializer(error, origin) lh_initializer(error, origin)

#endif /* LH_EXCEPTION_INITIALIZER_H */
