/**
 * @file kind.h
 * @brief Named HTTP methods understood by LH's method conversion.
 *
 * HTTP permits extension method tokens; these names are a convenience and
 * do not restrict parsing or route matching to this list.
 */

#ifndef LH_HTTP_METHOD_KIND_H
#define LH_HTTP_METHOD_KIND_H

/** @brief Known case-sensitive HTTP method tokens. */
typedef enum lh_http_method_kind
{
    lh_http_method_get,
    lh_http_method_head,
    lh_http_method_post,
    lh_http_method_put,
    lh_http_method_delete,
    lh_http_method_connect,
    lh_http_method_options,
    lh_http_method_trace,
    lh_http_method_patch
} lh_http_method_kind_t;

#endif /* LH_HTTP_METHOD_KIND_H */
