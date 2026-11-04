/**
 * @file status.c
 * @brief HTTP status validity and reason phrases.
 */

#include <lh/core/http/status.h>

#include <lh/core/util/interval.h>

lh_bool_t
lh_http_status_is_valid(lh_uint_t status)
{
    return lh_interval_closed_contains_value(100U, 599U, status);
}

lh_bool_t
lh_http_status_is_success(lh_uint_t status)
{
    return lh_interval_closed_contains_value(200U, 299U, status);
}

lh_bool_t
lh_http_status_is_content_forbidden(lh_uint_t status)
{
    return lh_interval_closed_contains_value(100U, 199U, status) ||
           status == lh_http_status_no_content || status == lh_http_status_reset_content ||
           status == lh_http_status_not_modified;
}

lh_str_view_t
lh_http_status_get_reason(lh_uint_t status)
{
    switch (status)
    {
    case lh_http_status_continue:
        return lh_str_view_lit("Continue");
    case lh_http_status_switching_protocols:
        return lh_str_view_lit("Switching Protocols");
    case lh_http_status_ok:
        return lh_str_view_lit("OK");
    case lh_http_status_no_content:
        return lh_str_view_lit("No Content");
    case lh_http_status_reset_content:
        return lh_str_view_lit("Reset Content");
    case lh_http_status_not_modified:
        return lh_str_view_lit("Not Modified");
    case lh_http_status_bad_request:
        return lh_str_view_lit("Bad Request");
    case lh_http_status_not_found:
        return lh_str_view_lit("Not Found");
    case lh_http_status_content_too_large:
        return lh_str_view_lit("Content Too Large");
    case lh_http_status_unsupported_media_type:
        return lh_str_view_lit("Unsupported Media Type");
    case lh_http_status_expectation_failed:
        return lh_str_view_lit("Expectation Failed");
    case lh_http_status_internal_server_error:
        return lh_str_view_lit("Internal Server Error");
    default:
        return lh_str_view_lit("");
    }
}
