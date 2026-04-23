/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_exception_t.
 *
 * When **`NDEBUG` is not defined**, each macro appends
 * ::lh_exception_origin_initializer_now() after the error
 * initializer so ::lh_exception_t::origin is filled at the call site.
 *
 * In release builds (`NDEBUG` defined),
 * only the ::lh_error_t part is initialized,
 * matching the layout described in `lh/exception.h`.
 */

#ifndef LH_EXCEPTION_INITIALIZER_H
#define LH_EXCEPTION_INITIALIZER_H

#include <lh/error/initializer.h>
#include <lh/exception/origin/initializer.h>

#ifndef NDEBUG
/**
 * @def lh_exception_initializer(...)
 * @brief Produces a brace-enclosed initializer for ::lh_exception_t.
 *
 * Forwards the variadic list to ::lh_error_initializer.
 * When **`NDEBUG`** is not defined,
 * also supplies ::lh_exception_origin_initializer_now() as the `origin` field.
 *
 * When `NDEBUG` is defined, expands to only
 * ::lh_initializer(::lh_error_initializer(__VA_ARGS__)) (no `origin` member).
 *
 * @param ... Arguments for ::lh_error_initializer (code and optional tail).
 *
 * Example usage:
 * @code{.c}
 * lh_exception_t ex = lh_exception_initializer(LH_ERROR_CODE_OK, lh_null);
 * @endcode
 *
 * @see lh_error_initializer
 * @see lh_exception_origin_initializer_now
 * @see lh_exception_t
 */
#    define lh_exception_initializer(...)                                                          \
        lh_initializer(lh_error_initializer(__VA_ARGS__), lh_exception_origin_initializer_now())
#else
#    define lh_exception_initializer(...) lh_initializer(lh_error_initializer(__VA_ARGS__))
#endif

#ifndef NDEBUG
/**
 * @def lh_exception_empty_initializer()
 * @brief Initializer for a cleared / success exception (::LH_ERROR_CODE_OK).
 *
 * Wraps ::lh_error_empty_initializer().
 * In debug builds, also appends ::lh_exception_origin_initializer_now(); * in release (`NDEBUG`),
 * only the error sub-object is initialized.
 *
 * Example usage:
 * @code{.c}
 * lh_exception_t ok = lh_exception_empty_initializer();
 * @endcode
 *
 * @see lh_error_empty_initializer
 * @see lh_exception_origin_initializer_now
 * @see lh_exception_t
 */
#    define lh_exception_empty_initializer()                                                       \
        lh_initializer(lh_error_empty_initializer(), lh_exception_origin_initializer_now())
#else
#    define lh_exception_empty_initializer() lh_initializer(lh_error_empty_initializer())
#endif

#endif // LH_EXCEPTION_INITIALIZER_H