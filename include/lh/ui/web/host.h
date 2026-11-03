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
/**
 * Request head and buffered-request wire capacity, in bytes.
 * Streaming routes reuse this capacity as scratch and may receive larger bodies.
 *
 * Define this before building LH to change the host's fixed stack allocation.
 * The value must be positive and suitable for the host thread's stack.
 * ::lh_ui_web_host_try_read_request instead uses its caller's buffer size.
 */
#ifndef LH_UI_WEB_HOST_REQUEST_CAPACITY
#    define LH_UI_WEB_HOST_REQUEST_CAPACITY 1024u
#endif

/** Maximum chunk size line or trailer line held by a streaming request. */
#ifndef LH_UI_WEB_HOST_CHUNK_LINE_CAPACITY
#    define LH_UI_WEB_HOST_CHUNK_LINE_CAPACITY 1024u
#endif

/** How long an accepted connection may take to send its complete request. */
#define LH_UI_WEB_HOST_REQUEST_MS 2000u

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Invoke a staging handler and commit only after the body reaches EOF.
 * The reader and request are borrowed. On staging or read failure the abort
 * callback runs; on complete success the commit callback runs. Both callbacks
 * may send a response. A late failure can follow partial staged data. This is
 * synchronous: LH_IO_PENDING while draining is treated as an aborted request.
 * @param app Application and context, borrowed.
 * @param route Streaming route with all callbacks, borrowed.
 * @param request Parsed request, borrowed.
 * @param headers Parsed fields, borrowed.
 * @param reader Decoded body reader, borrowed.
 * @param client Connected response socket, borrowed.
 * @return True if commit ran; false if staging or reading failed and abort ran.
 * @pre All pointers are valid and route callbacks are configured.
 */
lh_bool_t
lh_ui_web_host_try_dispatch_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                                   const lh_http_request_t *request,
                                   const lh_http_header_t *headers, lh_io_reader_t *reader,
                                   lh_os_net_socket_t *client);

/** @brief Checked form of ::lh_ui_web_host_try_dispatch_stream. */
lh_void
lh_ui_web_host_dispatch_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                               const lh_http_request_t *request, const lh_http_header_t *headers,
                               lh_io_reader_t *reader, lh_os_net_socket_t *client);

/**
 * @brief Decode gzip from a framed reader and dispatch a transaction.
 * Uses bounded scratch and history, validates all gzip members and limits, and
 * commits only after their trailers and source EOF. Components remain borrowed.
 * @param app Application and context.
 * @param route Route with decoded-body and gzip metadata limits.
 * @param request Borrowed parsed request.
 * @param headers Borrowed parsed fields.
 * @param compressed Borrowed framed compressed-content reader.
 * @param client Borrowed connected response socket.
 * @return True if the validated transaction committed; false if aborted.
 * @pre All pointers are valid and route callbacks are configured.
 */
lh_bool_t
lh_ui_web_host_try_dispatch_gzip(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                                 const lh_http_request_t *request, const lh_http_header_t *headers,
                                 lh_io_reader_t *compressed, lh_os_net_socket_t *client);

/** @brief Checked form of ::lh_ui_web_host_try_dispatch_gzip. */
lh_void
lh_ui_web_host_dispatch_gzip(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                             const lh_http_request_t *request, const lh_http_header_t *headers,
                             lh_io_reader_t *compressed, lh_os_net_socket_t *client);

/**
 * @brief Dispatch an already parsed request to a streaming route.
 * The staging callback borrows the request, headers and body reader. The host
 * validates the remaining body, calls commit on success or abort on failure,
 * and passes the socket to that finish callback for a reply. The route limits
 * decoded bytes, chunk metadata, and the total time since @p start (milliseconds
 * from lh_os_tick_ms). Initial bytes are borrowed and copied into scratch.
 * @return False if framing, staging, body validation, limits or the interim
 * response fail; true when commit was called. A reply may already be sent.
 * @pre All objects and borrowed views are valid; initial fits host scratch.
 */
lh_bool_t
lh_ui_web_host_try_serve_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                                lh_os_net_socket_t *client, const lh_http_request_t *request,
                                const lh_http_header_t *headers, lh_str_view_t initial,
                                lh_u64_t start);

/** @brief Checked form of ::lh_ui_web_host_try_serve_stream; aborts on failure. */
lh_void
lh_ui_web_host_serve_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                            lh_os_net_socket_t *client, const lh_http_request_t *request,
                            const lh_http_header_t *headers, lh_str_view_t initial, lh_u64_t start);

/**
 * @brief Serialize a caller-supplied response and send its head and content.
 *
 * Composes the HTTP response formatter and socket writer. The caller chooses
 * every field and whether to pass content, including the rules for HEAD and
 * statuses that forbid it. A failure can leave a partial reply on the socket.
 * The response and fields borrow their strings; all inputs and the scratch
 * buffer remain caller-owned and need only outlive this call.
 *
 * @param client Connected socket.
 * @param response Response whose header count selects @p headers.
 * @param headers Fields to serialize; null is permitted for a zero count.
 * @param scratch Writable buffer for the complete serialized response head.
 * @param capacity Size of @p scratch in bytes.
 * @param body Content to send, or null if @p body_size is zero.
 * @param body_size Number of content bytes to send.
 * @return True if formatting and all sends succeed, false otherwise.
 * @pre Response and fields satisfy ::lh_http_format_try_response_head; the
 *      buffer holds @p capacity writable bytes and @p body holds @p body_size
 *      readable bytes when nonzero.
 */
lh_bool_t
lh_ui_web_host_try_send_response(lh_os_net_socket_t *client, const lh_http_response_t *response,
                                 const lh_http_header_t *headers, lh_str_ptr scratch,
                                 lh_usize_t capacity, const lh_ptr body, lh_usize_t body_size);

/**
 * @brief Send the response with ::lh_ui_web_host_try_send_response and check success.
 *
 * Arguments, ownership, partial-send behavior and preconditions are the same
 * as the try form.
 * @fails ::lh_runtime_error_code_invalid_range Formatting or sending failed.
 */
lh_void
lh_ui_web_host_send_response(lh_os_net_socket_t *client, const lh_http_response_t *response,
                             const lh_http_header_t *headers, lh_str_ptr scratch,
                             lh_usize_t capacity, const lh_ptr body, lh_usize_t body_size);

/**
 * @brief Send a `100 Continue` interim response on a connected socket.
 *
 * Builds a status response without fields and delegates formatting and sending
 * to ::lh_ui_web_host_try_send_response. The socket remains caller-owned and
 * may have received a partial response on failure.
 * @param client Connected socket to write to.
 * @return True if sent, false on formatting or socket failure.
 * @pre @p client is valid.
 */
lh_bool_t
lh_ui_web_host_try_send_continue(lh_os_net_socket_t *client);

/**
 * @brief Send `100 Continue`, checking ::lh_ui_web_host_try_send_continue.
 * @param client Connected, caller-owned socket.
 * @fails ::lh_runtime_error_code_invalid_range Formatting or sending failed.
 */
lh_void
lh_ui_web_host_send_continue(lh_os_net_socket_t *client);

/**
 * @brief Send an HTTP response head and its permitted content.
 *
 * Builds the head from HTTP response and field objects. A HEAD request keeps
 * the representation length but receives no body; informational, 204, 205
 * and 304 responses receive neither body nor Content-Length. Successful
 * CONNECT responses also omit both; this function does not establish a tunnel.
 * Output and the request remain caller-owned. Invalid status, formatting or
 * socket failure returns false; the peer may have received a partial reply.
 *
 * @param client Connected socket to write to.
 * @param request Parsed request with a valid method view.
 * @param code HTTP status in the 100..599 range.
 * @param type NUL-terminated, valid HTTP Content-Type field value.
 * @param body Caller-owned content bytes. May be null when @p len is zero or
 *             the request method or response status forbids sending content.
 * @param len Number of representation bytes.
 * @return True when the permitted response is sent, false on invalid status,
 *         insufficient head space, formatting failure or socket failure.
 * @pre All non-optional pointers are valid, the field value contains no HTTP
 *      control characters. When content is sent, @p body has @p len readable
 *      bytes.
 */
lh_bool_t
lh_ui_web_host_try_reply(lh_os_net_socket_t *client, const lh_http_request_t *request,
                         lh_uint_t code, const lh_str_ptr type, const lh_str_ptr body,
                         lh_usize_t len);

/**
 * @brief Send an ordinary response and check ::lh_ui_web_host_try_reply.
 *
 * Arguments, ownership, and partial-send behavior are as for the try form.
 * @fails ::lh_runtime_error_code_invalid_range The response could not be sent.
 */
lh_void
lh_ui_web_host_reply(lh_os_net_socket_t *client, const lh_http_request_t *request, lh_uint_t code,
                     const lh_str_ptr type, const lh_str_ptr body, lh_usize_t len);

/**
 * @brief Try to send a plain-text HTTP response using ::lh_ui_web_host_try_reply.
 *
 * @param client Connected socket to write to.
 * @param request Parsed request with a valid method view.
 * @param code HTTP status in the 100..599 range.
 * @param text Caller-owned content; the same null rules as the body of
 *             ::lh_ui_web_host_reply apply.
 * @param len Number of representation bytes.
 * @pre The pointer, status and content preconditions of
 *      ::lh_ui_web_host_try_reply hold.
 * @return True on success, false on formatting or socket failure.
 */
lh_bool_t
lh_ui_web_host_try_reply_text(lh_os_net_socket_t *client, const lh_http_request_t *request,
                              lh_uint_t code, const lh_str_ptr text, lh_usize_t len);

/**
 * @brief Send a plain-text response and check ::lh_ui_web_host_try_reply_text.
 *
 * Arguments, ownership, and partial-send behavior are as for the try form.
 * @fails ::lh_runtime_error_code_invalid_range The response could not be sent.
 */
lh_void
lh_ui_web_host_reply_text(lh_os_net_socket_t *client, const lh_http_request_t *request,
                          lh_uint_t code, const lh_str_ptr text, lh_usize_t len);

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
 * @brief Read and parse a request head, retaining any body bytes read with it.
 *
 * Reads into caller-owned storage until the shared HTTP parser completes. The
 * request and headers borrow the buffer. On failure only their contents are
 * unspecified; @p received and @p head_size remain unchanged.
 * @param client Connected blocking socket.
 * @param buf Caller-owned writable input buffer.
 * @param capacity Bytes available in @p buf.
 * @param timeout_ms Total time allowed in milliseconds.
 * @param received Receives bytes read, including any early body bytes.
 * @param head_size Receives bytes consumed by the parsed head.
 * @param request Receives the parsed request.
 * @param headers Caller-owned parsed field storage.
 * @param header_capacity Number of fields that fit in @p headers.
 * @return True for a complete head, false on timeout, invalid input or I/O failure.
 * @pre Pointers are valid; @p buf holds @p capacity bytes. A nonzero header
 *      capacity requires that many writable entries.
 */
lh_bool_t
lh_ui_web_host_try_read_head(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                             lh_u32_t timeout_ms, lh_usize_t *received, lh_usize_t *head_size,
                             lh_http_request_t *request, lh_http_header_t *headers,
                             lh_usize_t header_capacity);

/**
 * @brief Read a request head and check ::lh_ui_web_host_try_read_head.
 * @copydetails lh_ui_web_host_try_read_head
 * @fails ::lh_runtime_error_code_invalid_range The head could not be read.
 */
lh_void
lh_ui_web_host_read_head(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                         lh_u32_t timeout_ms, lh_usize_t *received, lh_usize_t *head_size,
                         lh_http_request_t *request, lh_http_header_t *headers,
                         lh_usize_t header_capacity);

/**
 * @brief Validate request body framing and handle its HTTP expectation.
 *
 * Supports Content-Length, chunked, and gzip followed by chunked. The maximum
 * applies after chunk framing is removed but before gzip decompression. If 100-continue is
 * requested and more bytes are needed, sends the interim reply before returning; unsupported
 * expectations receive 417. HTTP/1.0 ignores Expect and rejects chunked.
 * @p initial borrows any body bytes already read. No input is owned or changed.
 * @param client Connected socket used for an interim or rejection response.
 * @param request Parsed request borrowing the head buffer.
 * @param headers Parsed fields; count is in @p request.
 * @param initial Borrowed body wire bytes already received.
 * @param decoded_limit Maximum content bytes after chunk framing and before
 *                      any gzip decompression.
 * @param can_receive_more Whether the caller has room to receive more wire bytes.
 * @param chunked Receives true for chunked, false for Content-Length or no body.
 * @param length Receives Content-Length, or zero for chunked.
 * @return True for supported framing; false on invalid framing, unsupported
 *         expectations, size limit or socket failure. Outputs remain unchanged
 *         on failure; a partial response may already have been sent.
 * @pre Pointers and borrowed views are valid; fields match the request count.
 */
lh_bool_t
lh_ui_web_host_try_begin_body(lh_os_net_socket_t *client, const lh_http_request_t *request,
                              const lh_http_header_t *headers, lh_str_view_t initial,
                              lh_usize_t decoded_limit, lh_bool_t can_receive_more,
                              lh_bool_t *chunked, lh_usize_t *length);

/**
 * @brief Begin a request body and check ::lh_ui_web_host_try_begin_body.
 * @copydetails lh_ui_web_host_try_begin_body
 * @fails ::lh_runtime_error_code_invalid_range The body could not be begun.
 */
lh_void
lh_ui_web_host_begin_body(lh_os_net_socket_t *client, const lh_http_request_t *request,
                          const lh_http_header_t *headers, lh_str_view_t initial,
                          lh_usize_t decoded_limit, lh_bool_t can_receive_more, lh_bool_t *chunked,
                          lh_usize_t *length);

/**
 * @brief Decode complete gzip bytes in place using temporary allocator storage.
 * @param buffer Caller-owned writable buffer containing encoded bytes.
 * @param encoded_size Number of encoded bytes in the buffer.
 * @param capacity Buffer capacity and maximum decoded size.
 * @param decoded_size Receives decoded byte count only on success.
 * @return True after validation, copying and release succeed; false on invalid
 *         input, output overflow or allocation/release failure. Buffer may be
 *         modified if releasing temporary storage fails; output count is then
 *         unchanged. Temporary storage is managed by the runtime allocator.
 * @pre Buffer and decoded_size are valid; buffer holds capacity writable bytes.
 */
lh_bool_t
lh_ui_web_host_try_decode_gzip_in_place(lh_str_ptr buffer, lh_usize_t encoded_size,
                                        lh_usize_t capacity, lh_usize_t *decoded_size);

/** @brief Checked form of ::lh_ui_web_host_try_decode_gzip_in_place. */
lh_void
lh_ui_web_host_decode_gzip_in_place(lh_str_ptr buffer, lh_usize_t encoded_size, lh_usize_t capacity,
                                    lh_usize_t *decoded_size);

/**
 * @brief Finish a request after ::lh_ui_web_host_try_read_head.
 *
 * The buffer starts with the parsed head followed by @p received - @p head_size
 * bytes already read. Validates framing, handles Expect and collects a complete
 * body in the remaining buffer. Chunked content is compacted in place, and a
 * gzip transfer coding is decoded using temporary runtime-allocator storage.
 * Content-Encoding is left to the caller. Request and header views borrow the
 * unchanged head; input storage remains caller-owned.
 * @param client Connected blocking socket.
 * @param buf Buffer holding the head and any initial body bytes.
 * @param capacity Writable buffer size.
 * @param timeout_ms Time allowed for further reads.
 * @param received Number of bytes currently in @p buf.
 * @param head_size Size of its already parsed head.
 * @param request Parsed request borrowing the head.
 * @param headers Borrowed parsed fields.
 * @param length Receives consumed wire bytes on success; unchanged on failure.
 * @param body Receives a decoded body view on success; unchanged on failure.
 * @return True for a complete request; false on invalid framing, timeout,
 *         limits, expectation rejection or I/O failure.
 * @pre All non-optional pointers and views are valid; head_size <= received <=
 *      capacity, and headers has the count recorded in @p request.
 */
lh_bool_t
lh_ui_web_host_try_finish_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                                  lh_u32_t timeout_ms, lh_usize_t received, lh_usize_t head_size,
                                  lh_http_request_t *request, lh_http_header_t *headers,
                                  lh_usize_t *length, lh_str_view_t *body);

/**
 * @brief Finish a request and check ::lh_ui_web_host_try_finish_request.
 * @copydetails lh_ui_web_host_try_finish_request
 * @fails ::lh_runtime_error_code_invalid_range The request could not be completed.
 */
lh_void
lh_ui_web_host_finish_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                              lh_u32_t timeout_ms, lh_usize_t received, lh_usize_t head_size,
                              lh_http_request_t *request, lh_http_header_t *headers,
                              lh_usize_t *length, lh_str_view_t *body);

/**
 * @brief Read a complete HTTP request from a socket within a time limit.
 *
 * Uses the HTTP parser on a growing caller-owned buffer. The socket and buffer
 * remain owned by the caller. Content-Length, chunked, or gzip followed by
 * chunked determines the body; repeated lengths, conflicting fields and other
 * transfer chains are rejected, as is Transfer-Encoding on HTTP/1.0. Gzip
 * transfer coding is decoded with temporary runtime-allocator storage;
 * Content-Encoding remains unchanged for the caller. The complete wire request must fit
 * in the buffer. On chunked success its body is compacted in place, trailers
 * are validated and discarded, and header views still describe the original
 * header bytes, including Transfer-Encoding. Bytes following the wire request
 * may be read but are excluded from @p length. Request and header views borrow
 * @p buf and remain valid while its head is unchanged. On failure the request
 * and header contents are unspecified; inspect them only on success. For an
 * HTTP/1.1 Expect: 100-continue request, a 100 response is sent before waiting
 * for missing body bytes. Unsupported expectations receive 417. HTTP/1.0
 * expectations are ignored; an already complete body needs no interim reply.
 *
 * @param client Connected socket, configured for blocking reads.
 * @param buf Writable storage for the request.
 * @param capacity Number of bytes available in @p buf.
 * @param timeout_ms Total time allowed, in milliseconds.
 * @param length Receives the consumed wire-byte count on success, including
 *               chunk framing when present; unchanged on failure.
 * @param body Receives a borrowed view of decoded body bytes on success;
 *             unchanged on failure.
 * @param request Receives the parsed request on success.
 * @param headers Caller-owned storage for parsed fields.
 * @param header_capacity Number of entries in @p headers; zero permits null.
 * @return ::lh_bool_true for a complete supported request; ::lh_bool_false on timeout,
 *         closed socket, I/O error, invalid head, or insufficient storage.
 * @pre @p client, @p buf, @p length, @p body and @p request are valid; @p buf has
 *      @p capacity writable bytes. A nonzero @p header_capacity requires
 *      @p headers to point to that many writable entries.
 */
lh_bool_t
lh_ui_web_host_try_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                                lh_u32_t timeout_ms, lh_usize_t *length, lh_str_view_t *body,
                                lh_http_request_t *request, lh_http_header_t *headers,
                                lh_usize_t header_capacity);

/**
 * @brief Read a complete request, checking that the read succeeded.
 * @copydetails lh_ui_web_host_try_read_request
 * @fails ::lh_runtime_error_code_invalid_range The request could not be read.
 */
lh_void
lh_ui_web_host_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                            lh_u32_t timeout_ms, lh_usize_t *length, lh_str_view_t *body,
                            lh_http_request_t *request, lh_http_header_t *headers,
                            lh_usize_t header_capacity);

/**
 * @brief Decode a complete gzip body and dispatch a buffered route.
 * The decoded body uses bounded host scratch and is validated before any
 * handler is invoked. Invalid, truncated or oversized gzip receives 400.
 * @param app Borrowed application.
 * @param request Borrowed parsed request.
 * @param headers Borrowed fields, count stored in request.
 * @param body Borrowed complete gzip bytes.
 * @param client Borrowed connected response socket.
 * @return True if a handler handled the request or an error response was
 *         attempted; false if no route handled the valid decoded body.
 * @pre All pointers are valid and body holds its stated bytes.
 */
lh_bool_t
lh_ui_web_host_handle_buffered_gzip_is_handled(lh_ui_web_app_t *app,
                                               const lh_http_request_t *request,
                                               const lh_http_header_t *headers,
                                               const lh_str_view_t *body,
                                               lh_os_net_socket_t *client);

/**
 * @brief Serve one HTTP request; true when it became a WebSocket to keep.
 *
 * The request head must fit ::LH_UI_WEB_HOST_REQUEST_CAPACITY. Gzip is
 * supported as Content-Encoding or as Transfer-Encoding before chunked;
 * using both at once on a streaming route is rejected. Other content codings
 * receive 415. Matching streaming routes receive decoded content incrementally
 * with their own body, metadata and time limits. WebSocket matching takes precedence, then
 * streaming routes, then buffered routes. Buffered routes require the whole wire request including
 * framing and trailers to fit the host buffer. A single chunked coding, gzip followed by chunked,
 * or Content-Length is supported. HTTP/1.1 100-continue is supported; unsupported expectations
 * receive 417. Known oversized content receives 413. Streaming handlers stage changes; the host
 * calls commit after validation or abort on failure. The application owns its
 * staged state and finish callbacks.
 */
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
