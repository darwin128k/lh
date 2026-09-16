/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_werror_t.
 */

#ifndef LH_WERROR_INITIALIZER_H
#define LH_WERROR_INITIALIZER_H

#include <lh/error/code.h>
#include <lh/initializer.h>

/**
 * @def lh_werror_initializer(code, ...)
 * @brief Brace-enclosed initializer for ::lh_werror_t.
 */
#define lh_werror_initializer(code, ...) lh_initializer(code, __VA_ARGS__)

/**
 * @def lh_werror_empty_initializer(...)
 * @brief Cleared / success wide error (::lh_error_code_ok).
 */
#define lh_werror_empty_initializer(...) lh_werror_initializer(lh_error_code_ok, __VA_ARGS__)

#endif /* LH_WERROR_INITIALIZER_H */
