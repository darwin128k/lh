/**
 * @file raise.h
 * @brief Location-capturing glue between user-facing throw sites and ::lh_runtime_throw.
 *
 * ::lh_runtime_raise is the single public entry point for raising an exception.
 * It captures the call-site location (@c __FILE__, @c __LINE__, @c __FUNCTION__,
 * @c __TIMESTAMP__) by expanding a compound ::lh_exception_origin_t literal inline
 * at the macro expansion point, then delegates to ::lh_runtime_throw.
 *
 * In release builds (@c NDEBUG defined) origin capture is omitted:
 * the macro forwards @p error directly to the single-argument form of
 * ::lh_runtime_throw.
 *
 * Architecture layer order:
 * @code
 * lh_runtime_assert   (condition check)
 *        |
 * lh_runtime_raise    (location capture — this file)
 *        |
 * lh_runtime_throw    (exception assembly and stack unwind)
 * @endcode
 *
 * @see lh_runtime_throw
 * @see lh_exception_origin_t
 * @see lh_exception_origin_initializer_now
 */

#ifndef LH_RUNTIME_RAISE_H
#define LH_RUNTIME_RAISE_H

#include <lh/exception/origin/initializer.h>
#include <lh/runtime/throw.h>
#include <lh/clit.h>

/**
 * @def lh_runtime_raise(error)
 * @brief Raise an exception from the current source location.
 *
 * Captures the call-site origin metadata
 * (@c __TIMESTAMP__, @c __FILE__, @c __FUNCTION__, @c __LINE__)
 * via ::lh_exception_origin_initializer_now and passes the resulting
 * compound-literal pointer alongside @p error to ::lh_runtime_throw.
 *
 * Because location capture relies on preprocessor macros, this must remain
 * a macro — wrapping it in a function would record the wrapper's location,
 * not the caller's.
 *
 * In release builds (@c NDEBUG defined) the origin is omitted entirely and the
 * macro expands to the single-argument form of ::lh_runtime_throw.
 *
 * @param error Pointer to the ::lh_runtime_error_t describing the failure (must not be null).
 *
 * @note Does not return; see ::lh_runtime_throw.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_error_t err = lh_runtime_error_make_by_code(lh_runtime_error_code_null_pointer);
 * lh_runtime_raise(&err);
 * @endcode
 *
 * @see lh_runtime_throw
 * @see lh_runtime_rethrow
 * @see lh_exception_origin_initializer_now
 * @see lh_runtime_error_make
 * @see lh_runtime_error_make_by_code
 */
#ifndef NDEBUG
#    define lh_runtime_raise(error)                                                                \
        lh_runtime_throw(error,                                                                    \
                         lh_clit_of(lh_exception_origin_t, lh_exception_origin_initializer_now))
#else
#    define lh_runtime_raise(error) lh_runtime_throw(error)
#endif // NDEBUG

#endif // LH_RUNTIME_RAISE_H
