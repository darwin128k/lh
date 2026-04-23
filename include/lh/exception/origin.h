/**
 * @file origin.h
 * @brief Optional “where was this thrown?” metadata for ::lh_exception_t.
 *
 * In **debug** builds (`NDEBUG` not defined),
 * ::lh_exception_t carries an ::lh_exception_origin_t:
 * one **source site** (file, function, line, optional timestamp), not a full stack trace.
 * In **release** builds the field is stripped from ::lh_exception_t entirely,
 * so there is no size or string cost.
 *
 * File, function, and timestamp are stored as `const char *` * (typically string literals from
 * `__FILE__`, `__FUNCTION__`, `__TIMESTAMP__`).
 *
 * Those pointers must remain valid for as long as the origin is read.
 * `line` is a ::lh_usize_t (often `__LINE__`).
 *
 * Typical initializers: `lh/exception/origin/initializer.h`.
 */

#ifndef LH_EXCEPTION_ORIGIN_H
#define LH_EXCEPTION_ORIGIN_H

#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @struct lh_exception_origin
 * @brief One raise site: optional timestamp plus file, function, and line.
 *
 * Filled when constructing an exception for diagnostics.
 * String pointers may be null; `line` is a ::lh_usize_t (often from `__LINE__`).
 *
 * @see lh_exception_t
 * @see lh_exception_origin_initializer()
 * @see lh_exception_origin_initializer_now()
 */
typedef struct lh_exception_origin {
    lh_str_cptr timestamp; /**< Optional time stamp text (e.g. from `__TIMESTAMP__`). */
    lh_str_cptr filename;  /**< Source path fragment (often `__FILE__`). */
    lh_str_cptr function;  /**< Function name (often `__FUNCTION__` / `__func__`). */
    lh_usize_t line;       /**< Line number (typically `__LINE__`). */
} lh_exception_origin_t;   /**< Typedef for struct ::lh_exception_origin. */

#endif // LH_EXCEPTION_ORIGIN_H
