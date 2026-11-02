/**
 * @file app.c
 * @brief Implementation of `lh/ui/web/app.h`.
 */

#include <lh/ui/web/app.h>

const lh_ui_web_route_t *
lh_ui_web_app_get_routes(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->routes;
}

lh_void
lh_ui_web_app_set_routes(lh_ui_web_app_t *self, const lh_ui_web_route_t *routes)
{
    lh_assert_runtime_ref(self);
    self->routes = routes;
}

lh_usize_t
lh_ui_web_app_get_nroutes(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->nroutes;
}

lh_void
lh_ui_web_app_set_nroutes(lh_ui_web_app_t *self, lh_usize_t nroutes)
{
    lh_assert_runtime_ref(self);
    self->nroutes = nroutes;
}

const lh_str_ptr
lh_ui_web_app_get_ws_path(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ws_path;
}

lh_void
lh_ui_web_app_set_ws_path(lh_ui_web_app_t *self, const lh_str_ptr ws_path)
{
    lh_assert_runtime_ref(self);
    self->ws_path = ws_path;
}

lh_usize_t
lh_ui_web_app_get_ws_path_len(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ws_path_len;
}

lh_void
lh_ui_web_app_set_ws_path_len(lh_ui_web_app_t *self, lh_usize_t ws_path_len)
{
    lh_assert_runtime_ref(self);
    self->ws_path_len = ws_path_len;
}

lh_ui_web_app_frame_fn
lh_ui_web_app_get_frame_fn(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frame_fn;
}

lh_void
lh_ui_web_app_set_frame_fn(lh_ui_web_app_t *self, lh_ui_web_app_frame_fn frame_fn)
{
    lh_assert_runtime_ref(self);
    self->frame_fn = frame_fn;
}

lh_ui_web_app_message_fn
lh_ui_web_app_get_message_fn(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->message_fn;
}

lh_void
lh_ui_web_app_set_message_fn(lh_ui_web_app_t *self, lh_ui_web_app_message_fn message_fn)
{
    lh_assert_runtime_ref(self);
    self->message_fn = message_fn;
}

lh_ui_web_app_tick_fn
lh_ui_web_app_get_tick_fn(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tick_fn;
}

lh_void
lh_ui_web_app_set_tick_fn(lh_ui_web_app_t *self, lh_ui_web_app_tick_fn tick_fn)
{
    lh_assert_runtime_ref(self);
    self->tick_fn = tick_fn;
}

lh_ptr
lh_ui_web_app_get_ctx(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ctx;
}

lh_void
lh_ui_web_app_set_ctx(lh_ui_web_app_t *self, lh_ptr ctx)
{
    lh_assert_runtime_ref(self);
    self->ctx = ctx;
}

lh_uint_t
lh_ui_web_app_get_generation(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->generation;
}

lh_void
lh_ui_web_app_set_generation(lh_ui_web_app_t *self, lh_uint_t generation)
{
    lh_assert_runtime_ref(self);
    self->generation = generation;
}

lh_void
lh_ui_web_app_bump(lh_ui_web_app_t *self)
{
    lh_ui_web_app_set_generation(self, lh_ui_web_app_get_generation(self) + 1u);
}

lh_bool_t
lh_ui_web_app_is_websocket(const lh_ui_web_app_t *self, const lh_http_request_t *request)
{
    const lh_usize_t path_len = lh_ui_web_app_get_ws_path_len(self);
    if (lh_math_is_zero(path_len))
    {
        return lh_bool_false;
    }
    const lh_str_view_t path = lh_http_request_get_path(request);
    const lh_str_view_t expected =
        lh_str_view_initializer(lh_ui_web_app_get_ws_path(self), path_len);
    return lh_str_view_equals(lh_addr_of(path), lh_addr_of(expected), lh_bool_false);
}

lh_bool_t
lh_ui_web_app_handle_request_is_handled(lh_ui_web_app_t *self, const lh_str_ptr req, lh_usize_t len,
                                        lh_os_net_socket_t *client)
{
    const lh_ui_web_route_t *routes = lh_ui_web_app_get_routes(self);
    const lh_usize_t nroutes = lh_ui_web_app_get_nroutes(self);

    lh_assert_runtime_ref(self);
    for (lh_usize_t i = 0u; i < nroutes; ++i)
    {
        const lh_ui_web_route_t *route = lh_addr_of(routes[i]);
        lh_ui_web_route_fn handler;

        if (!lh_ui_web_route_match(route, req, len))
        {
            continue;
        }
        handler = lh_ui_web_route_get_handler(route);
        if (handler == lh_null)
        {
            continue;
        }
        if (handler(lh_ui_web_app_get_ctx(self), req, len, client))
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_ui_web_app_frame(lh_ui_web_app_t *self, lh_ui_canvas_web_glyphs_t *glyphs, const lh_str_ptr *bytes,
                    lh_usize_t *len)
{
    const lh_ui_web_app_frame_fn frame_fn = lh_ui_web_app_get_frame_fn(self);

    lh_assert_runtime_ref(self);
    if (bytes != lh_null)
    {
        *bytes = lh_null;
    }
    if (len != lh_null)
    {
        *len = 0u;
    }
    if (frame_fn == lh_null)
    {
        return;
    }
    frame_fn(lh_ui_web_app_get_ctx(self), glyphs, bytes, len);
}

lh_void
lh_ui_web_app_message(lh_ui_web_app_t *self, const lh_str_ptr text, lh_usize_t len)
{
    const lh_ui_web_app_message_fn message_fn = lh_ui_web_app_get_message_fn(self);

    if (message_fn != lh_null)
    {
        message_fn(lh_ui_web_app_get_ctx(self), text, len);
    }
}

lh_void
lh_ui_web_app_tick(lh_ui_web_app_t *self)
{
    const lh_ui_web_app_tick_fn tick_fn = lh_ui_web_app_get_tick_fn(self);

    if (tick_fn != lh_null)
    {
        tick_fn(lh_ui_web_app_get_ctx(self));
    }
}

lh_void
lh_ui_web_app_init(lh_ui_web_app_t *self, const lh_ui_web_route_t *routes, lh_usize_t nroutes)
{
    lh_ui_web_app_set_routes(self, routes);
    lh_ui_web_app_set_nroutes(self, nroutes);
    lh_ui_web_app_set_ws_path(self, lh_null);
    lh_ui_web_app_set_ws_path_len(self, 0u);
    lh_ui_web_app_set_frame_fn(self, lh_null);
    lh_ui_web_app_set_message_fn(self, lh_null);
    lh_ui_web_app_set_tick_fn(self, lh_null);
    lh_ui_web_app_set_ctx(self, lh_null);
    lh_ui_web_app_set_generation(self, 0u);
}
