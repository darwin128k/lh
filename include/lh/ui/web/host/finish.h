/**
 * @file finish.h
 * @brief The end of a streaming request: commit or abort, never both.
 *
 * A streaming route stages what its handler reads and publishes it only when
 * the whole body has arrived and validated. Whatever drove the body — a
 * blocking reader or the host's step-by-step pipe — ends it here, with exactly
 * one of the route's two finish callbacks.
 */

#ifndef LH_UI_WEB_HOST_FINISH_H
#define LH_UI_WEB_HOST_FINISH_H

#include <lh/core/http/parser.h>
#include <lh/ui/web/app.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Call the route's commit (when @p committed) or its abort, with the reply socket.
 * @param app       Application; its context is passed to the callback.
 * @param route     The streaming route the request matched.
 * @param request   Parsed request, borrowed.
 * @param headers   Parsed fields, count in @p request, borrowed.
 * @param client    Connected socket the callback may reply on, borrowed.
 * @param committed ::lh_bool_true after the body validated to its end.
 * @return @p committed.
 */
lh_bool_t
lh_ui_web_host_finish_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                             const lh_http_request_t *request, const lh_http_header_t *headers,
                             lh_os_net_socket_t *client, lh_bool_t committed);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_FINISH_H */
