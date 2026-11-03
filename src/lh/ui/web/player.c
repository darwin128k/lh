/**
 * @file player.c
 * @brief Implementation of `lh/ui/web/player.h`.
 */

#include <lh/ui/web/player.h>
#include <lh/http/status.h>
#include <lh/http/method.h>
#include <lh/attribute/unused.h>

#include <lh/ui/web/host.h>
#include <lh/ui/web/player/page.h>
#include <lh/ui/view/controller.h>

#include <lh/cast/const.h>
#include <lh/memory.h>
#include <lh/os/input.h>
#include <lh/os/tick.h>
#include <lh/util/return.h>

/** The paths the player's routes answer. */
static const lh_str_ptr m_ui_web_player_paths[LH_UI_WEB_PLAYER_ROUTES] = {
    "/",        "/ui.json",  "/ui.rev", "/ui/click", "/ui/press", "/ui/release",
    "/ui/move", "/ui/wheel", "/ui/key", "/ui/text",  "/ui/size"};
/** @copydoc m_ui_web_player_paths */
static const lh_usize_t m_ui_web_player_path_lens[LH_UI_WEB_PLAYER_ROUTES] = {
    lh_str_lit_size("/"),         lh_str_lit_size("/ui.json"),  lh_str_lit_size("/ui.rev"),
    lh_str_lit_size("/ui/click"), lh_str_lit_size("/ui/press"), lh_str_lit_size("/ui/release"),
    lh_str_lit_size("/ui/move"),  lh_str_lit_size("/ui/wheel"), lh_str_lit_size("/ui/key"),
    lh_str_lit_size("/ui/text"),  lh_str_lit_size("/ui/size")};
/** What answers each of the player's routes. */
static const lh_ui_web_route_fn m_ui_web_player_handlers[LH_UI_WEB_PLAYER_ROUTES] = {
    lh_ui_web_player_serve_page,  lh_ui_web_player_serve_frame, lh_ui_web_player_serve_revision,
    lh_ui_web_player_serve_input, lh_ui_web_player_serve_input, lh_ui_web_player_serve_input,
    lh_ui_web_player_serve_input, lh_ui_web_player_serve_input, lh_ui_web_player_serve_input,
    lh_ui_web_player_serve_input, lh_ui_web_player_serve_input};

lh_ui_web_app_t *
lh_ui_web_player_get_app(lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->app);
}

lh_ui_view_t *
lh_ui_web_player_get_view(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->view;
}

lh_void
lh_ui_web_player_set_view(lh_ui_web_player_t *self, lh_ui_view_t *view)
{
    lh_assert_runtime_ref(self);
    self->view = view;
}

lh_uint_t
lh_ui_web_player_get_width(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_ui_web_player_set_width(lh_ui_web_player_t *self, lh_uint_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_uint_t
lh_ui_web_player_get_height(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_ui_web_player_set_height(lh_ui_web_player_t *self, lh_uint_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_str_ptr
lh_ui_web_player_get_json(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->json;
}

lh_void
lh_ui_web_player_set_json(lh_ui_web_player_t *self, lh_str_ptr json)
{
    lh_assert_runtime_ref(self);
    self->json = json;
}

lh_usize_t
lh_ui_web_player_get_json_cap(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->json_cap;
}

lh_void
lh_ui_web_player_set_json_cap(lh_ui_web_player_t *self, lh_usize_t json_cap)
{
    lh_assert_runtime_ref(self);
    self->json_cap = json_cap;
}

lh_byte_ptr
lh_ui_web_player_get_page(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page;
}

lh_void
lh_ui_web_player_set_page(lh_ui_web_player_t *self, lh_byte_ptr page)
{
    lh_assert_runtime_ref(self);
    self->page = page;
}

lh_usize_t
lh_ui_web_player_get_page_cap(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page_cap;
}

lh_void
lh_ui_web_player_set_page_cap(lh_ui_web_player_t *self, lh_usize_t page_cap)
{
    lh_assert_runtime_ref(self);
    self->page_cap = page_cap;
}

lh_usize_t
lh_ui_web_player_get_page_len(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page_len;
}

lh_void
lh_ui_web_player_set_page_len(lh_ui_web_player_t *self, lh_usize_t page_len)
{
    lh_assert_runtime_ref(self);
    self->page_len = page_len;
}

const lh_ui_web_host_hooks_t *
lh_ui_web_player_get_hooks(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hooks;
}

lh_void
lh_ui_web_player_set_hooks(lh_ui_web_player_t *self, const lh_ui_web_host_hooks_t *hooks)
{
    lh_assert_runtime_ref(self);
    self->hooks = hooks;
}

const lh_ui_web_host_hooks_t *
lh_ui_web_player_get_routed(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->routed);
}

lh_ui_web_host_hooks_t *
lh_ui_web_player_get_routed_mut(lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->routed);
}

const lh_os_controller_t *
lh_ui_web_player_get_controller(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->controller);
}

lh_os_controller_t *
lh_ui_web_player_get_controller_mut(lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->controller);
}

lh_bool_t
lh_ui_web_player_get_owns_controller(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owns_controller;
}

lh_void
lh_ui_web_player_set_owns_controller(lh_ui_web_player_t *self, lh_bool_t owns_controller)
{
    lh_assert_runtime_ref(self);
    self->owns_controller = owns_controller;
}

const lh_ui_canvas_web_t *
lh_ui_web_player_get_web(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->web);
}

lh_ui_canvas_web_t *
lh_ui_web_player_get_web_mut(lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->web);
}

lh_ui_web_route_t *
lh_ui_web_player_get_routes(lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->routes;
}

lh_os_controller_t *
lh_ui_web_player_get_routed_controller(const lh_ui_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_web_host_hooks_get_controller(lh_ui_web_player_get_routed(self));
}

lh_bool_t
lh_ui_web_player_input(lh_ui_web_player_t *self, const lh_os_input_event_t *event)
{
    lh_bool_t changed = lh_bool_false;
    const lh_ui_web_host_hooks_t *hooks = lh_ui_web_player_get_routed(self);

    lh_return_if(lh_ui_web_host_hooks_get_input_fn(hooks) == lh_null &&
                     lh_ui_web_host_hooks_get_controller(hooks) == lh_null,
                 lh_ui_view_input(lh_ui_web_player_get_view(self), event));
    if (lh_ui_web_host_hooks_get_input_fn(hooks) != lh_null)
    {
        lh_ui_web_host_hooks_get_input_fn(hooks)(lh_ui_web_host_hooks_get_ctx(hooks), event);
        changed = lh_bool_true;
    }
    if (lh_ui_web_host_hooks_get_controller(hooks) != lh_null &&
        lh_os_controller_feed(lh_ui_web_host_hooks_get_controller(hooks), event))
    {
        changed = lh_bool_true;
    }
    return changed;
}

lh_bool_t
lh_ui_web_player_resize(lh_ui_web_player_t *self, lh_sint_t w, lh_sint_t h)
{
    if (w < LH_UI_WEB_PLAYER_SIZE_MIN)
    {
        w = LH_UI_WEB_PLAYER_SIZE_MIN;
    }
    if (h < LH_UI_WEB_PLAYER_SIZE_MIN)
    {
        h = LH_UI_WEB_PLAYER_SIZE_MIN;
    }
    if (w > LH_UI_WEB_PLAYER_SIZE_MAX)
    {
        w = LH_UI_WEB_PLAYER_SIZE_MAX;
    }
    if (h > LH_UI_WEB_PLAYER_SIZE_MAX)
    {
        h = LH_UI_WEB_PLAYER_SIZE_MAX;
    }
    if (lh_cast_static(lh_uint_t, w) == lh_ui_web_player_get_width(self) &&
        lh_cast_static(lh_uint_t, h) == lh_ui_web_player_get_height(self))
    {
        return lh_bool_false;
    }
    lh_ui_web_player_set_width(self, lh_cast_static(lh_uint_t, w));
    lh_ui_web_player_set_height(self, lh_cast_static(lh_uint_t, h));
    if (lh_ui_web_host_hooks_get_resize_fn(lh_ui_web_player_get_routed(self)) != lh_null)
    {
        lh_ui_web_host_hooks_get_resize_fn(lh_ui_web_player_get_routed(self))(
            w, h, lh_ui_web_host_hooks_get_ctx(lh_ui_web_player_get_routed(self)));
    }
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_player_serve_page(lh_ptr ctx, const lh_http_request_t *request,
                            const lh_http_header_t *headers, const lh_str_view_t *request_body,
                            lh_os_net_socket_t *client)
{
    lh_ui_web_player_t *self = lh_ptr_rcast(lh_ui_web_player_t, ctx);

    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);
    lh_assert_runtime_ref(self);
    LH_ATTRIBUTE_UNUSED(
        lh_ui_web_host_try_reply(client, request, lh_http_status_ok, "text/html; charset=utf-8",
                                 lh_ptr_rcast(const lh_char_t, lh_ui_web_player_get_page(self)),
                                 lh_ui_web_player_get_page_len(self)));
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_player_serve_revision(lh_ptr ctx, const lh_http_request_t *request,
                                const lh_http_header_t *headers, const lh_str_view_t *request_body,
                                lh_os_net_socket_t *client)
{
    lh_ui_web_player_t *self = lh_ptr_rcast(lh_ui_web_player_t, ctx);
    lh_char_t body[32];
    const lh_usize_t n = lh_str_ptr_format_text(
        body, sizeof(body), "%u\n",
        lh_cast_static(lh_uint_t, lh_ui_web_app_get_generation(lh_ui_web_player_get_app(self))));

    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);
    lh_assert_runtime_ref(self);
    LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(client, request, lh_http_status_ok, body, n));
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_player_draw(lh_ui_web_player_t *self, lh_ui_canvas_web_glyphs_t *glyphs,
                      const lh_str_ptr *bytes, lh_usize_t *len)
{
    lh_ui_canvas_t canvas;

    lh_ui_canvas_web_t *web = lh_ui_web_player_get_web_mut(self);
    if (!lh_ui_canvas_web_open_mem(
            web, lh_ui_web_player_get_json(self), lh_ui_web_player_get_json_cap(self),
            lh_ui_web_player_get_width(self), lh_ui_web_player_get_height(self)))
    {
        return lh_bool_false;
    }
    lh_ui_canvas_web_set_glyphs_v(web, glyphs);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), web);
    lh_ui_view_set_canvas(lh_ui_web_player_get_view(self), lh_addr_of(canvas));
    lh_ui_view_draw(lh_ui_web_player_get_view(self), lh_null);
    lh_ui_view_set_canvas(lh_ui_web_player_get_view(self), lh_null);
    if (!lh_ui_canvas_web_close(web))
    {
        return lh_bool_false;
    }
    if (bytes != lh_null)
    {
        *bytes = lh_ui_canvas_web_bytes(web);
    }
    if (len != lh_null)
    {
        *len = lh_ui_canvas_web_length(web);
    }
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_player_init(lh_ui_web_player_t *self, lh_ui_view_t *view, lh_uint_t width,
                      lh_uint_t height, lh_str_ptr json, lh_usize_t json_cap, lh_byte_ptr page,
                      lh_usize_t page_cap, const lh_ui_web_host_hooks_t *hooks)
{
    lh_ui_web_route_t *routes = lh_ui_web_player_get_routes(self);

    lh_ui_web_player_set_view(self, view);
    lh_ui_web_player_set_width(self, width);
    lh_ui_web_player_set_height(self, height);
    lh_ui_web_player_set_json(self, json);
    lh_ui_web_player_set_json_cap(self, json_cap);
    lh_ui_web_player_set_page(self, page);
    lh_ui_web_player_set_page_cap(self, page_cap);
    lh_ui_web_player_set_page_len(self, 0u);
    lh_ui_web_player_set_hooks(self, hooks);
    lh_ui_web_player_set_owns_controller(self, lh_bool_false);
    lh_ui_web_host_hooks_t *routed = lh_ui_web_player_get_routed_mut(self);
    lh_memory_set(routed, sizeof(lh_ui_web_host_hooks_t), 0);
    if (hooks != lh_null)
    {
        lh_ui_web_host_hooks_set_ctx(routed, lh_ui_web_host_hooks_get_ctx(hooks));
        lh_ui_web_host_hooks_set_tick_fn(routed, lh_ui_web_host_hooks_get_tick_fn(hooks));
        lh_ui_web_host_hooks_set_input_fn(routed, lh_ui_web_host_hooks_get_input_fn(hooks));
        lh_ui_web_host_hooks_set_resize_fn(routed, lh_ui_web_host_hooks_get_resize_fn(hooks));
        lh_ui_web_host_hooks_set_controller(routed, lh_ui_web_host_hooks_get_controller(hooks));
    }
    /* With neither an input hook nor a controller, the view gets every event as
       ::lh_ui_view_input would have, the gestures go along and are ignored by it. */
    if (lh_ui_web_host_hooks_get_input_fn(routed) == lh_null &&
        lh_ui_web_player_get_routed_controller(self) == lh_null)
    {
        lh_os_controller_t *controller = lh_ui_web_player_get_controller_mut(self);

        lh_os_controller_init(controller, lh_ui_view_controller_handle_event_is_handled, view,
                              lh_null);
        lh_os_controller_set_cancel_cb(controller, lh_ui_view_controller_cancel);
        lh_ui_web_player_set_owns_controller(self, lh_bool_true);
        lh_ui_web_host_hooks_set_controller(routed, controller);
    }
    const lh_str_view_t method = lh_http_method_get_name(lh_http_method_get);
    for (lh_usize_t i = 0u; i < LH_UI_WEB_PLAYER_ROUTES; i = lh_math_add_one(i))
    {
        lh_ui_web_route_set_method(routes + i, lh_str_view_get_data(lh_addr_of(method)));
        lh_ui_web_route_set_method_len(routes + i, lh_str_view_get_size(lh_addr_of(method)));
        lh_ui_web_route_set_path(routes + i, m_ui_web_player_paths[i]);
        lh_ui_web_route_set_path_len(routes + i, m_ui_web_player_path_lens[i]);
        lh_ui_web_route_set_handler(routes + i, m_ui_web_player_handlers[i]);
    }
    lh_ui_web_app_init(lh_ui_web_player_get_app(self), routes, LH_UI_WEB_PLAYER_ROUTES);
    lh_ui_web_app_set_ctx(lh_ui_web_player_get_app(self), self);
    lh_ui_web_app_set_ws_path(lh_ui_web_player_get_app(self), "/ws");
    lh_ui_web_app_set_ws_path_len(lh_ui_web_player_get_app(self), lh_str_lit_size("/ws"));
    lh_ui_web_app_set_frame_fn(lh_ui_web_player_get_app(self), lh_ui_web_player_frame);
    lh_ui_web_app_set_message_fn(lh_ui_web_player_get_app(self), lh_ui_web_player_message);
    lh_ui_web_app_set_tick_fn(lh_ui_web_player_get_app(self), lh_ui_web_player_tick);
    lh_ui_web_app_set_generation(lh_ui_web_player_get_app(self), 1u);
    lh_usize_t page_len = 0u;
    const lh_bool_t built = lh_ui_web_player_page_build(
        lh_ui_web_player_get_page(self), lh_ui_web_player_get_page_cap(self), lh_addr_of(page_len));
    lh_ui_web_player_set_page_len(self, page_len);
    return built;
}

lh_bool_t
lh_ui_web_player_pointer(lh_ui_web_player_t *self, lh_os_input_kind_t kind, lh_sint_t x,
                         lh_sint_t y)
{
    lh_os_input_event_t event;
    const lh_bool_t up = lh_type_cast(lh_bool_t, kind == lh_os_input_kind_pointer_up);
    const lh_os_input_button_t button = kind == lh_os_input_kind_pointer_move
                                            ? lh_os_input_button_none
                                            : lh_os_input_button_primary;

    lh_os_input_event_init_pointer(lh_addr_of(event), kind, lh_os_input_device_kind_mouse, 0U,
                                   button, lh_os_input_coord_from_int(x),
                                   lh_os_input_coord_from_int(y), lh_os_tick_ms());
    lh_os_input_event_set_buttons(lh_addr_of(event),
                                  up ? 0U : lh_os_input_button_to_mask(lh_os_input_button_primary));
    return lh_ui_web_player_input(self, lh_addr_of(event));
}

lh_bool_t
lh_ui_web_player_wheel(lh_ui_web_player_t *self, lh_sint_t x, lh_sint_t y, lh_sint_t dx,
                       lh_sint_t dy)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_wheel(
        lh_addr_of(event), lh_os_input_coord_from_int(x), lh_os_input_coord_from_int(y),
        lh_type_cast(lh_s32_t, dx) * LH_OS_INPUT_WHEEL_NOTCH,
        lh_type_cast(lh_s32_t, dy) * LH_OS_INPUT_WHEEL_NOTCH, lh_os_tick_ms());
    return lh_ui_web_player_input(self, lh_addr_of(event));
}

lh_bool_t
lh_ui_web_player_key(lh_ui_web_player_t *self, lh_sint_t k, lh_bool_t pressed)
{
    lh_os_input_event_t event;

    if (k < lh_cast_static(lh_sint_t, lh_os_key_other) ||
        k > lh_cast_static(lh_sint_t, lh_os_key_alt))
    {
        k = lh_cast_static(lh_sint_t, lh_os_key_other);
    }
    lh_os_input_event_init_key(lh_addr_of(event), lh_type_cast(lh_os_key_t, k), pressed,
                               lh_bool_false, 0U, lh_os_tick_ms());
    return lh_ui_web_player_input(self, lh_addr_of(event));
}

lh_bool_t
lh_ui_web_player_text(lh_ui_web_player_t *self, lh_u32_t code)
{
    lh_os_input_event_t event;

    lh_return_if(code == 0U, lh_bool_false);
    lh_os_input_event_init_text(lh_addr_of(event), code, lh_os_tick_ms());
    return lh_ui_web_player_input(self, lh_addr_of(event));
}

lh_bool_t
lh_ui_web_player_serve_frame(lh_ptr ctx, const lh_http_request_t *request,
                             const lh_http_header_t *headers, const lh_str_view_t *request_body,
                             lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);

    lh_ui_web_player_t *self = lh_ptr_rcast(lh_ui_web_player_t, ctx);
    const lh_str_ptr bytes = lh_null;
    lh_usize_t n = 0u;

    lh_assert_runtime_ref(self);
    if (lh_ui_web_host_get_query_uint(request, lh_str_lit("rev=")) ==
        lh_ui_web_app_get_generation(lh_ui_web_player_get_app(self)))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(
            client, request, lh_http_status_not_modified, lh_str_lit("")));
        return lh_bool_true;
    }
    if (!lh_ui_web_player_draw(self, lh_null, lh_addr_of(bytes), lh_addr_of(n)))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(
            client, request, lh_http_status_internal_server_error, lh_str_lit("draw failed\n")));
        return lh_bool_true;
    }
    LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply(client, request, lh_http_status_ok,
                                                 "application/json; charset=utf-8", bytes, n));
    return lh_bool_true;
}

lh_void
lh_ui_web_player_frame(lh_ptr ctx, lh_ui_canvas_web_glyphs_t *glyphs, const lh_str_ptr *bytes,
                       lh_usize_t *len)
{
    lh_ui_web_player_t *self = lh_ptr_rcast(lh_ui_web_player_t, ctx);

    lh_assert_runtime_ref(self);
    LH_ATTRIBUTE_UNUSED(lh_ui_web_player_draw(self, glyphs, bytes, len));
}

lh_void
lh_ui_web_player_tick(lh_ptr ctx)
{
    lh_ui_web_player_t *self = lh_ptr_rcast(lh_ui_web_player_t, ctx);
    lh_ui_rect_t damage;

    /* The controller's timers before the app's tick, as a window does: a long
       press that came due is in the view by the time the app looks. */
    if (lh_ui_web_player_get_routed_controller(self) != lh_null)
    {
        LH_ATTRIBUTE_UNUSED(
            lh_os_controller_update(lh_ui_web_player_get_routed_controller(self), lh_os_tick_ms()));
    }
    if (lh_ui_web_host_hooks_get_tick_fn(lh_ui_web_player_get_routed(self)) != lh_null)
    {
        lh_ui_web_host_hooks_get_tick_fn(lh_ui_web_player_get_routed(self))(
            lh_ui_web_host_hooks_get_ctx(lh_ui_web_player_get_routed(self)));
    }
    if (lh_ui_view_take_damage(lh_ui_web_player_get_view(self), lh_addr_of(damage)))
    {
        lh_ui_web_app_bump(lh_ui_web_player_get_app(self));
    }
}

lh_void
lh_ui_web_player_event_line(lh_ui_web_player_t *self, const lh_str_ptr line, lh_usize_t len)
{
    lh_sint_t x = 0;
    lh_sint_t y = 0;
    lh_sint_t dx = 0;
    lh_sint_t dy = 0;
    lh_sint_t k = 0;
    lh_sint_t pressed = 0;
    lh_uint_t code = 0u;

    if (lh_str_ptr_parse_text(line, len, "p %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_down,
                                                     lh_cast_static(lh_sint_t, x),
                                                     lh_cast_static(lh_sint_t, y)));
    }
    else if (lh_str_ptr_parse_text(line, len, "r %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_up,
                                                     lh_cast_static(lh_sint_t, x),
                                                     lh_cast_static(lh_sint_t, y)));
    }
    else if (lh_str_ptr_parse_text(line, len, "w %d %d %d %d", lh_addr_of(x), lh_addr_of(y),
                                   lh_addr_of(dx), lh_addr_of(dy)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(
            lh_ui_web_player_wheel(self, lh_cast_static(lh_sint_t, x), lh_cast_static(lh_sint_t, y),
                                   lh_cast_static(lh_sint_t, dx), lh_cast_static(lh_sint_t, dy)));
    }
    else if (lh_str_ptr_parse_text(line, len, "m %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* A drag sends one every frame; one that changed nothing is not worth a draw. */
        if (!lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_move,
                                      lh_cast_static(lh_sint_t, x), lh_cast_static(lh_sint_t, y)))
        {
            return;
        }
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d %d", lh_addr_of(k), lh_addr_of(pressed)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_key(self, lh_cast_static(lh_sint_t, k),
                                                 (pressed != 0) ? lh_bool_true : lh_bool_false));
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d", lh_addr_of(k)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_key(self, lh_cast_static(lh_sint_t, k), lh_bool_true));
    }
    else if (lh_str_ptr_parse_text(line, len, "t %u", lh_addr_of(code)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_text(self, lh_cast_static(lh_u32_t, code)));
    }
    else if (lh_str_ptr_parse_text(line, len, "z %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* Always bump: an unchanged size still needs a frame for a new tab, or the
           player keeps the design 980×620 it got before `z` arrived. */
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_resize(self, lh_cast_static(lh_sint_t, x),
                                                    lh_cast_static(lh_sint_t, y)));
    }
    else
    {
        return;
    }
    lh_ui_web_app_bump(lh_ui_web_player_get_app(self));
}

lh_bool_t
lh_ui_web_player_serve_input(lh_ptr ctx, const lh_http_request_t *request,
                             const lh_http_header_t *headers, const lh_str_view_t *request_body,
                             lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);

    lh_ui_web_player_t *self = lh_ptr_rcast(lh_ui_web_player_t, ctx);
    const lh_sint_t x = lh_ui_web_host_get_query_int(request, lh_str_lit("x="));
    const lh_sint_t y = lh_ui_web_host_get_query_int(request, lh_str_lit("y="));

    lh_assert_runtime_ref(self);
    if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/click")))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_down, x, y));
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_up, x, y));
    }
    else if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/press")))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_down, x, y));
    }
    else if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/release")))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_up, x, y));
    }
    else if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/move")))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_pointer(self, lh_os_input_kind_pointer_move, x, y));
    }
    else if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/wheel")))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_wheel(
            self, x, y, lh_ui_web_host_get_query_int(request, lh_str_lit("dx=")),
            lh_ui_web_host_get_query_int(request, lh_str_lit("dy="))));
    }
    else if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/key")))
    {
        /* `p=` is the direction; without it the request is a key going down, as before. */
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_key(
            self, lh_ui_web_host_get_query_int(request, lh_str_lit("k=")),
            lh_ui_web_host_get_query_flag(request, lh_str_lit("p="), lh_bool_true)));
    }
    else if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/text")))
    {
        LH_ATTRIBUTE_UNUSED(lh_ui_web_player_text(
            self,
            lh_cast_static(lh_u32_t, lh_ui_web_host_get_query_uint(request, lh_str_lit("c=")))));
    }
    else if (lh_ui_web_host_is_route(request, lh_str_lit("/ui/size")))
    {
        /* Always handled: an unchanged size is still a valid no-op, not a 404. */
        LH_ATTRIBUTE_UNUSED(
            lh_ui_web_player_resize(self, lh_ui_web_host_get_query_int(request, lh_str_lit("w=")),
                                    lh_ui_web_host_get_query_int(request, lh_str_lit("h="))));
    }
    else
    {
        return lh_bool_false;
    }
    lh_ui_web_app_bump(lh_ui_web_player_get_app(self));
    LH_ATTRIBUTE_UNUSED(
        lh_ui_web_host_try_reply_text(client, request, lh_http_status_ok, lh_str_lit("ok\n")));
    return lh_bool_true;
}

lh_void
lh_ui_web_player_message(lh_ptr ctx, const lh_str_ptr text, lh_usize_t len)
{
    lh_ui_web_player_t *self = lh_ptr_rcast(lh_ui_web_player_t, ctx);

    lh_ui_web_player_event_line(self, text, len);
}
