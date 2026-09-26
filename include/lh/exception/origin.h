/**
 * @file origin.h
 * @brief Where an exception comes from (::lh_exception_origin_t) — the
 *        runtime check that failed.
 *
 * Each check owns one `static const` origin built entirely from
 * compile-time constants, so passing it costs nothing until the check
 * fails. Which members are filled depends on
 * ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT; the others are ::lh_null / `0`
 * / empty. The error code is not part of it: it is passed next to the
 * origin, so it is reported at every level and may be computed at run time.
 */

#ifndef LH_EXCEPTION_ORIGIN_H
#define LH_EXCEPTION_ORIGIN_H

#include <lh/numeric/types.h>
#include <lh/str/ptr.h>
#include <lh/str/view.h>

/**
 * @struct lh_exception_origin
 * @typedef lh_exception_origin_t
 * @brief Location, message and (at the full level) text of the runtime
 *        check an exception comes from.
 */
typedef struct lh_exception_origin
{
    lh_str_cptr file;      /**< Source file (`__FILE__`); LOCATION and FULL. */
    lh_str_cptr function;  /**< Enclosing function (`__func__`); FULL only. */
    lh_str_cptr condition; /**< Text of the condition that held; FULL only. */
    lh_str_view_t message; /**< Message given at the check, or empty; LOCATION and FULL. */
    lh_uint_t line;        /**< Source line (`__LINE__`); LOCATION and FULL. */
} lh_exception_origin_t;

#endif /* LH_EXCEPTION_ORIGIN_H */
