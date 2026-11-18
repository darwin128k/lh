/**
 * @file web.c
 * @brief Implementation of `lh/gx/web.h`.
 */

#include <lh/gx/web.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/http/method.h>
#include <lh/core/http/status.h>
#include <lh/core/util/return.h>
#include <lh/gx/web/input.h>
#include <lh/gx/web/page.h>
#include <lh/os/web/host.h>

const lh_gx_render_t *
lh_gx_web_get_render(const lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->render);
}

lh_gx_render_t *
lh_gx_web_get_render_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->render);
}

lh_gx_canvas_t *
lh_gx_web_get_canvas_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->canvas);
}

lh_gx_backend_web_t *
lh_gx_web_get_backend_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->backend);
}

lh_web_json_writer_t *
lh_gx_web_get_writer_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->writer);
}

const lh_str_t *
lh_gx_web_get_frame(const lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->frame);
}

lh_str_t *
lh_gx_web_get_frame_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->frame);
}

const lh_str_t *
lh_gx_web_get_page(const lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->page);
}

lh_str_t *
lh_gx_web_get_page_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->page);
}

const lh_gx_pointer_t *
lh_gx_web_get_pointer(const lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointer);
}

lh_gx_pointer_t *
lh_gx_web_get_pointer_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointer);
}

const lh_gx_focus_t *
lh_gx_web_get_focus(const lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->focus);
}

lh_gx_focus_t *
lh_gx_web_get_focus_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->focus);
}

lh_os_web_app_route_t *
lh_gx_web_get_routes_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return self->routes;
}

const lh_os_web_app_t *
lh_gx_web_get_app(const lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->app);
}

lh_os_web_app_t *
lh_gx_web_get_app_mut(lh_gx_web_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->app);
}

lh_gx_entity_t *
lh_gx_web_get_screen_mut(lh_gx_web_t *self)
{
    return lh_gx_render_get_screen_mut(lh_gx_web_get_render_mut(self));
}

lh_bool_t
lh_gx_web_bump_if_changed(lh_gx_web_t *self)
{
    lh_return_if(!lh_gx_render_has_damage(lh_gx_web_get_render(self)), lh_bool_false);
    lh_os_web_app_bump(lh_gx_web_get_app_mut(self));
    return lh_bool_true;
}

lh_usize_t
lh_gx_web_draw(lh_gx_web_t *self, lh_bool_t restart)
{
    lh_gx_render_t *render = lh_gx_web_get_render_mut(self);
    lh_gx_backend_web_t *backend = lh_gx_web_get_backend_mut(self);
    lh_gx_damage_t drawn;

    if (restart)
    {
        lh_gx_render_add_damage(render, lh_gx_entity_get_place(lh_gx_render_get_screen(render)));
    }
    lh_str_clear(lh_gx_web_get_frame_mut(self));
    lh_gx_backend_web_begin(backend);
    lh_gx_render_draw(render, lh_addr_of(drawn));
    lh_gx_backend_web_end(backend);
    return lh_gx_backend_web_get_count(backend);
}

lh_bool_t
lh_gx_web_serve_page(lh_ptr ctx, const lh_http_request_t *request, const lh_http_header_t *headers,
                     const lh_str_view_t *body, lh_os_net_socket_t *client)
{
    const lh_gx_web_t *self = lh_ptr_rcast(const lh_gx_web_t, ctx);
    const lh_str_t *page = lh_gx_web_get_page(self);

    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(body);
    LH_ATTRIBUTE_UNUSED(lh_os_web_host_try_reply(client, request, lh_http_status_ok,
                                                 "text/html; charset=utf-8", lh_str_get_data(page),
                                                 lh_str_get_size(page)));
    return lh_bool_true;
}

lh_void
lh_gx_web_deinit(lh_gx_web_t *self)
{
    lh_str_deinit(lh_gx_web_get_page_mut(self));
    lh_str_deinit(lh_gx_web_get_frame_mut(self));
}

lh_bool_t
lh_gx_web_take_line(lh_gx_web_t *self, const lh_char_t *text, lh_usize_t size)
{
    lh_gx_event_t event;

    lh_return_if(!lh_gx_web_input_try_parse(text, size, lh_addr_of(event)), lh_bool_false);
    if (lh_gx_event_is_pointer(lh_addr_of(event)))
    {
        lh_gx_pointer_t *pointer = lh_gx_web_get_pointer_mut(self);

        lh_gx_pointer_take(pointer, lh_gx_web_get_screen_mut(self), lh_addr_of(event));
        if (lh_gx_event_get_kind(lh_addr_of(event)) == lh_gx_event_kind_press)
        {
            lh_gx_focus_press(lh_gx_web_get_focus_mut(self), lh_gx_pointer_get_captured(pointer));
        }
    }
    else
    {
        LH_ATTRIBUTE_UNUSED(lh_gx_focus_take(lh_gx_web_get_focus(self), lh_addr_of(event)));
    }
    LH_ATTRIBUTE_UNUSED(lh_gx_web_bump_if_changed(self));
    return lh_bool_true;
}

lh_void
lh_gx_web_frame(lh_ptr ctx, lh_bool_t restart, const lh_str_ptr *bytes, lh_usize_t *len)
{
    lh_gx_web_t *self = lh_ptr_rcast(lh_gx_web_t, ctx);
    const lh_usize_t count = lh_gx_web_draw(self, restart);
    const lh_str_t *frame = lh_gx_web_get_frame(self);
    const lh_bool_t send = lh_cast_static(lh_bool_t, count != 0U || restart);

    lh_ptr_deref(bytes) = send ? lh_str_get_data(frame) : lh_null;
    lh_ptr_deref(len) = send ? lh_str_get_size(frame) : 0U;
}

lh_void
lh_gx_web_tick(lh_ptr ctx)
{
    LH_ATTRIBUTE_UNUSED(lh_gx_web_bump_if_changed(lh_ptr_rcast(lh_gx_web_t, ctx)));
}

lh_void
lh_gx_web_message(lh_ptr ctx, const lh_str_ptr text, lh_usize_t len)
{
    LH_ATTRIBUTE_UNUSED(lh_gx_web_take_line(lh_ptr_rcast(lh_gx_web_t, ctx), text, len));
}

lh_void
lh_gx_web_init(lh_gx_web_t *self, lh_gx_scalar_t width, lh_gx_scalar_t height)
{
    lh_str_t *frame = lh_gx_web_get_frame_mut(self);
    lh_str_t *page = lh_gx_web_get_page_mut(self);
    lh_gx_canvas_t *canvas = lh_gx_web_get_canvas_mut(self);
    lh_gx_backend_web_t *backend = lh_gx_web_get_backend_mut(self);
    lh_os_web_app_route_t *route = lh_gx_web_get_routes_mut(self);
    lh_os_web_app_t *app = lh_gx_web_get_app_mut(self);
    const lh_str_view_t get = lh_http_method_get_name(lh_http_method_get);

    lh_str_init(frame);
    lh_web_json_writer_init(lh_gx_web_get_writer_mut(self), frame, 0U);
    lh_gx_backend_web_init(backend, lh_gx_web_get_writer_mut(self));
    lh_gx_backend_web_canvas_init(canvas);
    lh_gx_render_init(lh_gx_web_get_render_mut(self), canvas, backend, width, height);
    lh_gx_pointer_init(lh_gx_web_get_pointer_mut(self));
    lh_gx_focus_init(lh_gx_web_get_focus_mut(self));
    lh_str_init(page);
    lh_gx_web_page_write(page, lh_cast_static(lh_uint_t, width), lh_cast_static(lh_uint_t, height));
    lh_os_web_app_route_set_method(route, lh_str_view_get_data(lh_addr_of(get)));
    lh_os_web_app_route_set_method_len(route, lh_str_view_get_size(lh_addr_of(get)));
    lh_os_web_app_route_set_path(route, "/");
    lh_os_web_app_route_set_path_len(route, lh_str_lit_size("/"));
    lh_os_web_app_route_set_handler(route, lh_gx_web_serve_page);
    lh_os_web_app_init(app, route, LH_GX_WEB_ROUTES);
    lh_os_web_app_set_ctx(app, self);
    lh_os_web_app_set_ws_path(app, LH_GX_WEB_PAGE_WS_PATH);
    lh_os_web_app_set_ws_path_len(app, lh_str_lit_size(LH_GX_WEB_PAGE_WS_PATH));
    lh_os_web_app_set_frame_fn(app, lh_gx_web_frame);
    lh_os_web_app_set_message_fn(app, lh_gx_web_message);
    lh_os_web_app_set_tick_fn(app, lh_gx_web_tick);
    lh_os_web_app_set_generation(app, 1U);
}
