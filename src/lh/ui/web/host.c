/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>
#include <lh/ui/web/host/deadline.h>
#include <lh/core/http/content/encoding.h>
#include <lh/core/compress/gzip.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/http/format.h>
#include <lh/core/http/framing.h>
#include <lh/core/http/expect.h>
#include <lh/core/http/header/name.h>
#include <lh/core/http/header/value.h>
#include <lh/core/http/status.h>
#include <lh/core/str/format/ullong.h>
#include <lh/core/str/split/once.h>
#include <lh/core/char/map.h>

#include <lh/core/cast/const.h>
#include <lh/core/crypto/sha.h>
#include <lh/core/memory.h>
#include <lh/os/alloc.h>
#include <lh/os/net.h>
#include <lh/os/tick.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/runtime/logger.h>
#include <lh/core/str/parse/text.h>
#include <lh/core/util/memory.h>
#include <lh/core/str/format/base64.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_ui_web_host_try_send_response(lh_os_net_socket_t *client, const lh_http_response_t *response,
                                 const lh_http_header_t *headers, lh_str_ptr scratch,
                                 lh_usize_t capacity, const lh_ptr body, lh_usize_t body_size)
{
    lh_usize_t size = 0U;

    if (!lh_http_format_try_response_head(response, headers, scratch, capacity, lh_addr_of(size)) ||
        !lh_os_net_socket_send_all(client, scratch, size))
    {
        return lh_bool_false;
    }
    return lh_math_is_zero(body_size) || lh_os_net_socket_send_all(client, body, body_size);
}

lh_str_cptr
lh_ui_web_host_find(lh_str_cptr p, lh_usize_t len, lh_str_cptr needle, lh_usize_t needle_len)
{
    lh_str_cptr at =
        lh_ptr_rcast(const lh_char_t, lh_memory_find(lh_cast_const(lh_ptr, p), len,
                                                     lh_cast_const(lh_ptr, needle), needle_len));

    return (at != lh_null && lh_cast_static(lh_usize_t, (at - p)) + needle_len <= len) ? at
                                                                                       : lh_null;
}

lh_bool_t
lh_ui_web_host_is_route(const lh_http_request_t *request, lh_str_cptr path, lh_usize_t path_len)
{
    if (!lh_http_request_is_method(request, lh_http_method_get) &&
        !lh_http_request_is_method(request, lh_http_method_post))
    {
        return lh_bool_false;
    }
    lh_ui_web_route_t route;
    lh_ui_web_route_set_path(lh_addr_of(route), path);
    lh_ui_web_route_set_path_len(lh_addr_of(route), path_len);
    lh_ui_web_route_set_method(lh_addr_of(route), lh_null);
    lh_ui_web_route_set_method_len(lh_addr_of(route), 0U);
    return lh_ui_web_route_is_match(lh_addr_of(route), request);
}

lh_usize_t
lh_ui_web_host_get_query_value_size(const lh_http_request_t *request, lh_str_cptr key,
                                    lh_usize_t key_len, lh_str_cptr *value)
{
    return lh_http_request_get_query_value_size(request, key, key_len, value);
}

lh_void
lh_ui_web_host_ws_drop(lh_ui_web_host_client_t *c)
{
    lh_ui_web_host_frame_t *q;

    if (c == lh_null || !lh_ui_web_host_client_is_used(c))
    {
        return;
    }
    lh_os_net_socket_close(lh_ui_web_host_client_get_sock_mut(c));
    lh_os_net_socket_init(lh_ui_web_host_client_get_sock_mut(c));
    q = lh_ui_web_host_client_get_qhead(c);
    while (q != lh_null)
    {
        lh_ui_web_host_frame_t *n = lh_ui_web_host_frame_get_next(q);

        lh_runtime_allocator_free(lh_ui_web_host_frame_get_bytes(q));
        lh_runtime_allocator_free(q);
        q = n;
    }
    lh_runtime_allocator_free(lh_ui_web_host_client_get_rx(c));
    lh_runtime_allocator_free(lh_ui_web_host_client_get_msg(c));
    lh_ui_web_host_client_set_rx(c, lh_null);
    lh_ui_web_host_client_set_msg(c, lh_null);
    lh_ui_web_host_client_set_rx_len(c, 0u);
    lh_ui_web_host_client_set_msg_len(c, 0u);
    lh_ui_web_host_client_set_msg_cap(c, 0u);
    lh_ui_web_host_client_set_msg_on(c, lh_bool_false);
    lh_ui_web_host_client_set_qhead(c, lh_null);
    lh_ui_web_host_client_set_qtail(c, lh_null);
    lh_ui_web_host_client_set_queued(c, 0u);
    lh_ui_web_host_client_set_used(c, lh_bool_false);
    lh_ui_web_host_client_set_closing(c, lh_bool_false);
}

lh_bool_t
lh_ui_web_host_ws_msg_add(lh_ui_web_host_client_t *c, lh_byte_cptr p, lh_usize_t n)
{
    lh_byte_ptr grow;

    if (lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n) > LH_UI_WEB_WS_MAX_MSG)
    {
        return lh_bool_false;
    }
    if (lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n) >
        lh_ui_web_host_client_get_msg_cap(c))
    {
        const lh_uint_t cap = lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n);
        const lh_uint_t next = (lh_ui_web_host_client_get_msg_cap(c) == 0u)
                                   ? 256u
                                   : (lh_ui_web_host_client_get_msg_cap(c) * 2u);
        const lh_uint_t want = (cap > next) ? cap : next;

        grow = lh_ptr_rcast(
            lh_byte_t, lh_runtime_allocator_realloc(lh_ui_web_host_client_get_msg(c),
                                                    lh_ui_web_host_client_get_msg_cap(c), want));
        if (grow == lh_null)
        {
            return lh_bool_false;
        }
        lh_ui_web_host_client_set_msg(c, grow);
        lh_ui_web_host_client_set_msg_cap(c, want);
    }
    lh_memory_copy(lh_ui_web_host_client_get_msg(c) + lh_ui_web_host_client_get_msg_len(c),
                   lh_ui_web_host_client_get_msg_cap(c) - lh_ui_web_host_client_get_msg_len(c),
                   lh_cast_const(lh_ptr, p), n);
    lh_ui_web_host_client_set_msg_len(c, lh_ui_web_host_client_get_msg_len(c) +
                                             (lh_cast_static(lh_uint_t, n)));
    return lh_bool_true;
}

lh_void
lh_ui_web_host_ws_attach(lh_ui_web_host_client_t *slots, lh_os_net_socket_t *client, lh_uint_t now)
{
    for (lh_uint_t i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
    {
        lh_ui_web_host_client_t *c = slots + i;

        if (lh_ui_web_host_client_is_used(c))
        {
            continue;
        }
        lh_ui_web_host_client_set_rx(c, lh_ptr_rcast(lh_byte_t, lh_os_alloc(LH_UI_WEB_HOST_WS_RX)));
        if (lh_ui_web_host_client_get_rx(c) == lh_null)
        {
            lh_os_net_socket_close(client);
            return;
        }
        lh_ui_web_host_client_set_sock(c, lh_ptr_deref(client));
        lh_ui_web_host_client_set_rx_len(c, 0u);
        lh_ui_web_host_client_set_msg(c, lh_null);
        lh_ui_web_host_client_set_msg_len(c, 0u);
        lh_ui_web_host_client_set_msg_cap(c, 0u);
        lh_ui_web_host_client_set_msg_on(c, lh_bool_false);
        lh_ui_web_host_client_set_qhead(c, lh_null);
        lh_ui_web_host_client_set_qtail(c, lh_null);
        lh_ui_web_host_client_set_queued(c, 0u);
        lh_ui_web_host_client_set_seen(c, 0u);
        lh_ui_web_host_client_set_last_rx_ms(c, now);
        lh_ui_web_host_client_set_last_ping_ms(c, now);
        lh_ui_web_host_client_set_used(c, lh_bool_true);
        lh_ui_web_host_client_set_closing(c, lh_bool_false);
        lh_ui_web_host_client_set_synced(c, lh_bool_false);
        return;
    }
    lh_os_net_socket_close(client);
}

lh_bool_t
lh_ui_web_host_try_read_head(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                             lh_u32_t timeout_ms, lh_usize_t *received, lh_usize_t *head_size,
                             lh_http_request_t *request, lh_http_header_t *headers,
                             lh_usize_t header_capacity)
{
    lh_http_parser_t parser;
    lh_http_parser_init(lh_addr_of(parser));
    lh_os_net_socket_t *const watch[] = {client};
    const lh_u64_t start = lh_os_tick_ms();
    lh_usize_t used = 0U;
    while (lh_math_lt(used, capacity))
    {
        const lh_u64_t elapsed = lh_math_sub(lh_os_tick_ms(), start);
        if (lh_math_ge(elapsed, timeout_ms) ||
            lh_math_le(
                lh_os_net_socket_wait(watch, 1U, lh_null, 0U,
                                      lh_cast_static(lh_u32_t, lh_math_sub(timeout_ms, elapsed))),
                0))
        {
            return lh_bool_false;
        }
        const lh_ssize_t got =
            lh_os_net_socket_recv(client, buf + used, lh_math_sub(capacity, used));
        if (lh_math_le(got, 0))
        {
            return lh_bool_false;
        }
        used = lh_math_add(used, lh_cast_static(lh_usize_t, got));
        const lh_str_view_t input = lh_str_view_initializer(buf, used);
        const lh_http_parse_status_t status =
            lh_http_parser_request(lh_addr_of(parser), input, request, headers, header_capacity);
        if (lh_math_eq(status, lh_http_parse_error))
        {
            return lh_bool_false;
        }
        if (lh_math_eq(status, lh_http_parse_complete))
        {
            lh_ptr_deref(received) = used;
            lh_ptr_deref(head_size) = lh_http_parser_get_consumed(lh_addr_of(parser));
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_bool_t
lh_ui_web_host_try_decode_gzip_in_place(lh_str_ptr buffer, lh_usize_t encoded_size,
                                        lh_usize_t capacity, lh_usize_t *decoded_size)
{
    if (lh_math_is_zero(capacity) || lh_math_gt(encoded_size, capacity))
    {
        return lh_bool_false;
    }
    lh_byte_t *scratch = lh_ptr_rcast(lh_byte_t, lh_runtime_allocator_try_alloc(capacity));
    if (lh_null_eq(scratch))
    {
        return lh_bool_false;
    }
    lh_usize_t result;
    const lh_bool_t ok = lh_compress_gzip_try_decode(lh_ptr_rcast(const lh_byte_t, buffer),
                                                     encoded_size, scratch, capacity, &result);
    if (ok && !lh_math_is_zero(result))
    {
        lh_memory_std_copy(buffer, scratch, result);
    }
    const lh_bool_t freed = lh_runtime_allocator_try_free(scratch);
    if (!ok || !freed)
    {
        return lh_bool_false;
    }
    *decoded_size = result;
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_host_try_dispatch_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                                   const lh_http_request_t *request,
                                   const lh_http_header_t *headers, lh_io_reader_t *reader,
                                   lh_os_net_socket_t *client)
{
    lh_bool_t ok = lh_ui_web_stream_route_get_handler(route)(lh_ui_web_app_get_ctx(app), request,
                                                             headers, reader);
    lh_char_t discard[LH_UI_WEB_HOST_REQUEST_CAPACITY];
    if (ok)
    {
        ok = lh_math_is_zero(lh_io_reader_drain(reader, discard, sizeof(discard)));
    }
    return lh_ui_web_host_finish_stream(app, route, request, headers, client, ok);
}

lh_bool_t
lh_ui_web_host_try_serve_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                                lh_os_net_socket_t *client, const lh_http_request_t *request,
                                const lh_http_header_t *headers, lh_str_view_t initial,
                                lh_u64_t start)
{
    lh_ui_web_host_pipe_t *pipe = lh_ptr_rcast(
        lh_ui_web_host_pipe_t, lh_runtime_allocator_try_alloc(sizeof(lh_ui_web_host_pipe_t)));
    if (lh_null_eq(pipe))
    {
        return lh_bool_false;
    }
    lh_bool_t committed = lh_bool_false;
    if (lh_ui_web_host_pipe_try_begin(pipe, route, client, request, headers, initial, start))
    {
        lh_os_net_socket_t *const watch[] = {client};
        lh_ui_web_host_pipe_status_t status =
            lh_ui_web_host_pipe_step(pipe, app, request, headers, client, lh_os_tick_ms());
        while (lh_math_eq(status, lh_ui_web_host_pipe_status_pending))
        {
            const lh_ssize_t ready =
                lh_os_net_socket_wait(watch, 1U, lh_null, 0U,
                                      lh_ui_web_host_pipe_get_remaining_ms(pipe, lh_os_tick_ms()));
            if (lh_math_eq(ready, LH_OS_RESULT_INVALID))
            {
                lh_ui_web_host_pipe_abort(pipe, app, request, headers, client);
                status = lh_ui_web_host_pipe_status_aborted;
                break;
            }
            if (lh_math_gt(ready, 0))
            {
                lh_ui_web_host_pipe_set_readable(pipe);
            }
            /* Out of time, the step aborts the route. */
            status = lh_ui_web_host_pipe_step(pipe, app, request, headers, client, lh_os_tick_ms());
        }
        committed = lh_math_eq(status, lh_ui_web_host_pipe_status_committed);
    }
    LH_ATTRIBUTE_UNUSED(lh_runtime_allocator_try_free(pipe));
    return committed;
}

lh_bool_t
lh_ui_web_host_handle_request_is_upgraded(lh_ui_web_app_t *app, lh_os_net_socket_t *client)
{
    lh_ui_web_host_conn_t *conn = lh_ptr_rcast(
        lh_ui_web_host_conn_t, lh_runtime_allocator_try_alloc(sizeof(lh_ui_web_host_conn_t)));
    if (lh_null_eq(conn))
    {
        return lh_bool_false;
    }
    lh_bool_t upgraded = lh_bool_false;
    if (lh_ui_web_host_conn_try_open(conn, client, lh_os_tick_ms64()))
    {
        lh_os_net_socket_t *const watch[] = {lh_ui_web_host_conn_get_sock_mut(conn)};
        lh_ui_web_host_conn_phase_t phase =
            lh_ui_web_host_conn_step(conn, app, lh_os_tick_ms64(), lh_bool_false);
        while (lh_ui_web_host_conn_is_active(conn))
        {
            const lh_ssize_t ready = lh_os_net_socket_wait(
                watch, 1U, lh_null, 0U,
                lh_ui_web_host_conn_get_remaining_ms(conn, lh_os_tick_ms64()));
            if (lh_math_eq(ready, LH_OS_RESULT_INVALID))
            {
                if (lh_math_eq(phase, lh_ui_web_host_conn_phase_stream))
                {
                    lh_ui_web_host_pipe_abort(lh_ui_web_host_conn_get_pipe_mut(conn), app,
                                              lh_ui_web_host_conn_get_request(conn),
                                              lh_ui_web_host_conn_get_headers(conn),
                                              lh_ui_web_host_conn_get_sock_mut(conn));
                }
                break;
            }
            /* Out of time, the step ends the request. */
            phase = lh_ui_web_host_conn_step(conn, app, lh_os_tick_ms64(), lh_math_gt(ready, 0));
        }
        upgraded = lh_math_eq(phase, lh_ui_web_host_conn_phase_upgraded);
        if (upgraded)
        {
            /* The handshake changed the socket's mode: the caller keeps this one. */
            lh_ptr_deref(client) = lh_ptr_deref(lh_ui_web_host_conn_get_sock(conn));
        }
    }
    LH_ATTRIBUTE_UNUSED(lh_runtime_allocator_try_free(conn));
    return upgraded;
}

lh_bool_t
lh_ui_web_host_ws_wants(const lh_ui_web_host_client_t *c, lh_uint_t gen)
{
    return (lh_ui_web_host_client_is_used(c) && !lh_ui_web_host_client_is_closing(c) &&
            lh_ui_web_host_client_get_seen(c) != gen &&
            lh_ui_web_host_client_get_qhead(c) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_web_host_ws_remember(lh_ui_web_host_stream_t *stream, lh_str_cptr body, lh_usize_t len)
{
    if (len > lh_ui_web_host_stream_get_last_cap(stream))
    {
        lh_str_ptr grow = lh_ptr_rcast(
            lh_char_t,
            lh_runtime_allocator_realloc(lh_ui_web_host_stream_get_last(stream),
                                         lh_ui_web_host_stream_get_last_cap(stream), len));

        if (grow == lh_null)
        {
            lh_ui_web_host_stream_set_last_len(stream, 0u);
            return;
        }
        lh_ui_web_host_stream_set_last(stream, grow);
        lh_ui_web_host_stream_set_last_cap(stream, len);
    }
    lh_memory_copy(lh_ui_web_host_stream_get_last(stream),
                   lh_ui_web_host_stream_get_last_cap(stream), lh_cast_const(lh_ptr, body), len);
    lh_ui_web_host_stream_set_last_len(stream, len);
}

lh_bool_t
lh_ui_web_host_ws_same(const lh_ui_web_host_stream_t *stream, lh_str_cptr body, lh_usize_t len)
{
    return (len > 0u && len == lh_ui_web_host_stream_get_last_len(stream) &&
            lh_memory_compare(lh_cast_const(lh_ptr, body), len,
                              lh_ui_web_host_stream_get_last(stream), len) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

lh_ui_web_app_t *
lh_ui_web_host_get_app(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->app;
}

lh_void
lh_ui_web_host_set_app(lh_ui_web_host_t *self, lh_ui_web_app_t *value)
{
    lh_assert_runtime_ref(self);
    self->app = value;
}

const lh_os_net_socket_t *
lh_ui_web_host_get_listener(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->listener);
}

lh_os_net_socket_t *
lh_ui_web_host_get_listener_mut(lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->listener);
}

const lh_os_wake_t *
lh_ui_web_host_get_wake(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->wake);
}

lh_os_wake_t *
lh_ui_web_host_get_wake_mut(lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->wake);
}

lh_exec_loop_t *
lh_ui_web_host_get_loop(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->loop;
}

lh_void
lh_ui_web_host_set_loop(lh_ui_web_host_t *self, lh_exec_loop_t *value)
{
    lh_assert_runtime_ref(self);
    self->loop = value;
}

const lh_ui_web_host_client_t *
lh_ui_web_host_get_clients(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clients;
}

lh_ui_web_host_client_t *
lh_ui_web_host_get_clients_mut(lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clients;
}

const lh_ui_web_host_stream_t *
lh_ui_web_host_get_stream(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->stream);
}

lh_ui_web_host_stream_t *
lh_ui_web_host_get_stream_mut(lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->stream);
}

const lh_ui_web_host_conn_t *
lh_ui_web_host_get_conns(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->conns;
}

lh_ui_web_host_conn_t *
lh_ui_web_host_get_conns_mut(lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->conns;
}

const lh_os_net_socket_poll_t *
lh_ui_web_host_get_events(const lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->events;
}

lh_os_net_socket_poll_t *
lh_ui_web_host_get_events_mut(lh_ui_web_host_t *self)
{
    lh_assert_runtime_ref(self);
    return self->events;
}

lh_bool_t
lh_ui_web_host_try_listen(lh_os_net_socket_t *listener, lh_net_port_t port)
{
    if (!lh_os_net_socket_open(listener, lh_os_system_net_socket_type_tcp))
    {
        return lh_bool_false;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_net_socket_set_reuse_addr(listener, lh_bool_true));
    lh_net_ip4_t ip;
    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), port);
    if (!lh_os_net_socket_bind(listener, lh_addr_of(addr)) ||
        !lh_os_net_socket_listen(listener, 8) ||
        !lh_os_net_socket_set_blocking(listener, lh_bool_false))
    {
        lh_os_net_socket_close(listener);
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_void
lh_ui_web_host_send_response(lh_os_net_socket_t *client, const lh_http_response_t *response,
                             const lh_http_header_t *headers, lh_str_ptr scratch,
                             lh_usize_t capacity, const lh_ptr body, lh_usize_t body_size)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_send_response(client, response, headers, scratch,
                                                          capacity, body, body_size),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_ui_web_host_try_send_continue(lh_os_net_socket_t *client)
{
    lh_http_response_t response;
    lh_http_response_set_version_minor(lh_addr_of(response), 1U);
    lh_http_response_set_status(lh_addr_of(response), lh_http_status_continue);
    lh_http_response_set_reason(lh_addr_of(response),
                                lh_http_status_get_reason(lh_http_status_continue));
    lh_http_response_set_header_count(lh_addr_of(response), 0U);
    lh_char_t head[64];
    return lh_ui_web_host_try_send_response(client, lh_addr_of(response), lh_null, head,
                                            sizeof(head), lh_null, 0U);
}

lh_bool_t
lh_ui_web_host_try_reply(lh_os_net_socket_t *client, const lh_http_request_t *request,
                         lh_uint_t code, lh_str_cptr type, lh_str_cptr body, lh_usize_t len)
{
    lh_char_t length_text[LH_STR_FORMAT_ULLONG_DIGITS_MAX];
    const lh_usize_t length_size = lh_str_ptr_format_ullong(lh_cast_static(lh_ullong_t, len),
                                                            length_text, sizeof(length_text));
    if (!lh_http_status_is_valid(code))
    {
        return lh_bool_false;
    }
    const lh_str_view_t reason = lh_http_status_get_reason(code);
    const lh_bool_t content_forbidden =
        lh_http_status_is_content_forbidden(code) ||
        (lh_http_request_is_method(request, lh_http_method_connect) &&
         lh_http_status_is_success(code));
    const lh_bool_t is_head = lh_http_request_is_method(request, lh_http_method_head);
    lh_http_response_t response;
    lh_http_response_set_status(lh_addr_of(response), lh_cast_static(lh_u16_t, code));
    lh_http_response_set_version_minor(lh_addr_of(response), 1U);
    lh_http_response_set_reason(lh_addr_of(response), reason);
    lh_http_header_t headers[4];
    lh_usize_t count = 0U;
    lh_http_header_set(
        lh_addr_of(headers[count]), lh_http_header_name_content_type,
        lh_initializer_of(lh_str_view_t, lh_str_view_initializer, type, lh_str_ptr_len(type)));
    count = lh_math_add_one(count);
    if (!content_forbidden)
    {
        lh_http_header_set(
            lh_addr_of(headers[count]), lh_http_header_name_content_length,
            lh_initializer_of(lh_str_view_t, lh_str_view_initializer, length_text, length_size));
        count = lh_math_add_one(count);
    }
    lh_http_header_set(lh_addr_of(headers[count]), lh_http_header_name_connection,
                       lh_http_header_value_close);
    count = lh_math_add_one(count);
    lh_http_header_set(lh_addr_of(headers[count]), lh_http_header_name_cache_control,
                       lh_http_header_value_no_store);
    count = lh_math_add_one(count);
    lh_http_response_set_header_count(lh_addr_of(response), count);
    if (lh_math_is_zero(length_size))
    {
        return lh_bool_false;
    }
    const lh_usize_t body_size = !content_forbidden && !is_head ? len : 0U;
    lh_char_t head[256];
    return lh_ui_web_host_try_send_response(client, lh_addr_of(response), headers, head,
                                            sizeof(head), body, body_size);
}

lh_sint_t
lh_ui_web_host_get_query_int(const lh_http_request_t *request, lh_str_cptr key, lh_usize_t key_len)
{
    lh_str_cptr value = lh_null;
    const lh_usize_t n =
        lh_ui_web_host_get_query_value_size(request, key, key_len, lh_addr_of(value));
    lh_sint_t v = 0;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%d", lh_addr_of(v)) == 0u)
    {
        return 0;
    }
    return lh_cast_static(lh_sint_t, v);
}

lh_uint_t
lh_ui_web_host_get_query_uint(const lh_http_request_t *request, lh_str_cptr key, lh_usize_t key_len)
{
    lh_str_cptr value = lh_null;
    const lh_usize_t n =
        lh_ui_web_host_get_query_value_size(request, key, key_len, lh_addr_of(value));
    lh_uint_t v = 0u;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%u", lh_addr_of(v)) == 0u)
    {
        return 0u;
    }
    return lh_cast_static(lh_uint_t, v);
}

lh_bool_t
lh_ui_web_host_ws_upgrade(lh_os_net_socket_t *client, const lh_http_header_t *headers,
                          lh_usize_t count)
{
    static const lh_char_t guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const lh_str_view_t key_name = lh_http_header_name_websocket_key;

    const lh_http_header_t *key = lh_http_header_find(headers, count, lh_addr_of(key_name));
    if (key == lh_null)
    {
        return lh_bool_false;
    }
    const lh_str_view_t *value = lh_http_header_get_value(key);
    lh_usize_t n = lh_str_view_get_size(value);
    lh_char_t cat[80];

    if (n > sizeof(cat) - (sizeof(guid) - 1u))
    {
        return lh_bool_false;
    }
    lh_memory_copy(cat, sizeof(cat), lh_cast_const(lh_ptr, lh_str_view_get_data(value)), n);
    lh_memory_copy(cat + n, sizeof(cat) - n, lh_cast_const(lh_ptr, guid), sizeof(guid) - 1u);
    n += sizeof(guid) - 1u;
    lh_byte_t digest[LH_CRYPTO_SHA1_DIGEST_SIZE];
    if (lh_crypto_sha_hash(lh_crypto_sha_kind_1, cat, n, digest, sizeof(digest)) !=
        LH_CRYPTO_SHA1_DIGEST_SIZE)
    {
        return lh_bool_false;
    }
    lh_char_t accept[32];
    accept[lh_str_ptr_format_base64(digest, sizeof(digest), accept, sizeof(accept) - 1u)] =
        lh_char_map_nul;
    lh_http_response_t response;
    lh_http_response_set_status(lh_addr_of(response), lh_http_status_switching_protocols);
    lh_http_response_set_version_minor(lh_addr_of(response), 1U);
    lh_http_response_set_reason(lh_addr_of(response),
                                lh_http_status_get_reason(lh_http_status_switching_protocols));
    lh_http_response_set_header_count(lh_addr_of(response), 3U);
    lh_http_header_t response_headers[3];
    lh_http_header_set(lh_addr_of(response_headers[0]), lh_http_header_name_upgrade,
                       lh_http_header_value_websocket);
    lh_http_header_set(lh_addr_of(response_headers[1]), lh_http_header_name_connection,
                       lh_http_header_value_upgrade);
    lh_http_header_set(
        lh_addr_of(response_headers[2]), lh_http_header_name_websocket_accept,
        lh_initializer_of(lh_str_view_t, lh_str_view_initializer, accept, lh_str_ptr_len(accept)));
    lh_char_t head[280];
    if (!lh_ui_web_host_try_send_response(client, lh_addr_of(response), response_headers, head,
                                          sizeof(head), lh_null, 0U))
    {
        return lh_bool_false;
    }
    return lh_os_net_socket_set_blocking(client, lh_bool_false);
}

lh_bool_t
lh_ui_web_host_ws_enqueue(lh_ui_web_host_client_t *c, lh_uint_t opcode, const void *payload,
                          lh_usize_t plen, lh_bool_t front)
{
    const lh_usize_t need = lh_ui_web_ws_frame_size(plen);

    if (c == lh_null || !lh_ui_web_host_client_is_used(c))
    {
        return lh_bool_false;
    }
    if (need == 0u || lh_ui_web_host_client_get_queued(c) + lh_cast_static(lh_uint_t, need) >
                          LH_UI_WEB_HOST_WS_QMAX)
    {
        lh_ui_web_host_ws_drop(c);
        return lh_bool_false;
    }
    lh_ui_web_host_frame_t *q =
        lh_ptr_rcast(lh_ui_web_host_frame_t, lh_os_alloc(sizeof(lh_ptr_deref(q))));
    lh_byte_ptr bytes = lh_ptr_rcast(lh_byte_t, lh_os_alloc(need));
    if (q == lh_null || bytes == lh_null ||
        lh_ui_web_ws_write(bytes, need, opcode, lh_bool_true, payload, plen) != need)
    {
        lh_runtime_allocator_free(q);
        lh_runtime_allocator_free(bytes);
        lh_ui_web_host_ws_drop(c);
        return lh_bool_false;
    }
    lh_ui_web_host_frame_set_bytes(q, bytes);
    lh_ui_web_host_frame_set_len(q, lh_cast_static(lh_uint_t, need));
    lh_ui_web_host_frame_set_off(q, 0u);
    if (front)
    {
        lh_ui_web_host_frame_set_next(q, lh_ui_web_host_client_get_qhead(c));
        lh_ui_web_host_client_set_qhead(c, q);
        if (lh_ui_web_host_client_get_qtail(c) == lh_null)
        {
            lh_ui_web_host_client_set_qtail(c, q);
        }
    }
    else
    {
        lh_ui_web_host_frame_set_next(q, lh_null);
        if (lh_ui_web_host_client_get_qtail(c) != lh_null)
        {
            lh_ui_web_host_frame_set_next(lh_ui_web_host_client_get_qtail(c), q);
        }
        else
        {
            lh_ui_web_host_client_set_qhead(c, q);
        }
        lh_ui_web_host_client_set_qtail(c, q);
    }
    lh_ui_web_host_client_set_queued(c, lh_ui_web_host_client_get_queued(c) +
                                            (lh_cast_static(lh_uint_t, need)));
    return lh_bool_true;
}

lh_void
lh_ui_web_host_ws_flush(lh_ui_web_host_client_t *c)
{
    while (c != lh_null && lh_ui_web_host_client_is_used(c) &&
           lh_ui_web_host_client_get_qhead(c) != lh_null)
    {
        lh_ui_web_host_frame_t *q = lh_ui_web_host_client_get_qhead(c);
        const lh_ssize_t w = lh_os_net_socket_send(
            lh_ui_web_host_client_get_sock_mut(c),
            lh_ui_web_host_frame_get_bytes(q) + lh_ui_web_host_frame_get_off(q),
            lh_cast_static(lh_usize_t,
                           (lh_ui_web_host_frame_get_len(q) - lh_ui_web_host_frame_get_off(q))));

        if (w == LH_OS_RESULT_PENDING)
        {
            return;
        }
        if (w <= 0)
        {
            lh_ui_web_host_ws_drop(c);
            return;
        }
        lh_ui_web_host_frame_set_off(q, lh_ui_web_host_frame_get_off(q) +
                                            (lh_cast_static(lh_uint_t, w)));
        lh_ui_web_host_client_set_queued(c, lh_ui_web_host_client_get_queued(c) -
                                                (lh_cast_static(lh_uint_t, w)));
        if (lh_ui_web_host_frame_get_off(q) >= lh_ui_web_host_frame_get_len(q))
        {
            lh_ui_web_host_client_set_qhead(c, lh_ui_web_host_frame_get_next(q));
            if (lh_ui_web_host_client_get_qhead(c) == lh_null)
            {
                lh_ui_web_host_client_set_qtail(c, lh_null);
            }
            lh_runtime_allocator_free(lh_ui_web_host_frame_get_bytes(q));
            lh_runtime_allocator_free(q);
        }
    }
    if (c != lh_null && lh_ui_web_host_client_is_used(c) && lh_ui_web_host_client_is_closing(c) &&
        lh_ui_web_host_client_get_qhead(c) == lh_null)
    {
        lh_ui_web_host_ws_drop(c);
    }
}

lh_void
lh_ui_web_host_read_head(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                         lh_u32_t timeout_ms, lh_usize_t *received, lh_usize_t *head_size,
                         lh_http_request_t *request, lh_http_header_t *headers,
                         lh_usize_t header_capacity)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_read_head(client, buf, capacity, timeout_ms, received,
                                                      head_size, request, headers, header_capacity),
                         lh_runtime_error_code_invalid_range);
}

lh_void
lh_ui_web_host_decode_gzip_in_place(lh_str_ptr buffer, lh_usize_t encoded_size, lh_usize_t capacity,
                                    lh_usize_t *decoded_size)
{
    lh_runtime_check_ifn(
        lh_ui_web_host_try_decode_gzip_in_place(buffer, encoded_size, capacity, decoded_size),
        lh_runtime_error_code_invalid_range);
}

lh_http_parse_status_t
lh_ui_web_host_try_complete_body(lh_str_ptr buf, lh_usize_t capacity, lh_usize_t used,
                                 lh_usize_t head_size, lh_bool_t chunked, lh_usize_t body_size,
                                 lh_bool_t transfer_gzip, lh_usize_t *length, lh_str_view_t *body)
{
    if (!chunked)
    {
        if (lh_math_lt(lh_math_sub(used, head_size), body_size))
        {
            return lh_http_parse_incomplete;
        }
        lh_str_view_init_by_size(body, buf + head_size, body_size);
        lh_ptr_deref(length) = lh_math_add(head_size, body_size);
        return lh_http_parse_complete;
    }
    lh_usize_t consumed = 0U;
    lh_usize_t decoded = 0U;
    const lh_http_parse_status_t status = lh_http_chunked_decode(
        buf + head_size, lh_math_sub(used, head_size), lh_addr_of(consumed), lh_addr_of(decoded));
    if (lh_math_ne(status, lh_http_parse_complete))
    {
        return status;
    }
    if (transfer_gzip && !lh_ui_web_host_try_decode_gzip_in_place(
                             buf + head_size, decoded, lh_math_sub(capacity, head_size), &decoded))
    {
        return lh_http_parse_error;
    }
    lh_str_view_init_by_size(body, buf + head_size, decoded);
    lh_ptr_deref(length) = lh_math_add(head_size, consumed);
    return lh_http_parse_complete;
}

lh_void
lh_ui_web_host_dispatch_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                               const lh_http_request_t *request, const lh_http_header_t *headers,
                               lh_io_reader_t *reader, lh_os_net_socket_t *client)
{
    lh_runtime_check_ifn(
        lh_ui_web_host_try_dispatch_stream(app, route, request, headers, reader, client),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_ui_web_host_try_dispatch_gzip(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                                 const lh_http_request_t *request, const lh_http_header_t *headers,
                                 lh_io_reader_t *compressed, lh_os_net_socket_t *client)
{
    lh_ui_web_host_gzip_workspace_t workspace;
    lh_compress_gzip_reader_init(lh_ui_web_host_gzip_workspace_get_reader(&workspace), compressed,
                                 lh_ui_web_host_gzip_workspace_get_header(&workspace),
                                 lh_ui_web_host_gzip_workspace_get_deflate(&workspace),
                                 lh_ui_web_host_gzip_workspace_get_scratch(&workspace),
                                 lh_ui_web_host_gzip_workspace_get_history(&workspace),
                                 lh_ui_web_stream_route_get_body_limit(route),
                                 lh_ui_web_stream_route_get_metadata_limit(route));
    lh_io_reader_t reader;
    lh_compress_gzip_reader_get_reader(lh_ui_web_host_gzip_workspace_get_reader(&workspace),
                                       &reader);
    return lh_ui_web_host_try_dispatch_stream(app, route, request, headers, &reader, client);
}

lh_void
lh_ui_web_host_serve_stream(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                            lh_os_net_socket_t *client, const lh_http_request_t *request,
                            const lh_http_header_t *headers, lh_str_view_t initial, lh_u64_t start)
{
    lh_runtime_check_ifn(
        lh_ui_web_host_try_serve_stream(app, route, client, request, headers, initial, start),
        lh_runtime_error_code_invalid_range);
}

lh_void
lh_ui_web_host_settle(lh_ui_web_host_t *self, lh_ui_web_host_conn_t *conn, lh_uint_t now)
{
    const lh_ui_web_host_conn_phase_t phase = lh_ui_web_host_conn_get_phase(conn);
    if (lh_math_eq(phase, lh_ui_web_host_conn_phase_upgraded))
    {
        /* The socket moves to the clients; the slot is free without closing it. */
        lh_ui_web_host_ws_attach(lh_ui_web_host_get_clients_mut(self),
                                 lh_ui_web_host_conn_get_sock_mut(conn), now);
        lh_ui_web_host_conn_set_phase(conn, lh_ui_web_host_conn_phase_free);
    }
    else if (lh_math_eq(phase, lh_ui_web_host_conn_phase_done))
    {
        lh_ui_web_host_conn_close(conn);
    }
}

lh_void
lh_ui_web_host_prepare_poll(lh_ui_web_host_t *self)
{
    lh_os_net_socket_poll_t *entries = lh_ui_web_host_get_events_mut(self);
    lh_os_net_socket_poll_init(entries,
                               lh_os_net_socket_get_handle(lh_ui_web_host_get_listener(self)),
                               lh_os_net_socket_poll_read);
    lh_os_net_socket_poll_init(entries + 1, lh_os_wake_get_handle(lh_ui_web_host_get_wake(self)),
                               lh_os_net_socket_poll_read);
    entries = entries + 2;
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_WS_MAX); i = lh_math_add_one(i))
    {
        const lh_ui_web_host_client_t *c = lh_ui_web_host_get_clients(self) + i;
        if (!lh_ui_web_host_client_is_used(c))
        {
            lh_os_net_socket_poll_init(entries + i, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID, 0U);
            continue;
        }
        const lh_uint_t write =
            lh_null_ne(lh_ui_web_host_client_get_qhead(c)) ? lh_os_net_socket_poll_write : 0U;
        lh_os_net_socket_poll_init(entries + i,
                                   lh_os_net_socket_get_handle(lh_ui_web_host_client_get_sock(c)),
                                   lh_bit_or(lh_os_net_socket_poll_read, write));
    }
    entries = entries + LH_UI_WEB_HOST_WS_MAX;
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_CONN_MAX); i = lh_math_add_one(i))
    {
        const lh_ui_web_host_conn_t *conn = lh_ui_web_host_get_conns(self) + i;
        lh_os_net_socket_poll_init(
            entries + i,
            lh_ui_web_host_conn_is_active(conn)
                ? lh_os_net_socket_get_handle(lh_ui_web_host_conn_get_sock(conn))
                : LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID,
            lh_os_net_socket_poll_read);
    }
}

lh_void
lh_ui_web_host_wake(lh_ptr waiter)
{
    lh_ui_web_host_t *self = lh_ptr_rcast(lh_ui_web_host_t, waiter);
    LH_ATTRIBUTE_UNUSED(lh_os_wake_signal(lh_ui_web_host_get_wake_mut(self)));
}

lh_bool_t
lh_ui_web_host_try_start(lh_ui_web_host_t *self)
{
    return lh_exec_loop_try_start(lh_ui_web_host_get_loop(self));
}

lh_bool_t
lh_ui_web_host_try_stop(lh_ui_web_host_t *self, lh_u32_t timeout_ms)
{
    return lh_exec_loop_try_stop(lh_ui_web_host_get_loop(self), timeout_ms);
}

lh_void
lh_ui_web_host_destroy(lh_ui_web_host_t *self)
{
    lh_return_if(lh_null_eq(self));
    /* Joins a started loop after its last step; frees one never started. */
    lh_exec_loop_destroy(lh_ui_web_host_get_loop(self));
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_CONN_MAX); i = lh_math_add_one(i))
    {
        lh_ui_web_host_conn_t *conn = lh_ui_web_host_get_conns_mut(self) + i;
        if (lh_math_eq(lh_ui_web_host_conn_get_phase(conn), lh_ui_web_host_conn_phase_stream))
        {
            lh_ui_web_host_pipe_abort(
                lh_ui_web_host_conn_get_pipe_mut(conn), lh_ui_web_host_get_app(self),
                lh_ui_web_host_conn_get_request(conn), lh_ui_web_host_conn_get_headers(conn),
                lh_ui_web_host_conn_get_sock_mut(conn));
        }
        if (lh_math_ne(lh_ui_web_host_conn_get_phase(conn), lh_ui_web_host_conn_phase_free))
        {
            lh_ui_web_host_conn_close(conn);
        }
    }
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_WS_MAX); i = lh_math_add_one(i))
    {
        lh_ui_web_host_client_t *c = lh_ui_web_host_get_clients_mut(self) + i;
        if (lh_ui_web_host_client_is_used(c))
        {
            lh_ui_web_host_ws_drop(c);
        }
    }
    lh_os_net_socket_close(lh_ui_web_host_get_listener_mut(self));
    lh_os_wake_close(lh_ui_web_host_get_wake_mut(self));
    lh_os_net_deinit();
    LH_ATTRIBUTE_UNUSED(lh_runtime_allocator_try_free(self));
}

lh_void
lh_ui_web_host_send_continue(lh_os_net_socket_t *client)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_send_continue(client),
                         lh_runtime_error_code_invalid_range);
}

lh_void
lh_ui_web_host_reply(lh_os_net_socket_t *client, const lh_http_request_t *request, lh_uint_t code,
                     lh_str_cptr type, lh_str_cptr body, lh_usize_t len)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_reply(client, request, code, type, body, len),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_ui_web_host_try_reply_text(lh_os_net_socket_t *client, const lh_http_request_t *request,
                              lh_uint_t code, lh_str_cptr text, lh_usize_t len)
{
    return lh_ui_web_host_try_reply(client, request, code, "text/plain; charset=utf-8", text, len);
}

lh_bool_t
lh_ui_web_host_get_query_flag(const lh_http_request_t *request, lh_str_cptr key, lh_usize_t key_len,
                              lh_bool_t absent)
{
    lh_str_cptr value = lh_null;

    if (lh_ui_web_host_get_query_value_size(request, key, key_len, lh_addr_of(value)) == 0u)
    {
        return absent;
    }
    return (lh_ui_web_host_get_query_int(request, key, key_len) != 0) ? lh_bool_true
                                                                      : lh_bool_false;
}

lh_void
lh_ui_web_host_ws_ingest(lh_ui_web_host_client_t *c, lh_ui_web_app_t *app, lh_uint_t now)
{
    while (lh_ui_web_host_client_is_used(c) && lh_ui_web_host_client_get_rx_len(c) >= 2u)
    {
        lh_uint_t opcode = 0;
        lh_bool_t fin = lh_bool_false;
        lh_byte_cptr payload = lh_null;
        lh_usize_t plen = 0;
        lh_byte_cptr mask = lh_null;
        const lh_usize_t n =
            lh_ui_web_ws_read(lh_ui_web_host_client_get_rx(c), lh_ui_web_host_client_get_rx_len(c),
                              lh_addr_of(opcode), lh_addr_of(fin), lh_addr_of(payload),
                              lh_addr_of(plen), lh_addr_of(mask));
        lh_byte_ptr body;

        if (n == 0u)
        {
            return;
        }
        if (n == lh_cast_static(lh_usize_t, -1))
        {
            lh_ui_web_host_ws_drop(c);
            return;
        }
        body = lh_cast_const(lh_byte_ptr, payload);
        if (mask != lh_null)
        {
            lh_ui_web_ws_unmask(body, plen, mask);
        }
        lh_ui_web_host_client_set_last_rx_ms(c, now);
        if (opcode >= 8u)
        {
            if (!fin || plen > 125u)
            {
                lh_ui_web_host_ws_drop(c);
                return;
            }
            if (opcode == LH_UI_WEB_WS_PING)
            {
                if (!lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_PONG, body, plen, lh_bool_true))
                {
                    return;
                }
            }
            else if (opcode == LH_UI_WEB_WS_CLOSE)
            {
                lh_ui_web_host_client_set_closing(c, lh_bool_true);
                if (!lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_CLOSE, body, plen, lh_bool_true))
                {
                    return;
                }
            }
        }
        else if (opcode == LH_UI_WEB_WS_CONT)
        {
            if (!lh_ui_web_host_client_is_msg_on(c) || !lh_ui_web_host_ws_msg_add(c, body, plen))
            {
                lh_ui_web_host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (lh_ui_web_host_client_get_msg_op(c) == LH_UI_WEB_WS_TEXT)
                {
                    lh_ui_web_app_message(
                        app, lh_ptr_rcast(const lh_char_t, lh_ui_web_host_client_get_msg(c)),
                        lh_ui_web_host_client_get_msg_len(c));
                }
                lh_ui_web_host_client_set_msg_on(c, lh_bool_false);
                lh_ui_web_host_client_set_msg_len(c, 0u);
            }
        }
        else if (opcode == LH_UI_WEB_WS_TEXT || opcode == LH_UI_WEB_WS_BIN)
        {
            if (lh_ui_web_host_client_is_msg_on(c))
            {
                lh_ui_web_host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (opcode == LH_UI_WEB_WS_TEXT)
                {
                    lh_ui_web_app_message(app, lh_ptr_rcast(const lh_char_t, body), plen);
                }
            }
            else
            {
                lh_ui_web_host_client_set_msg_on(c, lh_bool_true);
                lh_ui_web_host_client_set_msg_op(c, opcode);
                lh_ui_web_host_client_set_msg_len(c, 0u);
                if (!lh_ui_web_host_ws_msg_add(c, body, plen))
                {
                    lh_ui_web_host_ws_drop(c);
                    return;
                }
            }
        }
        else
        {
            lh_ui_web_host_ws_drop(c);
            return;
        }
        lh_memory_move(lh_ui_web_host_client_get_rx(c), LH_UI_WEB_HOST_WS_RX,
                       lh_ui_web_host_client_get_rx(c) + n,
                       lh_cast_static(lh_usize_t, (lh_ui_web_host_client_get_rx_len(c) -
                                                   lh_cast_static(lh_uint_t, n))));
        lh_ui_web_host_client_set_rx_len(c, lh_ui_web_host_client_get_rx_len(c) -
                                                (lh_cast_static(lh_uint_t, n)));
    }
}

lh_void
lh_ui_web_host_dispatch_gzip(lh_ui_web_app_t *app, const lh_ui_web_stream_route_t *route,
                             const lh_http_request_t *request, const lh_http_header_t *headers,
                             lh_io_reader_t *compressed, lh_os_net_socket_t *client)
{
    lh_runtime_check_ifn(
        lh_ui_web_host_try_dispatch_gzip(app, route, request, headers, compressed, client),
        lh_runtime_error_code_invalid_range);
}

lh_void
lh_ui_web_host_ws_frame(lh_ui_web_host_client_t *cli, lh_ui_web_host_stream_t *stream,
                        lh_ui_web_app_t *app)
{
    const lh_uint_t gen = lh_ui_web_app_get_generation(app);
    const lh_str_ptr body = lh_null;
    lh_usize_t len = 0u;
    lh_bool_t want = lh_bool_false;

    for (lh_uint_t i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
    {
        if (lh_ui_web_host_ws_wants(cli + i, gen))
        {
            want = lh_bool_true;
            if (!lh_ui_web_host_client_is_synced(cli + i))
            {
                lh_ui_canvas_web_glyphs_reset(lh_ui_web_host_stream_get_glyphs_mut(stream));
            }
        }
    }
    if (!want)
    {
        return;
    }
    lh_ui_web_app_frame(app, lh_ui_web_host_stream_get_glyphs_mut(stream), lh_addr_of(body),
                        lh_addr_of(len));
    if (len == 0u)
    {
        return;
    }
    {
        const lh_bool_t same = lh_ui_web_host_ws_same(stream, body, len);

        for (lh_uint_t i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
        {
            lh_ui_web_host_client_t *c = cli + i;

            if (!lh_ui_web_host_client_is_used(c))
            {
                continue;
            }
            if (!lh_ui_web_host_ws_wants(c, gen))
            {
                /* Not sent this one: any cover it introduced, this client lacks. */
                if (!same)
                {
                    lh_ui_web_host_client_set_synced(c, lh_bool_false);
                }
                continue;
            }
            if (same && lh_ui_web_host_client_is_synced(c))
            {
                lh_ui_web_host_client_set_seen(c, gen);
                continue;
            }
            if (lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_TEXT, body, len, lh_bool_false))
            {
                lh_ui_web_host_client_set_seen(c, gen);
                lh_ui_web_host_client_set_synced(c, lh_bool_true);
                lh_ui_web_host_ws_flush(c);
            }
        }
        if (!same)
        {
            lh_ui_web_host_ws_remember(stream, body, len);
        }
    }
}

lh_void
lh_ui_web_host_accept_all(lh_ui_web_host_t *self, lh_u64_t now)
{
    for (;;)
    {
        lh_os_net_socket_t client;
        lh_net_ip4_socket_addr_t peer;
        lh_os_net_socket_init(lh_addr_of(client));
        if (!lh_os_net_socket_accept(lh_ui_web_host_get_listener_mut(self), lh_addr_of(client),
                                     lh_addr_of(peer)))
        {
            return;
        }
        lh_ui_web_host_conn_t *conn = lh_null;
        for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_CONN_MAX) && lh_null_eq(conn);
             i = lh_math_add_one(i))
        {
            lh_ui_web_host_conn_t *slot = lh_ui_web_host_get_conns_mut(self) + i;
            if (lh_math_eq(lh_ui_web_host_conn_get_phase(slot), lh_ui_web_host_conn_phase_free))
            {
                conn = slot;
            }
        }
        if (lh_null_eq(conn) || !lh_ui_web_host_conn_try_open(conn, lh_addr_of(client), now))
        {
            lh_os_net_socket_close(lh_addr_of(client));
            continue;
        }
        /* Not readable yet as far as this step knows: only what the head brought. */
        LH_ATTRIBUTE_UNUSED(
            lh_ui_web_host_conn_step(conn, lh_ui_web_host_get_app(self), now, lh_bool_false));
        lh_ui_web_host_settle(self, conn, lh_cast_static(lh_uint_t, now));
    }
}

lh_void
lh_ui_web_host_advance_conns(lh_ui_web_host_t *self, lh_u64_t now)
{
    const lh_os_net_socket_poll_t *entries =
        lh_ui_web_host_get_events(self) + lh_math_add(2U, LH_UI_WEB_HOST_WS_MAX);
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_CONN_MAX); i = lh_math_add_one(i))
    {
        lh_ui_web_host_conn_t *conn = lh_ui_web_host_get_conns_mut(self) + i;
        if (!lh_ui_web_host_conn_is_active(conn))
        {
            continue;
        }
        /* The entry is this connection's only if it watched this socket. */
        const lh_bool_t readable =
            lh_os_net_socket_poll_is_readable(entries + i) &&
            lh_math_eq(lh_os_net_socket_poll_get_handle(entries + i),
                       lh_os_net_socket_get_handle(lh_ui_web_host_conn_get_sock(conn)));
        LH_ATTRIBUTE_UNUSED(
            lh_ui_web_host_conn_step(conn, lh_ui_web_host_get_app(self), now, readable));
        lh_ui_web_host_settle(self, conn, lh_cast_static(lh_uint_t, now));
    }
}

lh_void
lh_ui_web_host_wait(lh_ptr waiter, lh_u32_t timeout_ms)
{
    lh_ui_web_host_t *self = lh_ptr_rcast(lh_ui_web_host_t, waiter);
    lh_ui_web_host_prepare_poll(self);
    if (lh_math_eq(lh_os_net_socket_poll(lh_ui_web_host_get_events_mut(self),
                                         LH_UI_WEB_HOST_POLL_MAX, timeout_ms),
                   LH_OS_RESULT_INVALID))
    {
        LH_ATTRIBUTE_UNUSED(
            lh_os_net_socket_poll(lh_ui_web_host_get_events_mut(self), 2U, timeout_ms));
    }
}

lh_ui_web_host_t *
lh_ui_web_host_try_create(lh_ui_web_app_t *app, lh_net_port_t port)
{
    lh_return_if(lh_null_eq(app), lh_null);
    lh_ui_web_host_t *self = lh_ptr_rcast(
        lh_ui_web_host_t, lh_runtime_allocator_try_alloc_zero(1U, sizeof(lh_ui_web_host_t)));
    lh_return_if(lh_null_eq(self), lh_null);
    lh_ui_web_host_set_app(self, app);
    lh_os_net_socket_init(lh_ui_web_host_get_listener_mut(self));
    lh_os_wake_init(lh_ui_web_host_get_wake_mut(self));
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_WS_MAX); i = lh_math_add_one(i))
    {
        lh_os_net_socket_init(
            lh_ui_web_host_client_get_sock_mut(lh_ui_web_host_get_clients_mut(self) + i));
    }
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_CONN_MAX); i = lh_math_add_one(i))
    {
        lh_ui_web_host_conn_t *conn = lh_ui_web_host_get_conns_mut(self) + i;
        lh_os_net_socket_init(lh_ui_web_host_conn_get_sock_mut(conn));
        lh_ui_web_host_conn_set_phase(conn, lh_ui_web_host_conn_phase_free);
    }
    if (!lh_os_net_init())
    {
        LH_ATTRIBUTE_UNUSED(lh_runtime_allocator_try_free(self));
        return lh_null;
    }
    if (!lh_ui_web_host_try_listen(lh_ui_web_host_get_listener_mut(self), port) ||
        !lh_os_wake_open(lh_ui_web_host_get_wake_mut(self), 1000U))
    {
        lh_ui_web_host_destroy(self);
        return lh_null;
    }
    lh_ui_web_host_set_loop(
        self, lh_exec_loop_try_create_waiting(LH_UI_WEB_HOST_TICK_MS, lh_ui_web_host_step, self,
                                              lh_ui_web_host_wait, lh_ui_web_host_wake, self));
    if (lh_null_eq(lh_ui_web_host_get_loop(self)))
    {
        lh_ui_web_host_destroy(self);
        return lh_null;
    }
    lh_logger_notice(lh_runtime_logger_get_mut(), "[lh] ui web host http://127.0.0.1:%u/",
                     lh_cast_static(lh_uint_t, port));
    return self;
}

lh_void
lh_ui_web_host_reply_text(lh_os_net_socket_t *client, const lh_http_request_t *request,
                          lh_uint_t code, lh_str_cptr text, lh_usize_t len)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_reply_text(client, request, code, text, len),
                         lh_runtime_error_code_invalid_range);
}

lh_void
lh_ui_web_host_ws_pump(lh_ui_web_host_client_t *c, lh_ui_web_app_t *app, lh_uint_t now)
{
    lh_ssize_t got;

    if (!lh_ui_web_host_client_is_used(c))
    {
        return;
    }
    if ((now - lh_ui_web_host_client_get_last_rx_ms(c)) > LH_UI_WEB_HOST_WS_DEAD_MS)
    {
        lh_ui_web_host_ws_drop(c);
        return;
    }
    if (!lh_ui_web_host_client_is_closing(c) &&
        (now - lh_ui_web_host_client_get_last_ping_ms(c)) >= LH_UI_WEB_HOST_WS_PING_MS)
    {
        if (!lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_PING, lh_null, 0u, lh_bool_true))
        {
            return;
        }
        lh_ui_web_host_client_set_last_ping_ms(c, now);
    }
    if (lh_ui_web_host_client_get_rx_len(c) >= LH_UI_WEB_HOST_WS_RX)
    {
        lh_ui_web_host_ws_drop(c);
        return;
    }
    got =
        lh_os_net_socket_recv(lh_ui_web_host_client_get_sock_mut(c),
                              lh_ui_web_host_client_get_rx(c) + lh_ui_web_host_client_get_rx_len(c),
                              LH_UI_WEB_HOST_WS_RX - lh_ui_web_host_client_get_rx_len(c));
    if (got == LH_OS_RESULT_PENDING)
    {
        lh_ui_web_host_ws_flush(c);
        return;
    }
    if (got <= 0)
    {
        lh_ui_web_host_ws_drop(c);
        return;
    }
    lh_ui_web_host_client_set_rx_len(c, lh_ui_web_host_client_get_rx_len(c) +
                                            (lh_cast_static(lh_uint_t, got)));
    lh_ui_web_host_ws_ingest(c, app, now);
    if (lh_ui_web_host_client_is_used(c))
    {
        lh_ui_web_host_ws_flush(c);
    }
}

lh_bool_t
lh_ui_web_host_try_begin_body(lh_os_net_socket_t *client, const lh_http_request_t *request,
                              const lh_http_header_t *headers, lh_str_view_t initial,
                              lh_usize_t decoded_limit, lh_bool_t can_receive_more,
                              lh_bool_t *chunked, lh_usize_t *length)
{
    const lh_usize_t count = lh_http_request_get_header_count(request);
    lh_bool_t is_chunked, transfer_gzip;
    if (!lh_http_framing_try_get_transfer(headers, count, lh_addr_of(is_chunked), &transfer_gzip) ||
        (is_chunked && lh_math_is_zero(lh_http_request_get_version_minor(request))))
    {
        return lh_bool_false;
    }
    lh_usize_t size = 0U;
    if (!is_chunked && !lh_http_framing_try_get_content_length(headers, count, lh_addr_of(size)))
    {
        return lh_bool_false;
    }
    if (!is_chunked && lh_math_gt(size, decoded_limit))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(
            client, request, lh_http_status_content_too_large, lh_str_lit("body too large\n")));
        return lh_bool_false;
    }
    lh_bool_t needs_body = lh_math_lt(lh_str_view_get_size(lh_addr_of(initial)), size);
    if (is_chunked)
    {
        lh_usize_t consumed;
        lh_usize_t decoded;
        const lh_http_parse_status_t status =
            lh_http_chunked_scan(initial, lh_addr_of(consumed), lh_addr_of(decoded));
        if (lh_math_eq(status, lh_http_parse_error))
        {
            return lh_bool_false;
        }
        if (lh_math_eq(status, lh_http_parse_complete) && lh_math_gt(decoded, decoded_limit))
        {
            LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(
                client, request, lh_http_status_content_too_large, lh_str_lit("body too large\n")));
            return lh_bool_false;
        }
        needs_body = lh_math_eq(status, lh_http_parse_incomplete);
    }
    if (needs_body && !can_receive_more)
    {
        return lh_bool_false;
    }
    if (!lh_math_is_zero(lh_http_request_get_version_minor(request)))
    {
        lh_bool_t expect_continue;
        if (!lh_http_expect_try_get_continue(headers, count, lh_addr_of(expect_continue)))
        {
            LH_ATTRIBUTE_UNUSED(
                lh_ui_web_host_try_reply_text(client, request, lh_http_status_expectation_failed,
                                              lh_str_lit("unsupported expectation\n")));
            return lh_bool_false;
        }
        if (expect_continue && needs_body && !lh_ui_web_host_try_send_continue(client))
        {
            return lh_bool_false;
        }
    }
    lh_ptr_deref(chunked) = is_chunked;
    lh_ptr_deref(length) = size;
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_host_handle_buffered_gzip_is_handled(lh_ui_web_app_t *app,
                                               const lh_http_request_t *request,
                                               const lh_http_header_t *headers,
                                               const lh_str_view_t *body,
                                               lh_os_net_socket_t *client)
{
    lh_usize_t size;
    const lh_usize_t encoded_size = lh_str_view_get_size(body);
    lh_byte_t decoded[LH_UI_WEB_HOST_REQUEST_CAPACITY];
    if (!lh_compress_gzip_try_decode(lh_ptr_rcast(const lh_byte_t, lh_str_view_get_data(body)),
                                     encoded_size, decoded, sizeof(decoded), &size))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(
            client, request, lh_http_status_bad_request, lh_str_lit("invalid gzip body\n")));
        return lh_bool_true;
    }
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, lh_ptr_rcast(lh_char_t, decoded), size);
    return lh_ui_web_app_handle_request_is_handled(app, request, headers, &view, client);
}

lh_bool_t
lh_ui_web_host_run(lh_ui_web_app_t *app, lh_net_port_t port)
{
    lh_ui_web_host_t *self = lh_ui_web_host_try_create(app, port);
    lh_return_if(lh_null_eq(self), lh_bool_false);
    /* The loop's body on this thread: it ends only when the loop is stopped. */
    lh_exec_loop_run(lh_ui_web_host_get_loop(self));
    lh_ui_web_host_destroy(self);
    return lh_bool_true;
}

lh_void
lh_ui_web_host_begin_body(lh_os_net_socket_t *client, const lh_http_request_t *request,
                          const lh_http_header_t *headers, lh_str_view_t initial,
                          lh_usize_t decoded_limit, lh_bool_t can_receive_more, lh_bool_t *chunked,
                          lh_usize_t *length)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_begin_body(client, request, headers, initial,
                                                       decoded_limit, can_receive_more, chunked,
                                                       length),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_ui_web_host_try_finish_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                                  lh_u32_t timeout_ms, lh_usize_t received, lh_usize_t head_size,
                                  lh_http_request_t *request, lh_http_header_t *headers,
                                  lh_usize_t *length, lh_str_view_t *body)
{
    lh_bool_t transfer_chunked, transfer_gzip;
    if (!lh_http_framing_try_get_transfer(headers, lh_http_request_get_header_count(request),
                                          &transfer_chunked, &transfer_gzip))
    {
        return lh_bool_false;
    }
    const lh_u64_t start = lh_os_tick_ms();
    lh_usize_t used = received;
    lh_str_view_t initial;
    lh_str_view_init_by_size(lh_addr_of(initial), buf + head_size, lh_math_sub(used, head_size));
    lh_bool_t chunked = lh_bool_false;
    lh_usize_t body_size;
    if (!lh_ui_web_host_try_begin_body(client, request, headers, initial,
                                       lh_math_sub(capacity, head_size), lh_math_lt(used, capacity),
                                       lh_addr_of(chunked), lh_addr_of(body_size)))
    {
        return lh_bool_false;
    }
    lh_os_net_socket_t *const watch[] = {client};
    for (;;)
    {
        const lh_http_parse_status_t status = lh_ui_web_host_try_complete_body(
            buf, capacity, used, head_size, chunked, body_size, transfer_gzip, length, body);
        if (lh_math_ne(status, lh_http_parse_incomplete))
        {
            return lh_math_eq(status, lh_http_parse_complete);
        }
        const lh_u64_t elapsed = lh_math_sub(lh_os_tick_ms(), start);
        if (lh_math_ge(used, capacity) || lh_math_ge(elapsed, timeout_ms) ||
            lh_math_le(
                lh_os_net_socket_wait(watch, 1U, lh_null, 0U,
                                      lh_cast_static(lh_u32_t, lh_math_sub(timeout_ms, elapsed))),
                0))
        {
            return lh_bool_false;
        }
        const lh_ssize_t got =
            lh_os_net_socket_recv(client, buf + used, lh_math_sub(capacity, used));
        if (lh_math_le(got, 0))
        {
            return lh_bool_false;
        }
        used = lh_math_add(used, lh_cast_static(lh_usize_t, got));
    }
}

lh_void
lh_ui_web_host_step(lh_ptr user)
{
    lh_ui_web_host_t *self = lh_ptr_rcast(lh_ui_web_host_t, user);
    lh_ui_web_app_t *app = lh_ui_web_host_get_app(self);
    lh_os_net_socket_poll_t *entries = lh_ui_web_host_get_events_mut(self);
    const lh_u64_t now = lh_os_tick_ms64();
    const lh_uint_t tick = lh_cast_static(lh_uint_t, lh_os_tick_ms());
    if (lh_os_net_socket_poll_is_readable(entries + 1))
    {
        LH_ATTRIBUTE_UNUSED(lh_os_wake_reset(lh_ui_web_host_get_wake_mut(self)));
    }
    lh_ui_web_app_tick(app);
    /* Connections first: a slot freed here and taken by an accept below must not
       see the readiness of the socket it held before. */
    lh_ui_web_host_advance_conns(self, now);
    lh_ui_web_host_accept_all(self, now);
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_WS_MAX); i = lh_math_add_one(i))
    {
        lh_ui_web_host_ws_pump(lh_ui_web_host_get_clients_mut(self) + i, app, tick);
    }
    lh_ui_web_host_ws_frame(lh_ui_web_host_get_clients_mut(self),
                            lh_ui_web_host_get_stream_mut(self), app);
    /* A step that comes without a wait in between (a wake already pending) must
       not take this readiness for new. */
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_UI_WEB_HOST_POLL_MAX); i = lh_math_add_one(i))
    {
        lh_os_net_socket_poll_set_ready(entries + i, 0U);
    }
}

lh_void
lh_ui_web_host_finish_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                              lh_u32_t timeout_ms, lh_usize_t received, lh_usize_t head_size,
                              lh_http_request_t *request, lh_http_header_t *headers,
                              lh_usize_t *length, lh_str_view_t *body)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_finish_request(client, buf, capacity, timeout_ms,
                                                           received, head_size, request, headers,
                                                           length, body),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_ui_web_host_try_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                                lh_u32_t timeout_ms, lh_usize_t *length, lh_str_view_t *body,
                                lh_http_request_t *request, lh_http_header_t *headers,
                                lh_usize_t header_capacity)
{
    const lh_u64_t start = lh_os_tick_ms();
    lh_usize_t used = 0U;
    lh_usize_t head_size;
    if (!lh_ui_web_host_try_read_head(client, buf, capacity, timeout_ms, lh_addr_of(used),
                                      lh_addr_of(head_size), request, headers, header_capacity))
    {
        return lh_bool_false;
    }
    const lh_u64_t elapsed = lh_math_sub(lh_os_tick_ms(), start);
    const lh_u32_t remaining = lh_math_ge(elapsed, timeout_ms)
                                   ? 0U
                                   : lh_cast_static(lh_u32_t, lh_math_sub(timeout_ms, elapsed));
    return lh_ui_web_host_try_finish_request(client, buf, capacity, remaining, used, head_size,
                                             request, headers, length, body);
}

lh_void
lh_ui_web_host_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                            lh_u32_t timeout_ms, lh_usize_t *length, lh_str_view_t *body,
                            lh_http_request_t *request, lh_http_header_t *headers,
                            lh_usize_t header_capacity)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_read_request(client, buf, capacity, timeout_ms, length,
                                                         body, request, headers, header_capacity),
                         lh_runtime_error_code_invalid_range);
}
