/**
 * @file app.c
 * @brief Implementation of `lh/ui/web/app.h`.
 */

#include <lh/ui/web/app.h>

#include <lh/util/str/ptr.h>

lh_void
lh_ui_web_app_init(lh_ui_web_app_t *self, const lh_ui_web_route_t *routes, lh_usize_t nroutes)
{
    lh_assert_runtime_ref(self);
    self->routes = routes;
    self->nroutes = nroutes;
    self->ws_path = lh_null;
    self->ws_path_len = 0u;
    self->frame_fn = lh_null;
    self->message_fn = lh_null;
    self->tick_fn = lh_null;
    self->ctx = lh_null;
    self->generation = 0u;
}

const lh_ui_web_route_t *
lh_ui_web_app_get_routes(const lh_ui_web_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->routes;
}

lh_void
lh_ui_web_app_set_routes(lh_ui_web_app_t *self, const lh_ui_web_route_t *routes, lh_usize_t nroutes)
{
    lh_assert_runtime_ref(self);
    self->routes = routes;
    self->nroutes = nroutes;
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
lh_ui_web_app_set_ws_path(lh_ui_web_app_t *self, const lh_str_ptr ws_path, lh_usize_t ws_path_len)
{
    lh_assert_runtime_ref(self);
    self->ws_path = ws_path;
    self->ws_path_len = ws_path_len;
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
    lh_assert_runtime_ref(self);
    self->generation = self->generation + 1u;
}

lh_bool_t
lh_ui_web_app_is_websocket(const lh_ui_web_app_t *self, const lh_str_ptr req, lh_usize_t len)
{
    lh_usize_t off = 0u;
    lh_usize_t path_len;

    path_len = lh_ui_web_app_get_ws_path_len(self);
    if (path_len == 0u)
    {
        return lh_bool_false;
    }
    /* The path follows the method and its space; it ends at the `?` of the
       query or at the space before the HTTP version. */
    while (off < len && req[off] != ' ')
    {
        ++off;
    }
    if (off >= len)
    {
        return lh_bool_false;
    }
    ++off;
    if (!lh_str_ptr_starts_with_by_size(req + off, len - off, lh_ui_web_app_get_ws_path(self), path_len,
                                        lh_bool_false))
    {
        return lh_bool_false;
    }
    off += path_len;
    return (off >= len || req[off] == ' ' || req[off] == '?') ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_web_app_handle(lh_ui_web_app_t *self, const lh_str_ptr req, lh_usize_t len,
                      lh_os_net_socket_t *client)
{
    const lh_ui_web_route_t *routes = lh_ui_web_app_get_routes(self);
    const lh_usize_t nroutes = lh_ui_web_app_get_nroutes(self);

    lh_assert_runtime_ref(self);
    for (lh_usize_t i = 0u; i < nroutes; ++i)
    {
        const lh_ui_web_route_t *route = &routes[i];
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
