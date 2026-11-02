/**
 * @file host.h
 * @brief The web host: the transport that serves an application.
 *
 * The host listens, reads the requests, writes the answers and keeps the
 * websocket clients. What is served is the application's business — see
 * ::lh_ui_web_app_t. Nothing is built in: no page, no routes, no scaling. An
 * application that wants a REST surface attaches one; a desktop program has
 * no use for one, and the host does not insist.
 *
 * The Canvas2D player is one application built on this transport
 * (::lh_ui_web_player_t), not the transport's default.
 *
 * Requires ::LH_LIBRARY_OPTION_UI and ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_UI_WEB_HOST_H
#define LH_UI_WEB_HOST_H

#include <lh/config.h>
#include <lh/http/parser.h>
#include <lh/ui/web/app.h>
#include <lh/ui/web/host/hooks.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_UI"
#endif
#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_OS"
#endif

/** WebSocket clients served at once; one more is closed on arrival. */
#define LH_UI_WEB_HOST_WS_MAX 8u
/** Receive buffer of one client: the largest message plus a frame header. */
#define LH_UI_WEB_HOST_WS_RX (LH_UI_WEB_WS_MAX_MSG + 32u)
/** Bytes a client may have queued for sending before it is dropped as too slow. */
#define LH_UI_WEB_HOST_WS_QMAX (4u * 1024u * 1024u)
/** How long a quiet client goes before the host pings it. */
#define LH_UI_WEB_HOST_WS_PING_MS 15000u
/** How long a client may send nothing at all before it is dropped. */
#define LH_UI_WEB_HOST_WS_DEAD_MS 45000u
/** How long the loop sleeps when nothing at all happens: the tick's period. */
#define LH_UI_WEB_HOST_TICK_MS 20u
/** How long an accepted connection may take to send its request line. */
#define LH_UI_WEB_HOST_REQUEST_MS 2000u

LH_COMPILER_EXTERN_C_BEGIN

/** Answer an HTTP request with status @p code, content type @p type and @p body. */
lh_void
lh_ui_web_host_reply(lh_os_net_socket_t *client, lh_uint_t code, const lh_str_ptr type,
                     const lh_str_ptr body, lh_usize_t len);

/** ::lh_ui_web_host_reply with a plain-text body. */
lh_void
lh_ui_web_host_reply_text(lh_os_net_socket_t *client, lh_uint_t code, const lh_str_ptr text,
                          lh_usize_t len);

/** The first whole @p needle in @p p, or lh_null. lh_memory_find compares a
 *   shortened needle at the tail, so a match running past the end is no match. */
const lh_str_ptr
lh_ui_web_host_find(const lh_str_ptr p, lh_usize_t len, const lh_str_ptr needle,
                    lh_usize_t needle_len);

/**
 * @brief Match a GET or POST request against a complete path.
 *
 * Uses the application route matcher; a query may follow the path, but a
 * longer path with the same prefix does not match. The request and path are
 * borrowed byte spans and remain owned by the caller.
 *
 * @param request Parsed request; its bytes remain caller-owned.
 * @param path Path to match.
 * @param path_len Number of path bytes.
 * @return ::lh_bool_true on a match; ::lh_bool_false otherwise.
 */
lh_bool_t
lh_ui_web_host_is_route(const lh_http_request_t *request, const lh_str_ptr path,
                        lh_usize_t path_len);

/**
 * @brief Find a parameter in the parsed request target's query.
 *
 * The key includes its equals sign, such as `x=`. The returned value is a
 * borrowed view into the request target and is not decoded or copied. The
 * first matching key wins; empty values and missing keys both return zero.
 *
 * @param request Parsed request whose target remains alive for this call.
 * @param key Parameter name including `=`; borrowed and not modified.
 * @param key_len Number of bytes in @p key.
 * @param value Receives the start of the value when a key is found; unchanged
 *              when no key is found. The caller retains the target storage.
 * @return Value length in bytes, or zero for a missing or empty value or an
 *         empty key.
 * @pre @p request and @p value are valid; @p key names @p key_len readable
 *      bytes when the length is nonzero. The request target is a valid view.
 */
lh_usize_t
lh_ui_web_host_get_query_value_size(const lh_http_request_t *request, const lh_str_ptr key,
                                    lh_usize_t key_len, const lh_str_ptr *value);

/**
 * @brief Read a signed integer from a request's query parameter.
 *
 * Uses ::lh_ui_web_host_get_query_value_size; no input is copied or owned.
 *
 * @param request Parsed request with a valid, caller-owned target.
 * @param key Parameter name including `=`; caller-owned.
 * @param key_len Number of bytes in @p key.
 * @return The parsed integer, or zero for a missing, empty or invalid value.
 * @pre @p request and the non-empty @p key are valid.
 */
lh_sint_t
lh_ui_web_host_get_query_int(const lh_http_request_t *request, const lh_str_ptr key,
                             lh_usize_t key_len);

/**
 * @brief Read an unsigned integer from a request's query parameter.
 *
 * Uses ::lh_ui_web_host_get_query_value_size; no input is copied or owned.
 *
 * @param request Parsed request with a valid, caller-owned target.
 * @param key Parameter name including `=`; caller-owned.
 * @param key_len Number of bytes in @p key.
 * @return The parsed integer, or zero for a missing, empty or invalid value.
 * @pre @p request and the non-empty @p key are valid.
 */
lh_uint_t
lh_ui_web_host_get_query_uint(const lh_http_request_t *request, const lh_str_ptr key,
                              lh_usize_t key_len);

/**
 * @brief Read a numeric query parameter as a flag.
 *
 * Uses the signed integer reader. Input remains owned by the caller.
 *
 * @param request Parsed request with a valid, caller-owned target.
 * @param key Parameter name including `=`; caller-owned.
 * @param key_len Number of bytes in @p key.
 * @param absent Value to return when the parameter is missing or empty.
 * @return @p absent for a missing or empty value; otherwise true for a
 *         nonzero parsed integer and false for zero or invalid input.
 * @pre @p request and the non-empty @p key are valid.
 */
lh_bool_t
lh_ui_web_host_get_query_flag(const lh_http_request_t *request, const lh_str_ptr key,
                              lh_usize_t key_len, lh_bool_t absent);

/**
 * @brief Answer a WebSocket handshake using already parsed HTTP headers.
 *
 * The socket remains caller-owned; header views borrow the request buffer.
 * On success the response was sent and the socket is non-blocking.
 *
 * @param client Connected socket.
 * @param headers Parsed header array, borrowed for this call.
 * @param count Number of headers in the array.
 * @return ::lh_bool_true on upgrade, ::lh_bool_false if the key is missing,
 *         too long, hashing or sending fails, or the socket mode cannot change.
 */
lh_bool_t
lh_ui_web_host_ws_upgrade(lh_os_net_socket_t *client, const lh_http_header_t *headers,
                          lh_usize_t count);

/** Close a client and free everything it holds. */
lh_void
lh_ui_web_host_ws_drop(lh_ui_web_host_client_t *c);

/** Queue one frame for a client, at the front for control frames; drops a client over
 * ::LH_UI_WEB_HOST_WS_QMAX. */
lh_bool_t
lh_ui_web_host_ws_enqueue(lh_ui_web_host_client_t *c, lh_uint_t opcode, const void *payload,
                          lh_usize_t plen, lh_bool_t front);

/** Send what a client's socket takes now; drops it on error or once a close has drained. */
lh_void
lh_ui_web_host_ws_flush(lh_ui_web_host_client_t *c);

/** Append a fragment to the message being assembled; false past ::LH_UI_WEB_WS_MAX_MSG. */
lh_bool_t
lh_ui_web_host_ws_msg_add(lh_ui_web_host_client_t *c, const lh_byte_ptr p, lh_usize_t n);

/** Parse every whole frame received: answer pings and closes, give the text messages
 *   to the application. */
lh_void
lh_ui_web_host_ws_ingest(lh_ui_web_host_client_t *c, lh_ui_web_app_t *app, lh_uint_t now);

/** Give an upgraded connection a free client slot, or close it when there is none. */
lh_void
lh_ui_web_host_ws_attach(lh_ui_web_host_client_t *slots, lh_os_net_socket_t *client, lh_uint_t now);

/** Ping, time out, read and answer one client; one pass, never blocks. */
lh_void
lh_ui_web_host_ws_pump(lh_ui_web_host_client_t *c, lh_ui_web_app_t *app, lh_uint_t now);

/** Is @p c waiting for a frame it can take now? A client still sending the last
 *   one is skipped and gets the newest when it is done. */
lh_bool_t
lh_ui_web_host_ws_wants(const lh_ui_web_host_client_t *c, lh_uint_t gen);

/** Keep a copy of the frame just sent, to skip sending an equal one. */
lh_void
lh_ui_web_host_ws_remember(lh_ui_web_host_stream_t *stream, const lh_str_ptr body, lh_usize_t len);

/** Whether @p body equals the last frame sent. */
lh_bool_t
lh_ui_web_host_ws_same(const lh_ui_web_host_stream_t *stream, const lh_str_ptr body,
                       lh_usize_t len);

/** Draw once for every client that wants a frame and send it. Covers go once per
 *   connection: a client that missed a frame, or is new, resets the set so this
 *   one carries them all. A frame equal to the last one sent is not sent again. */
lh_void
lh_ui_web_host_ws_frame(lh_ui_web_host_client_t *cli, lh_ui_web_host_stream_t *stream,
                        lh_ui_web_app_t *app);

/**
 * @brief Read a complete HTTP request head from a socket within a time limit.
 *
 * Uses the HTTP parser on a growing caller-owned buffer. The socket and buffer
 * remain owned by the caller. Bytes received after the head are retained in
 * the buffer; this function does not guarantee that a request body is complete.
 * Parsed views borrow @p buf and remain valid while it is unchanged. The
 * caller supplies the request object and header storage. On failure, the
 * request and header contents are unspecified; inspect them only on success.
 *
 * @param client Connected socket, configured for blocking reads.
 * @param buf Writable storage for the request.
 * @param capacity Number of bytes available in @p buf.
 * @param timeout_ms Total time allowed, in milliseconds.
 * @param length Receives the number of bytes read on success; unchanged on failure.
 * @param request Receives the parsed request on success.
 * @param headers Caller-owned storage for parsed fields.
 * @param header_capacity Number of entries in @p headers; zero permits null.
 * @return ::lh_bool_true for a complete valid head; ::lh_bool_false on timeout,
 *         closed socket, I/O error, invalid head, or insufficient storage.
 * @pre @p client, @p buf, @p length and @p request are valid; @p buf has
 *      @p capacity writable bytes. A nonzero @p header_capacity requires
 *      @p headers to point to that many writable entries.
 */
lh_bool_t
lh_ui_web_host_try_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                                lh_u32_t timeout_ms, lh_usize_t *length, lh_http_request_t *request,
                                lh_http_header_t *headers, lh_usize_t header_capacity);

/**
 * @brief Read a request head, checking that the read succeeded.
 * @copydetails lh_ui_web_host_try_read_request
 * @fails ::lh_runtime_error_code_invalid_range The request could not be read.
 */
lh_void
lh_ui_web_host_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                            lh_u32_t timeout_ms, lh_usize_t *length, lh_http_request_t *request,
                            lh_http_header_t *headers, lh_usize_t header_capacity);

/** Serve one HTTP request; true when it became a WebSocket to keep. */
lh_bool_t
lh_ui_web_host_handle_request_is_upgraded(lh_ui_web_app_t *app, lh_os_net_socket_t *client);

/** Accept and serve every connection waiting on the listener. */
lh_void
lh_ui_web_host_accept_all(lh_os_net_socket_t *listen_fd, lh_ui_web_host_client_t *cli,
                          lh_ui_web_app_t *app, lh_uint_t now);

/** Sleep until a connection, a client's input or room for its queued output --
 *   or @p timeout_ms, whichever is first. This is what answers an input in the
 *   time it takes to draw it, rather than in up to one tick. A wait that fails
 *   (a client socket gone bad under it) waits on the listener alone, so the loop
 *   still sleeps instead of spinning. */
lh_void
lh_ui_web_host_wait(lh_os_net_socket_t *listen_fd, lh_ui_web_host_client_t *cli,
                    lh_uint_t timeout_ms);

/**
 * @brief Listen on 127.0.0.1:@p port and serve @p app. Does not return on success.
 *
 * @p app is the application: its routes answer the requests, its websocket path
 * takes the upgrades, its frames go to the clients and its tick runs between
 * the requests.
 *
 * @return ::lh_bool_false when the socket would not listen.
 */
lh_bool_t
lh_ui_web_host_run(lh_ui_web_app_t *app, lh_net_port_t port);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_H */
