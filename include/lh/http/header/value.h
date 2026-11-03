/**
 * @file value.h
 * @brief Common HTTP field values used by the web transport.
 *
 * These macros yield borrowed views of static protocol tokens. A caller may
 * use other values, including lists of tokens, through the HTTP header API.
 */

#ifndef LH_HTTP_HEADER_VALUE_H
#define LH_HTTP_HEADER_VALUE_H

#include <lh/str/view.h>

/** The gzip coding. */
#define lh_http_header_value_gzip lh_str_view_lit("gzip")

/** The identity coding. */
#define lh_http_header_value_identity lh_str_view_lit("identity")

/** The chunked transfer coding. */
#define lh_http_header_value_chunked lh_str_view_lit("chunked")

/** The HTTP 100-continue expectation. */
#define lh_http_header_value_continue lh_str_view_lit("100-continue")

/** Close the connection after the response. */
#define lh_http_header_value_close lh_str_view_lit("close")

/** Forbid caches from storing the response. */
#define lh_http_header_value_no_store lh_str_view_lit("no-store")

/** WebSocket protocol name in an Upgrade field. */
#define lh_http_header_value_websocket lh_str_view_lit("websocket")

/** Upgrade token in a Connection field. */
#define lh_http_header_value_upgrade lh_str_view_lit("Upgrade")

#endif /* LH_HTTP_HEADER_VALUE_H */
