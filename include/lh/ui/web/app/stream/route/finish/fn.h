/**
 * @file fn.h
 * @brief Commit or abort a staged streaming request.
 *
 * The host calls exactly one finish callback after staging. Commit is called
 * only after validating the entire framed body; abort discards staged changes.
 * Request, fields and socket are borrowed for the call.
 */

#ifndef LH_UI_WEB_APP_STREAM_ROUTE_FINISH_FN_H
#define LH_UI_WEB_APP_STREAM_ROUTE_FINISH_FN_H

#include <lh/core/http/header.h>
#include <lh/core/http/request.h>
#include <lh/os/net/socket.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Commit or abort staged changes and optionally send a response.
 * @param ctx Borrowed application context containing staged state.
 * @param request Parsed request borrowed for this call.
 * @param headers Parsed fields, count stored in @p request, borrowed.
 * @param client Connected response socket borrowed for this call.
 * @pre All pointers are valid. The callback does not retain borrowed data.
 */
typedef lh_void(lh_ui_web_stream_route_finish_fn)(lh_ptr ctx, const lh_http_request_t *request,
                                                  const lh_http_header_t *headers,
                                                  lh_os_net_socket_t *client);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_APP_STREAM_ROUTE_FINISH_FN_H */
