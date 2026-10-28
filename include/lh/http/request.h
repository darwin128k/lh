/**
 * @file request.h
 * @brief HTTP/1 request-line views and parsed header count.
 */

#ifndef LH_HTTP_REQUEST_H
#define LH_HTTP_REQUEST_H

#include <lh/str/view.h>
#include <lh/http/request/fields.h>

/**
 * @struct lh_http_request
 * @brief Parsed request line: method, target, version and field count.
 *
 * The views point into the caller's input buffer and expire with it.
 */
struct lh_http_request
{
    /** Members: ::lh_http_request_fields. */
    lh_http_request_fields(lh_str_view_t, lh_u8_t, lh_usize_t);
};
/** @copybrief lh_http_request */
typedef struct lh_http_request lh_http_request_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The @c method of @p self. */
const lh_str_view_t *
lh_http_request_get_method(const lh_http_request_t *self);

/** @brief Store @p method as the @c method of @p self. */
lh_void
lh_http_request_set_method(lh_http_request_t *self, lh_str_view_t method);

/** @brief The @c target of @p self. */
const lh_str_view_t *
lh_http_request_get_target(const lh_http_request_t *self);

/** @brief Store @p target as the @c target of @p self. */
lh_void
lh_http_request_set_target(lh_http_request_t *self, lh_str_view_t target);

/** @brief The @c version_minor of @p self. */
lh_u8_t
lh_http_request_get_version_minor(const lh_http_request_t *self);

/** @brief Store @p version_minor as the @c version_minor of @p self. */
lh_void
lh_http_request_set_version_minor(lh_http_request_t *self, lh_u8_t version_minor);

/** @brief The @c header_count of @p self. */
lh_usize_t
lh_http_request_get_header_count(const lh_http_request_t *self);

/** @brief Store @p header_count as the @c header_count of @p self. */
lh_void
lh_http_request_set_header_count(lh_http_request_t *self, lh_usize_t header_count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_REQUEST_H */
