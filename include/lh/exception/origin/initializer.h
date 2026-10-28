/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_exception_origin_t.
 */

#ifndef LH_EXCEPTION_ORIGIN_INITIALIZER_H
#define LH_EXCEPTION_ORIGIN_INITIALIZER_H

#include <lh/null.h>

/**
 * @def lh_exception_origin_initializer(file, function, condition, message, line)
 * @brief Produces a brace-enclosed initializer for ::lh_exception_origin_t.
 *
 * Expands to ::lh_initializer with the arguments in struct order.
 *
 * Every argument is a compile-time constant in a check, so the result fits a
 * `static const` origin.
 *
 * @param file      Source file (`__FILE__`), or ::lh_null.
 * @param function  Enclosing function (`__func__`), or ::lh_null.
 * @param condition Text of the condition that held, or ::lh_null.
 * @param message   Message given at the check, or ::lh_null.
 * @param line      Source line (`__LINE__`), or `0`.
 *
 * Example usage:
 * @code{.c}
 * static const lh_exception_origin_t origin =
 *     lh_exception_origin_initializer(__FILE__,              // file
 *                                     __func__,              // function
 *                                     "x > 0",               // condition
 *                                     "x must be positive",  // message
 *                                     __LINE__);             // line
 * @endcode
 *
 * @see lh_initializer
 * @see lh_exception_origin_t
 */
#define lh_exception_origin_initializer(file, function, condition, message, line)                  \
    lh_initializer(file, function, condition, message, line)

/**
 * @def lh_exception_origin_empty_initializer()
 * @brief Initializer of an origin with nothing filled in.
 *
 * Expands to ::lh_exception_origin_initializer with every text ::lh_null and
 * @c line `0` — what the report level leaves of the members it drops.
 *
 * Example usage:
 * @code{.c}
 * static const lh_exception_origin_t origin = lh_exception_origin_empty_initializer();
 * @endcode
 *
 * @see lh_exception_origin_initializer
 * @see LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT
 */
#define lh_exception_origin_empty_initializer()                                                    \
    lh_exception_origin_initializer(lh_null, lh_null, lh_null, lh_null, 0)

#endif /* LH_EXCEPTION_ORIGIN_INITIALIZER_H */
