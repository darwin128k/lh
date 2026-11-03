/**
 * @file fn.h
 * @brief Handler function type for a selected streaming web route.
 *
 * The handler is called once after the request head and framing are accepted.
 * It reads decoded body bytes through the borrowed LH reader and sends its
 * response on the borrowed socket. It must handle a later read error; output
 * already acted upon may need rollback. The host closes after it returns.
 */

#ifndef LH_UI_WEB_APP_STREAM_ROUTE_FN_H
#define LH_UI_WEB_APP_STREAM_ROUTE_FN_H

#include <lh/http/header.h>
#include <lh/http/request.h>
#include <lh/os/net/socket.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Handle one request selected by a streaming route.
 * @param ctx Borrowed application context.
 * @param request Parsed request borrowing the host's head buffer.
 * @param headers Parsed header fields, count stored in @p request.
 * @param body Decoded body reader, valid only during this call.
 * @param client Connected response socket, valid only during this call.
 * @pre All pointers are valid and the handler does not retain borrowed data.
 */
typedef lh_void(lh_ui_web_stream_route_fn)(lh_ptr ctx, const lh_http_request_t *request,
                                           const lh_http_header_t *headers, lh_io_reader_t *body,
                                           lh_os_net_socket_t *client);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_APP_STREAM_ROUTE_FN_H */
