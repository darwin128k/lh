/**
 * @file origin.h
 * @brief Where an exception comes from (::lh_exception_origin_t) — the
 *        runtime check that failed.
 *
 * Each check owns one `static const` origin built entirely from
 * compile-time constants, so passing it costs nothing until the check
 * fails. Which members are filled depends on
 * ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT; the others are ::lh_null / `0`.
 * The error code is not part of it: it is passed next to the
 * origin, so it is reported at every level and may be computed at run time.
 */

#ifndef LH_EXCEPTION_ORIGIN_H
#define LH_EXCEPTION_ORIGIN_H

#include <lh/numeric/types.h>
#include <lh/str/ptr.h>

/**
 * @struct lh_exception_origin
 * @brief Location, message and (at the full level) text of the runtime
 *        check an exception comes from.
 */
struct lh_exception_origin
{
    const lh_str_ptr file;      /**< Source file (`__FILE__`); LOCATION and FULL. */
    const lh_str_ptr function;  /**< Enclosing function (`__func__`); FULL only. */
    const lh_str_ptr condition; /**< Text of the condition that held; FULL only. */
    const lh_str_ptr message;   /**< Message given at the check, or ::lh_null; LOCATION and FULL. */
    lh_uint_t line;             /**< Source line (`__LINE__`); LOCATION and FULL. */
};
/** @copybrief lh_exception_origin */
typedef struct lh_exception_origin lh_exception_origin_t;

#endif /* LH_EXCEPTION_ORIGIN_H */
