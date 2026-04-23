/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_exception_origin_t.
 *
 * Produces a comma-separated initializer list
 * in struct field order (`timestamp`, `filename`, `function`, `line`).
 *
 *Typical call sites pass preprocessor symbols such as `__FILE__` and `__LINE__`;
 * those literals must outlive any use of the stored pointers (see `lh/exception/origin.h`).
 */

#ifndef LH_EXCEPTION_ORIGIN_INITIALIZER_H
#define LH_EXCEPTION_ORIGIN_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_exception_origin_initializer(timestamp, filename, function, line)
 * @brief Produces a brace-enclosed initializer for ::lh_exception_origin_t.
 *
 * Expands to ::lh_initializer(@p timestamp, @p filename, @p function, @p line)
 * in the same order as the struct members.
 *
 * @param timestamp  Time stamp text (often `__TIMESTAMP__` or a string literal).
 * @param filename   Source file identifier (often `__FILE__`).
 * @param function   Function name (often `__FUNCTION__` or `__func__`).
 * @param line       Line number (often `__LINE__`).
 *
 * Example usage:
 * @code{.c}
 * lh_exception_origin_t o
 *     = lh_exception_origin_initializer(__TIMESTAMP__, __FILE__, __FUNCTION__, __LINE__);
 * @endcode
 *
 * @see lh_exception_origin_t
 * @see lh_initializer
 * @see lh_exception_origin_initializer_now()
 */
#define lh_exception_origin_initializer(timestamp, filename, function, line)                       \
    lh_initializer(timestamp, filename, function, line)

/**
 * @def lh_exception_origin_initializer_now()
 * @brief Shorthand origin initializer using the current preprocessing site.
 *
 * Expands to
 * ::lh_exception_origin_initializer(@c __TIMESTAMP__, @c __FILE__, @c __FUNCTION__, @c __LINE__).
 * Use where you build the exception so metadata refers to that source location.
 *
 * Example usage:
 * @code{.c}
 * lh_exception_origin_t here = lh_exception_origin_initializer_now();
 * @endcode
 *
 * @see lh_exception_origin_initializer
 * @see lh_exception_origin_t
 */
#define lh_exception_origin_initializer_now()                                                      \
    lh_exception_origin_initializer(__TIMESTAMP__, __FILE__, __FUNCTION__, __LINE__)

#endif // LH_EXCEPTION_ORIGIN_INITIALIZER_H