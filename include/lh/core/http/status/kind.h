/**
 * @file kind.h
 * @brief Named HTTP response status codes used by LH.
 *
 * These names cover the statuses used by the web host. HTTP permits other
 * three-digit status codes, which remain accepted by the response formatter.
 */

#ifndef LH_HTTP_STATUS_KIND_H
#define LH_HTTP_STATUS_KIND_H

/** @brief Known HTTP status codes. */
typedef enum lh_http_status_kind
{
    lh_http_status_continue = 100,
    lh_http_status_switching_protocols = 101,
    lh_http_status_ok = 200,
    lh_http_status_no_content = 204,
    lh_http_status_reset_content = 205,
    lh_http_status_not_modified = 304,
    lh_http_status_bad_request = 400,
    lh_http_status_not_found = 404,
    lh_http_status_content_too_large = 413,
    lh_http_status_unsupported_media_type = 415,
    lh_http_status_expectation_failed = 417,
    lh_http_status_internal_server_error = 500
} lh_http_status_kind_t;

#endif /* LH_HTTP_STATUS_KIND_H */
