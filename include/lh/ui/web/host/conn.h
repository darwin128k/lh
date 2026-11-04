/**
 * @file conn.h
 * @brief ::lh_ui_web_host_conn_t — one HTTP connection of the web host, step by step.
 *
 * A connection goes head → body (buffered, or the websocket handshake) or
 * stream (a streaming route) → done or upgraded. Each step runs when the
 * host's poll found the socket readable, receives once, and advances as far as
 * the bytes allow; it never waits. So one client that sends its request slowly
 * holds only its own connection while the host serves the others.
 *
 * The record belongs to the host (one per slot, ::LH_UI_WEB_HOST_CONN_MAX of
 * them) or to a synchronous caller; it must not move while a request is in it
 * (the streaming pipe reads through its own fields). The socket is moved in at
 * ::lh_ui_web_host_conn_try_open and stays blocking: a step receives only after
 * readiness, and replies are written whole.
 */

#ifndef LH_UI_WEB_HOST_CONN_H
#define LH_UI_WEB_HOST_CONN_H

#include <lh/ui/web/host/conn/fields.h>
#include <lh/ui/web/host/conn/phase.h>
#include <lh/ui/web/host/pipe.h>

/**
 * @brief One HTTP connection in progress.
 */
struct lh_ui_web_host_conn
{
    /** Members: ::lh_ui_web_host_conn_fields. */
    lh_ui_web_host_conn_fields();
};

/** @copybrief lh_ui_web_host_conn */
typedef struct lh_ui_web_host_conn lh_ui_web_host_conn_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The connection's socket, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_os_net_socket_t *
lh_ui_web_host_conn_get_sock(const lh_ui_web_host_conn_t *self);

/**
 * @brief The connection's socket, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_os_net_socket_t *
lh_ui_web_host_conn_get_sock_mut(lh_ui_web_host_conn_t *self);

/**
 * @brief Where the connection stands (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_ui_web_host_conn_phase_t
lh_ui_web_host_conn_get_phase(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store where the connection stands (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_phase(lh_ui_web_host_conn_t *self, lh_ui_web_host_conn_phase_t value);

/**
 * @brief When the connection was accepted (lh_os_tick_ms) (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_u64_t
lh_ui_web_host_conn_get_start(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store when the connection was accepted (lh_os_tick_ms) (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_start(lh_ui_web_host_conn_t *self, lh_u64_t value);

/**
 * @brief The request head parser, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_http_parser_t *
lh_ui_web_host_conn_get_parser(const lh_ui_web_host_conn_t *self);

/**
 * @brief The request head parser, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_http_parser_t *
lh_ui_web_host_conn_get_parser_mut(lh_ui_web_host_conn_t *self);

/**
 * @brief The request buffer: the head, then a buffered body, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_char_t *
lh_ui_web_host_conn_get_buf(const lh_ui_web_host_conn_t *self);

/**
 * @brief The request buffer: the head, then a buffered body, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_char_t *
lh_ui_web_host_conn_get_buf_mut(lh_ui_web_host_conn_t *self);

/**
 * @brief Bytes in the request buffer (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_usize_t
lh_ui_web_host_conn_get_received(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store bytes in the request buffer (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_received(lh_ui_web_host_conn_t *self, lh_usize_t value);

/**
 * @brief Bytes of the parsed head (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_usize_t
lh_ui_web_host_conn_get_head_size(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store bytes of the parsed head (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_head_size(lh_ui_web_host_conn_t *self, lh_usize_t value);

/**
 * @brief The parsed request, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_http_request_t *
lh_ui_web_host_conn_get_request(const lh_ui_web_host_conn_t *self);

/**
 * @brief The parsed request, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_http_request_t *
lh_ui_web_host_conn_get_request_mut(lh_ui_web_host_conn_t *self);

/**
 * @brief The parsed fields, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_http_header_t *
lh_ui_web_host_conn_get_headers(const lh_ui_web_host_conn_t *self);

/**
 * @brief The parsed fields, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_http_header_t *
lh_ui_web_host_conn_get_headers_mut(lh_ui_web_host_conn_t *self);

/**
 * @brief Whether the request asks for the websocket path (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_bool_t
lh_ui_web_host_conn_is_websocket(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store whether the request asks for the websocket path (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_websocket(lh_ui_web_host_conn_t *self, lh_bool_t value);

/**
 * @brief Whether a buffered body is gzip content (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_bool_t
lh_ui_web_host_conn_is_content_gzip(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store whether a buffered body is gzip content (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_content_gzip(lh_ui_web_host_conn_t *self, lh_bool_t value);

/**
 * @brief Whether a buffered body is chunked (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_bool_t
lh_ui_web_host_conn_is_chunked(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store whether a buffered body is chunked (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_chunked(lh_ui_web_host_conn_t *self, lh_bool_t value);

/**
 * @brief Whether a buffered body has gzip transfer coding (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_bool_t
lh_ui_web_host_conn_is_transfer_gzip(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store whether a buffered body has gzip transfer coding (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_transfer_gzip(lh_ui_web_host_conn_t *self, lh_bool_t value);

/**
 * @brief Content-Length of a buffered body (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_usize_t
lh_ui_web_host_conn_get_body_size(const lh_ui_web_host_conn_t *self);

/**
 * @brief Store content-Length of a buffered body (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_conn_set_body_size(lh_ui_web_host_conn_t *self, lh_usize_t value);

/**
 * @brief The streaming body's pipe, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_ui_web_host_pipe_t *
lh_ui_web_host_conn_get_pipe(const lh_ui_web_host_conn_t *self);

/**
 * @brief The streaming body's pipe, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_ui_web_host_pipe_t *
lh_ui_web_host_conn_get_pipe_mut(lh_ui_web_host_conn_t *self);

/**
 * @brief Whether a request is in progress (head, body or stream).
 * @param self Connection; must not be null.
 * @return ::lh_bool_true while the connection still needs input.
 */
lh_bool_t
lh_ui_web_host_conn_is_active(const lh_ui_web_host_conn_t *self);

/**
 * @brief Milliseconds left before the request's time limit, at @p now.
 *
 * The head and a buffered body have ::LH_UI_WEB_HOST_REQUEST_MS from the
 * accept; a stream has its route's limit (::lh_ui_web_host_pipe_get_remaining_ms).
 *
 * @param self Active connection; must not be null.
 * @param now  Milliseconds from lh_os_tick_ms.
 * @return The time left, 0 once expired.
 */
lh_u32_t
lh_ui_web_host_conn_get_remaining_ms(const lh_ui_web_host_conn_t *self, lh_u64_t now);

/**
 * @brief Take an accepted socket and start reading its request head.
 *
 * The socket is made blocking (a step receives only after readiness) and
 * no-delay. On failure the socket is left to the caller.
 *
 * @param self   Free connection; must not be null.
 * @param client Accepted socket, copied in: the connection owns it now.
 * @param now    Milliseconds from lh_os_tick_ms: the request's time runs from it.
 * @return ::lh_bool_true when the connection is reading its head.
 */
lh_bool_t
lh_ui_web_host_conn_try_open(lh_ui_web_host_conn_t *self, const lh_os_net_socket_t *client,
                             lh_u64_t now);

/**
 * @brief Close the socket and free the slot.
 * @param self Connection; must not be null.
 */
lh_void
lh_ui_web_host_conn_close(lh_ui_web_host_conn_t *self);

/**
 * @brief Receive once into the request buffer.
 * @param self Connection reading its head or body; must not be null.
 * @return Bytes received; 0 when the peer closed or the buffer is full; negative on error.
 */
lh_ssize_t
lh_ui_web_host_conn_try_receive(lh_ui_web_host_conn_t *self);

/**
 * @brief Parse what arrived of the head.
 * @param self Connection reading its head; must not be null.
 * @return ::lh_http_parse_complete (the head size is stored), incomplete or error.
 */
lh_http_parse_status_t
lh_ui_web_host_conn_try_parse_head(lh_ui_web_host_conn_t *self);

/**
 * @brief Decide what the parsed head asks for and enter that phase.
 *
 * The websocket path and buffered routes go to the body phase (after framing,
 * `Expect` and a supported coding are checked); a streaming route goes to the
 * stream phase through ::lh_ui_web_host_pipe_try_begin. A refusal goes to done.
 *
 * @param self Connection whose head is complete; must not be null.
 * @param app  The application whose routes decide.
 */
lh_void
lh_ui_web_host_conn_begin(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app);

/**
 * @brief Answer a complete buffered request: the websocket handshake, a route, or 404.
 * @param self Connection whose body is complete; must not be null.
 * @param app  The application.
 * @param body The decoded body, borrowed from the request buffer.
 */
lh_void
lh_ui_web_host_conn_answer(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app,
                           const lh_str_view_t *body);

/**
 * @brief See whether the buffered body is complete; answer it when it is.
 * @param self Connection in the body phase; must not be null.
 * @param app  The application.
 */
lh_void
lh_ui_web_host_conn_advance_body(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app);

/**
 * @brief One step of the connection; never waits.
 *
 * When @p readable, receives once (or lets the stream's gate receive once),
 * then advances the phase as far as the bytes allow. An expired request is
 * ended (a stream is aborted). Call it with @p readable false right after
 * opening, and whenever the time limit should be checked.
 *
 * @param self     Active connection; must not be null.
 * @param app      The application.
 * @param now      Milliseconds from lh_os_tick_ms.
 * @param readable Whether the poll found the socket readable.
 * @return The phase after the step: still active, upgraded (move the socket to
 *         the websocket clients) or done (close it).
 */
lh_ui_web_host_conn_phase_t
lh_ui_web_host_conn_step(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app, lh_u64_t now,
                         lh_bool_t readable);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_CONN_H */
