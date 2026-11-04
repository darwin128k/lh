/**
 * @file method.c
 * @brief Known HTTP method token names.
 */

#include <lh/core/http/method.h>

lh_str_view_t
lh_http_method_get_name(lh_http_method_kind_t method)
{
    switch (method)
    {
    case lh_http_method_get:
        return lh_str_view_lit("GET");
    case lh_http_method_head:
        return lh_str_view_lit("HEAD");
    case lh_http_method_post:
        return lh_str_view_lit("POST");
    case lh_http_method_put:
        return lh_str_view_lit("PUT");
    case lh_http_method_delete:
        return lh_str_view_lit("DELETE");
    case lh_http_method_connect:
        return lh_str_view_lit("CONNECT");
    case lh_http_method_options:
        return lh_str_view_lit("OPTIONS");
    case lh_http_method_trace:
        return lh_str_view_lit("TRACE");
    case lh_http_method_patch:
        return lh_str_view_lit("PATCH");
    default:
        return lh_str_view_lit("");
    }
}
