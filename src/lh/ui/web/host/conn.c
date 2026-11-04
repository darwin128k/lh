/**
 * @file conn.c
 * @brief Implementation of `lh/ui/web/host/conn.h`.
 */

#include <lh/ui/web/host/conn.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/http/content/encoding.h>
#include <lh/core/http/framing.h>
#include <lh/core/http/status.h>
#include <lh/ui/web/host.h>

const lh_os_net_socket_t *
lh_ui_web_host_conn_get_sock(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sock);
}

lh_os_net_socket_t *
lh_ui_web_host_conn_get_sock_mut(lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sock);
}

lh_ui_web_host_conn_phase_t
lh_ui_web_host_conn_get_phase(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->phase;
}

lh_void
lh_ui_web_host_conn_set_phase(lh_ui_web_host_conn_t *self, lh_ui_web_host_conn_phase_t value)
{
    lh_assert_runtime_ref(self);
    self->phase = value;
}

lh_u64_t
lh_ui_web_host_conn_get_start(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_ui_web_host_conn_set_start(lh_ui_web_host_conn_t *self, lh_u64_t value)
{
    lh_assert_runtime_ref(self);
    self->start = value;
}

const lh_http_parser_t *
lh_ui_web_host_conn_get_parser(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->parser);
}

lh_http_parser_t *
lh_ui_web_host_conn_get_parser_mut(lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->parser);
}

const lh_char_t *
lh_ui_web_host_conn_get_buf(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buf;
}

lh_char_t *
lh_ui_web_host_conn_get_buf_mut(lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buf;
}

lh_usize_t
lh_ui_web_host_conn_get_received(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->received;
}

lh_void
lh_ui_web_host_conn_set_received(lh_ui_web_host_conn_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->received = value;
}

lh_usize_t
lh_ui_web_host_conn_get_head_size(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->head_size;
}

lh_void
lh_ui_web_host_conn_set_head_size(lh_ui_web_host_conn_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->head_size = value;
}

const lh_http_request_t *
lh_ui_web_host_conn_get_request(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->request);
}

lh_http_request_t *
lh_ui_web_host_conn_get_request_mut(lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->request);
}

const lh_http_header_t *
lh_ui_web_host_conn_get_headers(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->headers;
}

lh_http_header_t *
lh_ui_web_host_conn_get_headers_mut(lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->headers;
}

lh_bool_t
lh_ui_web_host_conn_is_websocket(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->websocket;
}

lh_void
lh_ui_web_host_conn_set_websocket(lh_ui_web_host_conn_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->websocket = value;
}

lh_bool_t
lh_ui_web_host_conn_is_content_gzip(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->content_gzip;
}

lh_void
lh_ui_web_host_conn_set_content_gzip(lh_ui_web_host_conn_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->content_gzip = value;
}

lh_bool_t
lh_ui_web_host_conn_is_chunked(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->chunked;
}

lh_void
lh_ui_web_host_conn_set_chunked(lh_ui_web_host_conn_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->chunked = value;
}

lh_bool_t
lh_ui_web_host_conn_is_transfer_gzip(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->transfer_gzip;
}

lh_void
lh_ui_web_host_conn_set_transfer_gzip(lh_ui_web_host_conn_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->transfer_gzip = value;
}

lh_usize_t
lh_ui_web_host_conn_get_body_size(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return self->body_size;
}

lh_void
lh_ui_web_host_conn_set_body_size(lh_ui_web_host_conn_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->body_size = value;
}

const lh_ui_web_host_pipe_t *
lh_ui_web_host_conn_get_pipe(const lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pipe);
}

lh_ui_web_host_pipe_t *
lh_ui_web_host_conn_get_pipe_mut(lh_ui_web_host_conn_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pipe);
}

lh_bool_t
lh_ui_web_host_conn_is_active(const lh_ui_web_host_conn_t *self)
{
    const lh_ui_web_host_conn_phase_t phase = lh_ui_web_host_conn_get_phase(self);
    return lh_math_eq(phase, lh_ui_web_host_conn_phase_head) ||
           lh_math_eq(phase, lh_ui_web_host_conn_phase_body) ||
           lh_math_eq(phase, lh_ui_web_host_conn_phase_stream);
}

lh_u32_t
lh_ui_web_host_conn_get_remaining_ms(const lh_ui_web_host_conn_t *self, lh_u64_t now)
{
    if (lh_math_eq(lh_ui_web_host_conn_get_phase(self), lh_ui_web_host_conn_phase_stream))
    {
        return lh_ui_web_host_pipe_get_remaining_ms(lh_ui_web_host_conn_get_pipe(self), now);
    }
    const lh_u64_t elapsed = lh_math_sub(now, lh_ui_web_host_conn_get_start(self));
    if (lh_math_ge(elapsed, LH_UI_WEB_HOST_REQUEST_MS))
    {
        return 0U;
    }
    return lh_cast_static(lh_u32_t, lh_math_sub(LH_UI_WEB_HOST_REQUEST_MS, elapsed));
}

lh_bool_t
lh_ui_web_host_conn_try_open(lh_ui_web_host_conn_t *self, const lh_os_net_socket_t *client,
                             lh_u64_t now)
{
    lh_os_net_socket_t *sock = lh_ui_web_host_conn_get_sock_mut(self);
    lh_ptr_deref(sock) = lh_ptr_deref(client);
    if (!lh_os_net_socket_set_blocking(sock, lh_bool_true))
    {
        return lh_bool_false;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_net_socket_set_no_delay(sock, lh_bool_true));
    lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_head);
    lh_ui_web_host_conn_set_start(self, now);
    lh_http_parser_init(lh_ui_web_host_conn_get_parser_mut(self));
    lh_ui_web_host_conn_set_received(self, 0U);
    lh_ui_web_host_conn_set_head_size(self, 0U);
    lh_ui_web_host_conn_set_websocket(self, lh_bool_false);
    lh_ui_web_host_conn_set_content_gzip(self, lh_bool_false);
    lh_ui_web_host_conn_set_chunked(self, lh_bool_false);
    lh_ui_web_host_conn_set_transfer_gzip(self, lh_bool_false);
    lh_ui_web_host_conn_set_body_size(self, 0U);
    return lh_bool_true;
}

lh_void
lh_ui_web_host_conn_close(lh_ui_web_host_conn_t *self)
{
    lh_os_net_socket_close(lh_ui_web_host_conn_get_sock_mut(self));
    lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_free);
}

lh_ssize_t
lh_ui_web_host_conn_try_receive(lh_ui_web_host_conn_t *self)
{
    const lh_usize_t received = lh_ui_web_host_conn_get_received(self);
    if (lh_math_ge(received, LH_UI_WEB_HOST_REQUEST_CAPACITY))
    {
        return 0;
    }
    const lh_ssize_t got = lh_os_net_socket_recv(
        lh_ui_web_host_conn_get_sock_mut(self), lh_ui_web_host_conn_get_buf_mut(self) + received,
        lh_math_sub(LH_UI_WEB_HOST_REQUEST_CAPACITY, received));
    if (lh_math_gt(got, 0))
    {
        lh_ui_web_host_conn_set_received(self,
                                         lh_math_add(received, lh_cast_static(lh_usize_t, got)));
    }
    return got;
}

lh_http_parse_status_t
lh_ui_web_host_conn_try_parse_head(lh_ui_web_host_conn_t *self)
{
    lh_str_view_t input;
    lh_str_view_init_by_size(lh_addr_of(input), lh_ui_web_host_conn_get_buf_mut(self),
                             lh_ui_web_host_conn_get_received(self));
    const lh_http_parse_status_t status = lh_http_parser_request(
        lh_ui_web_host_conn_get_parser_mut(self), input, lh_ui_web_host_conn_get_request_mut(self),
        lh_ui_web_host_conn_get_headers_mut(self), LH_UI_WEB_HOST_HEADER_MAX);
    if (lh_math_eq(status, lh_http_parse_complete))
    {
        lh_ui_web_host_conn_set_head_size(
            self, lh_http_parser_get_consumed(lh_ui_web_host_conn_get_parser(self)));
    }
    return status;
}

lh_void
lh_ui_web_host_conn_begin(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app)
{
    lh_os_net_socket_t *sock = lh_ui_web_host_conn_get_sock_mut(self);
    const lh_http_request_t *request = lh_ui_web_host_conn_get_request(self);
    const lh_http_header_t *headers = lh_ui_web_host_conn_get_headers(self);
    const lh_usize_t count = lh_http_request_get_header_count(request);
    const lh_usize_t head_size = lh_ui_web_host_conn_get_head_size(self);
    const lh_usize_t received = lh_ui_web_host_conn_get_received(self);
    lh_str_view_t initial;
    lh_str_view_init_by_size(lh_addr_of(initial), lh_ui_web_host_conn_get_buf_mut(self) + head_size,
                             lh_math_sub(received, head_size));
    const lh_bool_t websocket = lh_ui_web_app_is_websocket(app, request);
    lh_ui_web_host_conn_set_websocket(self, websocket);
    lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_done);
    if (!websocket)
    {
        const lh_ui_web_stream_route_t *route = lh_ui_web_app_find_stream_route(app, request);
        if (lh_null_ne(route))
        {
            if (lh_ui_web_host_pipe_try_begin(lh_ui_web_host_conn_get_pipe_mut(self), route, sock,
                                              request, headers, initial,
                                              lh_ui_web_host_conn_get_start(self)))
            {
                lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_stream);
            }
            return;
        }
        lh_bool_t content_gzip = lh_bool_false;
        if (!lh_http_content_encoding_try_is_gzip(headers, count, lh_addr_of(content_gzip)))
        {
            LH_ATTRIBUTE_UNUSED(
                lh_ui_web_host_try_reply_text(sock, request, lh_http_status_unsupported_media_type,
                                              lh_str_lit("unsupported content encoding\n")));
            return;
        }
        lh_ui_web_host_conn_set_content_gzip(self, content_gzip);
    }
    lh_bool_t transfer_chunked = lh_bool_false;
    lh_bool_t transfer_gzip = lh_bool_false;
    if (!lh_http_framing_try_get_transfer(headers, count, lh_addr_of(transfer_chunked),
                                          lh_addr_of(transfer_gzip)))
    {
        return;
    }
    lh_bool_t chunked = lh_bool_false;
    lh_usize_t body_size = 0U;
    if (!lh_ui_web_host_try_begin_body(sock, request, headers, initial,
                                       lh_math_sub(LH_UI_WEB_HOST_REQUEST_CAPACITY, head_size),
                                       lh_math_lt(received, LH_UI_WEB_HOST_REQUEST_CAPACITY),
                                       lh_addr_of(chunked), lh_addr_of(body_size)))
    {
        return;
    }
    lh_ui_web_host_conn_set_transfer_gzip(self, transfer_gzip);
    lh_ui_web_host_conn_set_chunked(self, chunked);
    lh_ui_web_host_conn_set_body_size(self, body_size);
    lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_body);
}

lh_void
lh_ui_web_host_conn_answer(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app,
                           const lh_str_view_t *body)
{
    lh_os_net_socket_t *sock = lh_ui_web_host_conn_get_sock_mut(self);
    const lh_http_request_t *request = lh_ui_web_host_conn_get_request(self);
    const lh_http_header_t *headers = lh_ui_web_host_conn_get_headers(self);
    lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_done);
    if (lh_ui_web_host_conn_is_websocket(self))
    {
        if (lh_ui_web_host_ws_upgrade(sock, headers, lh_http_request_get_header_count(request)))
        {
            lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_upgraded);
        }
        return;
    }
    if (lh_ui_web_host_conn_is_content_gzip(self)
            ? lh_ui_web_host_handle_buffered_gzip_is_handled(app, request, headers, body, sock)
            : lh_ui_web_app_handle_request_is_handled(app, request, headers, body, sock))
    {
        return;
    }
    LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(sock, request, lh_http_status_not_found,
                                                      lh_str_lit("not found\n")));
}

lh_void
lh_ui_web_host_conn_advance_body(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app)
{
    lh_usize_t length = 0U;
    lh_str_view_t body;
    const lh_http_parse_status_t status = lh_ui_web_host_try_complete_body(
        lh_ui_web_host_conn_get_buf_mut(self), LH_UI_WEB_HOST_REQUEST_CAPACITY,
        lh_ui_web_host_conn_get_received(self), lh_ui_web_host_conn_get_head_size(self),
        lh_ui_web_host_conn_is_chunked(self), lh_ui_web_host_conn_get_body_size(self),
        lh_ui_web_host_conn_is_transfer_gzip(self), lh_addr_of(length), lh_addr_of(body));
    if (lh_math_eq(status, lh_http_parse_complete))
    {
        lh_ui_web_host_conn_answer(self, app, lh_addr_of(body));
        return;
    }
    if (lh_math_eq(status, lh_http_parse_error) ||
        lh_math_ge(lh_ui_web_host_conn_get_received(self), LH_UI_WEB_HOST_REQUEST_CAPACITY))
    {
        lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_done);
    }
}

lh_ui_web_host_conn_phase_t
lh_ui_web_host_conn_step(lh_ui_web_host_conn_t *self, lh_ui_web_app_t *app, lh_u64_t now,
                         lh_bool_t readable)
{
    lh_ui_web_host_conn_phase_t phase = lh_ui_web_host_conn_get_phase(self);
    if (lh_math_eq(phase, lh_ui_web_host_conn_phase_stream))
    {
        /* The pipe keeps its own time and aborts the route when it runs out. */
        lh_ui_web_host_pipe_t *pipe = lh_ui_web_host_conn_get_pipe_mut(self);
        if (readable)
        {
            lh_ui_web_host_pipe_set_readable(pipe);
        }
        if (lh_math_ne(lh_ui_web_host_pipe_step(pipe, app, lh_ui_web_host_conn_get_request(self),
                                                lh_ui_web_host_conn_get_headers(self),
                                                lh_ui_web_host_conn_get_sock_mut(self), now),
                       lh_ui_web_host_pipe_status_pending))
        {
            lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_done);
        }
        return lh_ui_web_host_conn_get_phase(self);
    }
    if (lh_math_is_zero(lh_ui_web_host_conn_get_remaining_ms(self, now)) ||
        (readable && lh_math_le(lh_ui_web_host_conn_try_receive(self), 0)))
    {
        lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_done);
        return lh_ui_web_host_conn_phase_done;
    }
    if (lh_math_eq(phase, lh_ui_web_host_conn_phase_head))
    {
        const lh_http_parse_status_t status = lh_ui_web_host_conn_try_parse_head(self);
        if (lh_math_eq(status, lh_http_parse_error) ||
            (lh_math_eq(status, lh_http_parse_incomplete) &&
             lh_math_ge(lh_ui_web_host_conn_get_received(self), LH_UI_WEB_HOST_REQUEST_CAPACITY)))
        {
            lh_ui_web_host_conn_set_phase(self, lh_ui_web_host_conn_phase_done);
            return lh_ui_web_host_conn_phase_done;
        }
        if (lh_math_eq(status, lh_http_parse_incomplete))
        {
            return lh_ui_web_host_conn_phase_head;
        }
        lh_ui_web_host_conn_begin(self, app);
        phase = lh_ui_web_host_conn_get_phase(self);
        if (lh_math_eq(phase, lh_ui_web_host_conn_phase_stream))
        {
            /* The bytes that came with the head go to the handler at once. */
            return lh_ui_web_host_conn_step(self, app, now, lh_bool_false);
        }
    }
    if (lh_math_eq(phase, lh_ui_web_host_conn_phase_body))
    {
        lh_ui_web_host_conn_advance_body(self, app);
    }
    return lh_ui_web_host_conn_get_phase(self);
}
