/**
 * @file pipe.h
 * @brief ::lh_ui_web_host_pipe_t — the body of one streaming request, step by step.
 *
 * A streaming route receives its body through a reader and stages what it
 * reads. The pipe holds everything that reader needs between two steps: the
 * wire buffer, the chunked and gzip decoders, the socket gate. Each step lets
 * the gate receive once (when the socket was readable) and calls the route's
 * handler, which reads until the reader says ::LH_IO_PENDING (wait for more),
 * 0 (the body ended and validated) or an error. A handler that stops early
 * leaves the rest to the pipe, which validates it on the following steps. When
 * the body is complete the route's commit runs; on any failure, a refused
 * handler or the route's time limit, its abort runs — exactly one of the two.
 *
 * So a slow client costs its pipe nothing but memory: the host serves others
 * meanwhile. The record belongs to the caller (the host's connection, or a
 * synchronous caller) and must not move between ::lh_ui_web_host_pipe_try_begin
 * and the step that commits or aborts.
 */

#ifndef LH_UI_WEB_HOST_PIPE_H
#define LH_UI_WEB_HOST_PIPE_H

#include <lh/core/http/body/reader.h>
#include <lh/ui/web/host/finish.h>
#include <lh/ui/web/host/gate.h>
#include <lh/ui/web/host/gzip/workspace.h>
#include <lh/ui/web/host/pipe/fields.h>
#include <lh/ui/web/host/pipe/status.h>
#include <lh/ui/web/host/tap.h>

/**
 * @brief The reader chain and state of one streaming request body.
 */
struct lh_ui_web_host_pipe
{
    /** Members: ::lh_ui_web_host_pipe_fields. */
    lh_ui_web_host_pipe_fields();
};

/** @copybrief lh_ui_web_host_pipe */
typedef struct lh_ui_web_host_pipe lh_ui_web_host_pipe_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The streaming route the request matched (level 1).
 * @param self Must not be null.
 * @return The value.
 */
const lh_ui_web_stream_route_t *
lh_ui_web_host_pipe_get_route(const lh_ui_web_host_pipe_t *self);

/**
 * @brief Store the streaming route the request matched (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_pipe_set_route(lh_ui_web_host_pipe_t *self, const lh_ui_web_stream_route_t *value);

/**
 * @brief When the request began (lh_os_tick_ms) (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_u64_t
lh_ui_web_host_pipe_get_start(const lh_ui_web_host_pipe_t *self);

/**
 * @brief Store when the request began (lh_os_tick_ms) (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_pipe_set_start(lh_ui_web_host_pipe_t *self, lh_u64_t value);

/**
 * @brief The wire buffer and the chunk-line buffer after it, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_char_t *
lh_ui_web_host_pipe_get_wire(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The wire buffer and the chunk-line buffer after it, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_char_t *
lh_ui_web_host_pipe_get_wire_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief The chunked decoder, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_http_chunked_decoder_t *
lh_ui_web_host_pipe_get_decoder(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The chunked decoder, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_http_chunked_decoder_t *
lh_ui_web_host_pipe_get_decoder_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief The framed body reader, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_http_body_reader_t *
lh_ui_web_host_pipe_get_body(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The framed body reader, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_http_body_reader_t *
lh_ui_web_host_pipe_get_body_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief The body reader as a reader, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_io_reader_t *
lh_ui_web_host_pipe_get_body_io(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The body reader as a reader, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_io_reader_t *
lh_ui_web_host_pipe_get_body_io_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief The socket gate the body reads from, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_ui_web_host_gate_t *
lh_ui_web_host_pipe_get_gate(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The socket gate the body reads from, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_ui_web_host_gate_t *
lh_ui_web_host_pipe_get_gate_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief The gate as a reader, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_io_reader_t *
lh_ui_web_host_pipe_get_gate_io(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The gate as a reader, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_io_reader_t *
lh_ui_web_host_pipe_get_gate_io_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief The gzip decoder workspace, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_ui_web_host_gzip_workspace_t *
lh_ui_web_host_pipe_get_gzip(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The gzip decoder workspace, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_ui_web_host_gzip_workspace_t *
lh_ui_web_host_pipe_get_gzip_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief The gzip reader as a reader, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_io_reader_t *
lh_ui_web_host_pipe_get_gzip_io(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The gzip reader as a reader, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_io_reader_t *
lh_ui_web_host_pipe_get_gzip_io_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief Whether the body is gzip-decoded (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_bool_t
lh_ui_web_host_pipe_is_gzip_on(const lh_ui_web_host_pipe_t *self);

/**
 * @brief Store whether the body is gzip-decoded (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_pipe_set_gzip_on(lh_ui_web_host_pipe_t *self, lh_bool_t value);

/**
 * @brief The tap the handler reads through, read-only (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
const lh_ui_web_host_tap_t *
lh_ui_web_host_pipe_get_tap(const lh_ui_web_host_pipe_t *self);

/**
 * @brief The tap the handler reads through, for change (level 1).
 * @param self Must not be null.
 * @return Its address.
 */
lh_ui_web_host_tap_t *
lh_ui_web_host_pipe_get_tap_mut(lh_ui_web_host_pipe_t *self);

/**
 * @brief Whether the handler stopped and the rest is validated (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_bool_t
lh_ui_web_host_pipe_is_draining(const lh_ui_web_host_pipe_t *self);

/**
 * @brief Store whether the handler stopped and the rest is validated (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_ui_web_host_pipe_set_draining(lh_ui_web_host_pipe_t *self, lh_bool_t value);

/**
 * @brief Whether the route's time limit has passed at @p now.
 * @param self Begun pipe; must not be null.
 * @param now  Milliseconds from lh_os_tick_ms.
 * @return ::lh_bool_true once @p now is the route's timeout past the start.
 */
lh_bool_t
lh_ui_web_host_pipe_is_expired(const lh_ui_web_host_pipe_t *self, lh_u64_t now);

/**
 * @brief Check a streaming request's codings and framing and build its reader chain.
 *
 * Content-Encoding gzip or Transfer-Encoding gzip (not both) is decoded; other
 * codings receive 415. Framing and `Expect` go through
 * ::lh_ui_web_host_try_begin_body (100 Continue, 413, 417). @p initial — body
 * bytes that came with the head — is copied into the pipe. No callback of the
 * route runs here.
 *
 * @param self    Pipe; must not be null. It must not move afterwards.
 * @param route   Streaming route the request matched, borrowed.
 * @param client  Connected socket, borrowed: replies and the body come through it.
 * @param request Parsed request, borrowed.
 * @param headers Parsed fields, count in @p request, borrowed.
 * @param initial Body bytes already received, borrowed.
 * @param start   When the request began (lh_os_tick_ms): the route's time runs from it.
 * @return ::lh_bool_true when the body can be streamed; ::lh_bool_false on a bad
 *         coding, framing or expectation, or more initial bytes than the wire
 *         holds (a reply may have been sent).
 */
lh_bool_t
lh_ui_web_host_pipe_try_begin(lh_ui_web_host_pipe_t *self, const lh_ui_web_stream_route_t *route,
                              lh_os_net_socket_t *client, const lh_http_request_t *request,
                              const lh_http_header_t *headers, lh_str_view_t initial,
                              lh_u64_t start);

/**
 * @brief Let the gate receive once: the socket was found readable.
 * @param self Begun pipe; must not be null.
 */
lh_void
lh_ui_web_host_pipe_set_readable(lh_ui_web_host_pipe_t *self);

/**
 * @brief Abort a begun pipe that will not be stepped again (the host stops).
 * @param self    Begun, unfinished pipe; must not be null.
 * @param app     Application whose context the route's abort receives.
 * @param request The request given to try_begin.
 * @param headers The fields given to try_begin.
 * @param client  The socket given to try_begin.
 */
lh_void
lh_ui_web_host_pipe_abort(lh_ui_web_host_pipe_t *self, lh_ui_web_app_t *app,
                          const lh_http_request_t *request, const lh_http_header_t *headers,
                          lh_os_net_socket_t *client);

/**
 * @brief Milliseconds left before the route's time limit, at @p now.
 * @param self Begun pipe; must not be null.
 * @param now  Milliseconds from lh_os_tick_ms.
 * @return The time left, 0 once expired.
 */
lh_u32_t
lh_ui_web_host_pipe_get_remaining_ms(const lh_ui_web_host_pipe_t *self, lh_u64_t now);

/**
 * @brief Advance the body: run the handler (or validate what it left), finish when done.
 *
 * Call it once after ::lh_ui_web_host_pipe_try_begin (for the initial bytes) and
 * again each time the socket is readable, after ::lh_ui_web_host_pipe_set_readable.
 * Never waits.
 *
 * @param self    Begun pipe; must not be null.
 * @param app     Application whose context the route's callbacks receive.
 * @param request The request given to try_begin.
 * @param headers The fields given to try_begin.
 * @param client  The socket given to try_begin.
 * @param now     Milliseconds from lh_os_tick_ms.
 * @return ::lh_ui_web_host_pipe_status_pending to step again later; committed or
 *         aborted when the route's finish callback has run.
 */
lh_ui_web_host_pipe_status_t
lh_ui_web_host_pipe_step(lh_ui_web_host_pipe_t *self, lh_ui_web_app_t *app,
                         const lh_http_request_t *request, const lh_http_header_t *headers,
                         lh_os_net_socket_t *client, lh_u64_t now);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_PIPE_H */
