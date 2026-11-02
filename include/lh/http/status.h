/**
 * @file status.h
 * @brief HTTP status code validity and standard reason phrases.
 *
 * A status is the three-digit protocol number, independent of the optional
 * reason phrase. Named codes live in ::lh_http_status_kind_t; callers may use
 * other valid codes. Returned phrase views borrow static LH-owned storage.
 */

#ifndef LH_HTTP_STATUS_H
#define LH_HTTP_STATUS_H

#include <lh/http/status/kind.h>
#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether a number is an HTTP status in the range 100..599.
 * @param status Status number to inspect.
 * @return ::lh_bool_true when valid, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_http_status_is_valid(lh_uint_t status);

/**
 * @brief Test whether a status is in the successful 2xx class.
 * @param status Status number to inspect.
 * @return ::lh_bool_true for 200..299, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_http_status_is_success(lh_uint_t status);

/**
 * @brief Test whether the status itself forbids response content.
 *
 * Covers informational responses, 204, 205 and 304. Request-method rules are
 * separate: for example, HEAD also forbids content for otherwise valid codes.
 *
 * @param status Status number to inspect.
 * @return ::lh_bool_true for a status that forbids content, false otherwise.
 */
lh_bool_t
lh_http_status_is_content_forbidden(lh_uint_t status);

/**
 * @brief Get LH's standard reason phrase for a known status.
 *
 * @param status Status number, including codes without a named phrase.
 * @return A borrowed static view of the phrase, or an empty view for an
 *         unknown code. The caller must not free or modify its bytes.
 */
lh_str_view_t
lh_http_status_get_reason(lh_uint_t status);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_STATUS_H */
