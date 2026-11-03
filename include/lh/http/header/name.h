/**
 * @file name.h
 * @brief Names of HTTP fields shared by LH's framing and web transport.
 *
 * Each macro yields a borrowed view of a protocol field name with static
 * storage. HTTP field names compare without ASCII case. Other field names
 * remain possible: callers may supply their own views to the header API.
 */

#ifndef LH_HTTP_HEADER_NAME_H
#define LH_HTTP_HEADER_NAME_H

#include <lh/str/view.h>

/** The Content-Type field name. */
#define lh_http_header_name_content_type lh_str_view_lit("Content-Type")

/** The Content-Encoding field name. */
#define lh_http_header_name_content_encoding lh_str_view_lit("Content-Encoding")

/** The Content-Length field name. */
#define lh_http_header_name_content_length lh_str_view_lit("Content-Length")

/** The Transfer-Encoding field name. */
#define lh_http_header_name_transfer_encoding lh_str_view_lit("Transfer-Encoding")

/** The Expect field name. */
#define lh_http_header_name_expect lh_str_view_lit("Expect")

/** The Connection field name. */
#define lh_http_header_name_connection lh_str_view_lit("Connection")

/** The Cache-Control field name. */
#define lh_http_header_name_cache_control lh_str_view_lit("Cache-Control")

/** The Upgrade field name. */
#define lh_http_header_name_upgrade lh_str_view_lit("Upgrade")

/** The Sec-WebSocket-Key field name. */
#define lh_http_header_name_websocket_key lh_str_view_lit("Sec-WebSocket-Key")

/** The Sec-WebSocket-Accept field name. */
#define lh_http_header_name_websocket_accept lh_str_view_lit("Sec-WebSocket-Accept")

#endif /* LH_HTTP_HEADER_NAME_H */
