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

/**
 * @brief The request method token, e.g. `GET`.
 * The view points into the parsed input and lives as long as that buffer.
 */
const lh_str_view_t *
lh_http_request_get_method(const lh_http_request_t *self);

/**
 * @brief Store the request method token, e.g. `GET`.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_method(lh_http_request_t *self, lh_str_view_t method);

/**
 * @brief The request target as raw URI text, e.g. `/maps/a.bsp?x=1`.
 * The view points into the parsed input and lives as long as that buffer.
 */
const lh_str_view_t *
lh_http_request_get_target(const lh_http_request_t *self);

/**
 * @brief Store the request target as raw URI text, e.g. `/maps/a.bsp?x=1`.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_target(lh_http_request_t *self, lh_str_view_t target);

/** @brief The minor digit of `HTTP/1.x` (0..9); which ones a service accepts is its policy. */
lh_u8_t
lh_http_request_get_version_minor(const lh_http_request_t *self);

/**
 * @brief Store the minor digit of `HTTP/1.x` (0..9); which ones a service accepts is its policy.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_version_minor(lh_http_request_t *self, lh_u8_t version_minor);

/** @brief Number of header fields the parse stored in the caller's array. */
lh_usize_t
lh_http_request_get_header_count(const lh_http_request_t *self);

/**
 * @brief Store number of header fields the parse stored in the caller's array.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_header_count(lh_http_request_t *self, lh_usize_t header_count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_REQUEST_H */
