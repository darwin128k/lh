/**
 * @file fn.h
 * @brief Handler function type for a selected streaming web route.
 *
 * The handler is called once after the request head and framing are accepted.
 * It reads decoded body bytes through the borrowed LH reader and stages changes.
 * No staged changes may be published before the separate commit callback. The
 * host validates the remaining body and then calls either commit or abort.
 */

#ifndef LH_UI_WEB_APP_STREAM_ROUTE_FN_H
#define LH_UI_WEB_APP_STREAM_ROUTE_FN_H

#include <lh/http/header.h>
#include <lh/http/request.h>
#include <lh/io/reader.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Handle one request selected by a streaming route.
 * @param ctx Borrowed application context.
 * @param request Parsed request borrowing the host's head buffer.
 * @param headers Parsed header fields, count stored in @p request.
 * @param body Decoded body reader, valid only during this call.
 * @return True if staging succeeded, false to abort the transaction.
 * @pre All pointers are valid and the handler does not retain borrowed data.
 */
typedef lh_bool_t(lh_ui_web_stream_route_fn)(lh_ptr ctx, const lh_http_request_t *request,
                                             const lh_http_header_t *headers, lh_io_reader_t *body);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_APP_STREAM_ROUTE_FN_H */
