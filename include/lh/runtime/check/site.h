/**
 * @file site.h
 * @brief Where a runtime check sits (::lh_runtime_check_site_t) — what a
 *        failed check hands to its handler.
 *
 * Each check site owns one `static const` instance built entirely from
 * compile-time constants, so passing it costs nothing until a check
 * fails. Which members are filled depends on
 * ::LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT; the others are ::lh_null / `0`.
 */

#ifndef LH_RUNTIME_CHECK_SITE_H
#define LH_RUNTIME_CHECK_SITE_H

#include <lh/numeric/types.h>
#include <lh/str/ptr.h>

/**
 * @struct lh_runtime_check_site
 * @typedef lh_runtime_check_site_t
 * @brief Location (and, at the full level, text) of one runtime check.
 */
typedef struct lh_runtime_check_site
{
    lh_str_cptr file;      /**< Source file (`__FILE__`); LOCATION and FULL. */
    lh_str_cptr function;  /**< Enclosing function (`__func__`); FULL only. */
    lh_str_cptr condition; /**< Text of the condition that held; FULL only. */
    lh_uint_t line;        /**< Source line (`__LINE__`); LOCATION and FULL. */
} lh_runtime_check_site_t;

#endif /* LH_RUNTIME_CHECK_SITE_H */
