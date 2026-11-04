/**
 * @file pipe.c
 * @brief Implementation of `lh/ui/web/host/pipe.h`.
 */

#include <lh/ui/web/host/pipe.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/http/content/encoding.h>
#include <lh/core/http/framing.h>
#include <lh/core/http/status.h>
#include <lh/core/memory/std.h>
#include <lh/ui/web/host.h>

const lh_ui_web_stream_route_t *
lh_ui_web_host_pipe_get_route(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return self->route;
}

lh_void
lh_ui_web_host_pipe_set_route(lh_ui_web_host_pipe_t *self, const lh_ui_web_stream_route_t *value)
{
    lh_assert_runtime_ref(self);
    self->route = value;
}

lh_u64_t
lh_ui_web_host_pipe_get_start(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_ui_web_host_pipe_set_start(lh_ui_web_host_pipe_t *self, lh_u64_t value)
{
    lh_assert_runtime_ref(self);
    self->start = value;
}

const lh_char_t *
lh_ui_web_host_pipe_get_wire(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wire;
}

lh_char_t *
lh_ui_web_host_pipe_get_wire_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wire;
}

const lh_http_chunked_decoder_t *
lh_ui_web_host_pipe_get_decoder(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->decoder);
}

lh_http_chunked_decoder_t *
lh_ui_web_host_pipe_get_decoder_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->decoder);
}

const lh_http_body_reader_t *
lh_ui_web_host_pipe_get_body(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->body);
}

lh_http_body_reader_t *
lh_ui_web_host_pipe_get_body_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->body);
}

const lh_io_reader_t *
lh_ui_web_host_pipe_get_body_io(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->body_io);
}

lh_io_reader_t *
lh_ui_web_host_pipe_get_body_io_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->body_io);
}

const lh_ui_web_host_gate_t *
lh_ui_web_host_pipe_get_gate(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gate);
}

lh_ui_web_host_gate_t *
lh_ui_web_host_pipe_get_gate_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gate);
}

const lh_io_reader_t *
lh_ui_web_host_pipe_get_gate_io(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gate_io);
}

lh_io_reader_t *
lh_ui_web_host_pipe_get_gate_io_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gate_io);
}

const lh_ui_web_host_gzip_workspace_t *
lh_ui_web_host_pipe_get_gzip(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gzip);
}

lh_ui_web_host_gzip_workspace_t *
lh_ui_web_host_pipe_get_gzip_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gzip);
}

const lh_io_reader_t *
lh_ui_web_host_pipe_get_gzip_io(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gzip_io);
}

lh_io_reader_t *
lh_ui_web_host_pipe_get_gzip_io_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->gzip_io);
}

lh_bool_t
lh_ui_web_host_pipe_is_gzip_on(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gzip_on;
}

lh_void
lh_ui_web_host_pipe_set_gzip_on(lh_ui_web_host_pipe_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->gzip_on = value;
}

const lh_ui_web_host_tap_t *
lh_ui_web_host_pipe_get_tap(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tap);
}

lh_ui_web_host_tap_t *
lh_ui_web_host_pipe_get_tap_mut(lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tap);
}

lh_bool_t
lh_ui_web_host_pipe_is_draining(const lh_ui_web_host_pipe_t *self)
{
    lh_assert_runtime_ref(self);
    return self->draining;
}

lh_void
lh_ui_web_host_pipe_set_draining(lh_ui_web_host_pipe_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->draining = value;
}

lh_bool_t
lh_ui_web_host_pipe_is_expired(const lh_ui_web_host_pipe_t *self, lh_u64_t now)
{
    return lh_math_ge(lh_math_sub(now, lh_ui_web_host_pipe_get_start(self)),
                      lh_cast_static(lh_u64_t, lh_ui_web_stream_route_get_timeout_ms(
                                                   lh_ui_web_host_pipe_get_route(self))));
}

lh_bool_t
lh_ui_web_host_pipe_try_begin(lh_ui_web_host_pipe_t *self, const lh_ui_web_stream_route_t *route,
                              lh_os_net_socket_t *client, const lh_http_request_t *request,
                              const lh_http_header_t *headers, lh_str_view_t initial,
                              lh_u64_t start)
{
    const lh_usize_t available = lh_str_view_get_size(lh_addr_of(initial));
    if (lh_math_gt(available, LH_UI_WEB_HOST_REQUEST_CAPACITY))
    {
        return lh_bool_false;
    }
    const lh_usize_t count = lh_http_request_get_header_count(request);
    lh_bool_t gzip;
    if (!lh_http_content_encoding_try_is_gzip(headers, count, lh_addr_of(gzip)))
    {
        LH_ATTRIBUTE_UNUSED(
            lh_ui_web_host_try_reply_text(client, request, lh_http_status_unsupported_media_type,
                                          lh_str_lit("unsupported content encoding\n")));
        return lh_bool_false;
    }
    lh_bool_t transfer_chunked = lh_bool_false;
    lh_bool_t transfer_gzip = lh_bool_false;
    if (!lh_http_framing_try_get_transfer(headers, count, lh_addr_of(transfer_chunked),
                                          lh_addr_of(transfer_gzip)))
    {
        return lh_bool_false;
    }
    if (gzip && transfer_gzip)
    {
        LH_ATTRIBUTE_UNUSED(
            lh_ui_web_host_try_reply_text(client, request, lh_http_status_unsupported_media_type,
                                          lh_str_lit("unsupported coding chain\n")));
        return lh_bool_false;
    }
    gzip = gzip || transfer_gzip;
    const lh_usize_t content_limit = gzip ? lh_ui_web_stream_route_get_wire_limit(route)
                                          : lh_ui_web_stream_route_get_body_limit(route);
    lh_bool_t chunked = lh_bool_false;
    lh_usize_t length = 0U;
    if (!lh_ui_web_host_try_begin_body(client, request, headers, initial, content_limit,
                                       lh_bool_true, lh_addr_of(chunked), lh_addr_of(length)))
    {
        return lh_bool_false;
    }
    lh_str_ptr wire = lh_ui_web_host_pipe_get_wire_mut(self);
    if (!lh_math_is_zero(available))
    {
        lh_memory_std_copy(wire, lh_str_view_get_begin(lh_addr_of(initial)), available);
    }
    lh_ui_web_host_pipe_set_route(self, route);
    lh_ui_web_host_pipe_set_start(self, start);
    lh_ui_web_host_pipe_set_draining(self, lh_bool_false);
    lh_ui_web_host_pipe_set_gzip_on(self, gzip);
    lh_ui_web_host_gate_init(lh_ui_web_host_pipe_get_gate_mut(self), client);
    lh_ui_web_host_gate_get_reader(lh_ui_web_host_pipe_get_gate_mut(self),
                                   lh_ui_web_host_pipe_get_gate_io_mut(self));
    if (chunked)
    {
        lh_http_chunked_decoder_init(lh_ui_web_host_pipe_get_decoder_mut(self),
                                     wire + LH_UI_WEB_HOST_REQUEST_CAPACITY,
                                     LH_UI_WEB_HOST_CHUNK_LINE_CAPACITY, content_limit,
                                     lh_ui_web_stream_route_get_metadata_limit(route));
        lh_http_body_reader_init_chunked(
            lh_ui_web_host_pipe_get_body_mut(self), lh_ui_web_host_pipe_get_gate_io_mut(self), wire,
            LH_UI_WEB_HOST_REQUEST_CAPACITY, available, lh_ui_web_host_pipe_get_decoder_mut(self));
    }
    else
    {
        lh_http_body_reader_init_length(lh_ui_web_host_pipe_get_body_mut(self),
                                        lh_ui_web_host_pipe_get_gate_io_mut(self), wire,
                                        LH_UI_WEB_HOST_REQUEST_CAPACITY, available, length);
    }
    lh_http_body_reader_get_reader(lh_ui_web_host_pipe_get_body_mut(self),
                                   lh_ui_web_host_pipe_get_body_io_mut(self));
    if (!gzip)
    {
        lh_ui_web_host_tap_init(lh_ui_web_host_pipe_get_tap_mut(self),
                                lh_ui_web_host_pipe_get_body_io(self));
        return lh_bool_true;
    }
    lh_ui_web_host_gzip_workspace_t *workspace = lh_ui_web_host_pipe_get_gzip_mut(self);
    lh_compress_gzip_reader_init(lh_ui_web_host_gzip_workspace_get_reader(workspace),
                                 lh_ui_web_host_pipe_get_body_io_mut(self),
                                 lh_ui_web_host_gzip_workspace_get_header(workspace),
                                 lh_ui_web_host_gzip_workspace_get_deflate(workspace),
                                 lh_ui_web_host_gzip_workspace_get_scratch(workspace),
                                 lh_ui_web_host_gzip_workspace_get_history(workspace),
                                 lh_ui_web_stream_route_get_body_limit(route),
                                 lh_ui_web_stream_route_get_metadata_limit(route));
    lh_compress_gzip_reader_get_reader(lh_ui_web_host_gzip_workspace_get_reader(workspace),
                                       lh_ui_web_host_pipe_get_gzip_io_mut(self));
    lh_ui_web_host_tap_init(lh_ui_web_host_pipe_get_tap_mut(self),
                            lh_ui_web_host_pipe_get_gzip_io(self));
    return lh_bool_true;
}

lh_void
lh_ui_web_host_pipe_set_readable(lh_ui_web_host_pipe_t *self)
{
    lh_ui_web_host_gate_set_ready(lh_ui_web_host_pipe_get_gate_mut(self), lh_bool_true);
}

lh_void
lh_ui_web_host_pipe_abort(lh_ui_web_host_pipe_t *self, lh_ui_web_app_t *app,
                          const lh_http_request_t *request, const lh_http_header_t *headers,
                          lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(lh_ui_web_host_finish_stream(app, lh_ui_web_host_pipe_get_route(self),
                                                     request, headers, client, lh_bool_false));
}

lh_u32_t
lh_ui_web_host_pipe_get_remaining_ms(const lh_ui_web_host_pipe_t *self, lh_u64_t now)
{
    if (lh_ui_web_host_pipe_is_expired(self, now))
    {
        return 0U;
    }
    const lh_u64_t elapsed = lh_math_sub(now, lh_ui_web_host_pipe_get_start(self));
    return lh_cast_static(
        lh_u32_t, lh_math_sub(lh_cast_static(lh_u64_t, lh_ui_web_stream_route_get_timeout_ms(
                                                           lh_ui_web_host_pipe_get_route(self))),
                              elapsed));
}

lh_ui_web_host_pipe_status_t
lh_ui_web_host_pipe_step(lh_ui_web_host_pipe_t *self, lh_ui_web_app_t *app,
                         const lh_http_request_t *request, const lh_http_header_t *headers,
                         lh_os_net_socket_t *client, lh_u64_t now)
{
    const lh_ui_web_stream_route_t *route = lh_ui_web_host_pipe_get_route(self);
    lh_ui_web_host_tap_t *tap = lh_ui_web_host_pipe_get_tap_mut(self);
    lh_io_reader_t reader;
    lh_ui_web_host_tap_get_reader(tap, lh_addr_of(reader));
    lh_bool_t ok = !lh_ui_web_host_pipe_is_expired(self, now);
    if (ok && !lh_ui_web_host_pipe_is_draining(self))
    {
        ok = lh_ui_web_stream_route_get_handler(route)(lh_ui_web_app_get_ctx(app), request, headers,
                                                       lh_addr_of(reader));
        if (ok && lh_ui_web_host_tap_is_pending(tap))
        {
            return lh_ui_web_host_pipe_status_pending;
        }
        ok = ok && !lh_ui_web_host_tap_is_failed(tap);
        /* Stopped before the end: the rest is validated, not given to the handler. */
        lh_ui_web_host_pipe_set_draining(self, ok && !lh_ui_web_host_tap_is_at_end(tap));
    }
    if (ok && lh_ui_web_host_pipe_is_draining(self))
    {
        lh_char_t discard[LH_UI_WEB_HOST_REQUEST_CAPACITY];
        lh_ssize_t got;
        do
        {
            got = lh_io_reader_read(lh_addr_of(reader), discard, sizeof(discard));
        } while (lh_math_gt(got, 0));
        if (lh_math_eq(got, LH_IO_PENDING))
        {
            return lh_ui_web_host_pipe_status_pending;
        }
        ok = lh_math_is_zero(got);
    }
    return lh_ui_web_host_finish_stream(app, route, request, headers, client, ok)
               ? lh_ui_web_host_pipe_status_committed
               : lh_ui_web_host_pipe_status_aborted;
}
