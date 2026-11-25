/**
 * @file player.c
 * @brief Implementation of `lh/gl/web/player.h`: the player itself: its fields (level 1), its
 * routes and init.
 */

#include <lh/gl/web/player.h>

#include <lh/core/assert.h>
#include <lh/core/http/method.h>
#include <lh/os/web/host.h>
#include <lh/gl/web/player/page.h>
#include <lh/core/memory.h>

/** The paths the player's routes answer. */
static const lh_str_ptr m_ui_web_player_paths[LH_GL_WEB_PLAYER_ROUTES] = {
    "/",        "/ui.json",  "/ui.rev", "/ui/click", "/ui/press", "/ui/release",
    "/ui/move", "/ui/wheel", "/ui/key", "/ui/text",  "/ui/size"};
/** @copydoc m_ui_web_player_paths */
static const lh_usize_t m_ui_web_player_path_lens[LH_GL_WEB_PLAYER_ROUTES] = {
    lh_str_lit_size("/"),         lh_str_lit_size("/ui.json"),  lh_str_lit_size("/ui.rev"),
    lh_str_lit_size("/ui/click"), lh_str_lit_size("/ui/press"), lh_str_lit_size("/ui/release"),
    lh_str_lit_size("/ui/move"),  lh_str_lit_size("/ui/wheel"), lh_str_lit_size("/ui/key"),
    lh_str_lit_size("/ui/text"),  lh_str_lit_size("/ui/size")};
/** What answers each of the player's routes. */
static const lh_os_web_app_route_fn m_ui_web_player_handlers[LH_GL_WEB_PLAYER_ROUTES] = {
    lh_gl_web_player_serve_page,  lh_gl_web_player_serve_frame, lh_gl_web_player_serve_revision,
    lh_gl_web_player_serve_input, lh_gl_web_player_serve_input, lh_gl_web_player_serve_input,
    lh_gl_web_player_serve_input, lh_gl_web_player_serve_input, lh_gl_web_player_serve_input,
    lh_gl_web_player_serve_input, lh_gl_web_player_serve_input};

lh_os_web_app_t *
lh_gl_web_player_get_app(lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->app);
}

lh_gl_view_t *
lh_gl_web_player_get_view(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->view;
}

lh_void
lh_gl_web_player_set_view(lh_gl_web_player_t *self, lh_gl_view_t *view)
{
    lh_assert_runtime_ref(self);
    self->view = view;
}

lh_uint_t
lh_gl_web_player_get_width(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gl_web_player_set_width(lh_gl_web_player_t *self, lh_uint_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_uint_t
lh_gl_web_player_get_height(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_gl_web_player_set_height(lh_gl_web_player_t *self, lh_uint_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_str_ptr
lh_gl_web_player_get_json(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->json;
}

lh_void
lh_gl_web_player_set_json(lh_gl_web_player_t *self, lh_str_ptr json)
{
    lh_assert_runtime_ref(self);
    self->json = json;
}

lh_usize_t
lh_gl_web_player_get_json_cap(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->json_cap;
}

lh_void
lh_gl_web_player_set_json_cap(lh_gl_web_player_t *self, lh_usize_t json_cap)
{
    lh_assert_runtime_ref(self);
    self->json_cap = json_cap;
}

lh_byte_ptr
lh_gl_web_player_get_page(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page;
}

lh_void
lh_gl_web_player_set_page(lh_gl_web_player_t *self, lh_byte_ptr page)
{
    lh_assert_runtime_ref(self);
    self->page = page;
}

lh_usize_t
lh_gl_web_player_get_page_cap(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page_cap;
}

lh_void
lh_gl_web_player_set_page_cap(lh_gl_web_player_t *self, lh_usize_t page_cap)
{
    lh_assert_runtime_ref(self);
    self->page_cap = page_cap;
}

lh_usize_t
lh_gl_web_player_get_page_len(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page_len;
}

lh_void
lh_gl_web_player_set_page_len(lh_gl_web_player_t *self, lh_usize_t page_len)
{
    lh_assert_runtime_ref(self);
    self->page_len = page_len;
}

const lh_os_web_host_hooks_t *
lh_gl_web_player_get_hooks(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hooks;
}

lh_void
lh_gl_web_player_set_hooks(lh_gl_web_player_t *self, const lh_os_web_host_hooks_t *hooks)
{
    lh_assert_runtime_ref(self);
    self->hooks = hooks;
}

const lh_os_web_host_hooks_t *
lh_gl_web_player_get_routed(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->routed);
}

lh_os_web_host_hooks_t *
lh_gl_web_player_get_routed_mut(lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->routed);
}

const lh_input_recognizer_t *
lh_gl_web_player_get_recognizer(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->recognizer);
}

lh_input_recognizer_t *
lh_gl_web_player_get_recognizer_mut(lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->recognizer);
}

const lh_gl_canvas_web_t *
lh_gl_web_player_get_web(const lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->web);
}

lh_gl_canvas_web_t *
lh_gl_web_player_get_web_mut(lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->web);
}

lh_gl_canvas_web_glyphs_t *
lh_gl_web_player_get_glyphs_mut(lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->glyphs);
}

lh_os_web_app_route_t *
lh_gl_web_player_get_routes(lh_gl_web_player_t *self)
{
    lh_assert_runtime_ref(self);
    return self->routes;
}

lh_bool_t
lh_gl_web_player_init(lh_gl_web_player_t *self, lh_gl_view_t *view, lh_uint_t width,
                      lh_uint_t height, lh_str_ptr json, lh_usize_t json_cap, lh_byte_ptr page,
                      lh_usize_t page_cap, const lh_os_web_host_hooks_t *hooks)
{
    lh_os_web_app_route_t *routes = lh_gl_web_player_get_routes(self);

    lh_gl_web_player_set_view(self, view);
    lh_gl_web_player_set_width(self, width);
    lh_gl_web_player_set_height(self, height);
    lh_gl_web_player_set_json(self, json);
    lh_gl_web_player_set_json_cap(self, json_cap);
    lh_gl_web_player_set_page(self, page);
    lh_gl_web_player_set_page_cap(self, page_cap);
    lh_gl_web_player_set_page_len(self, 0u);
    lh_gl_web_player_set_hooks(self, hooks);
    lh_gl_canvas_web_glyphs_reset(lh_gl_web_player_get_glyphs_mut(self));
    lh_os_web_host_hooks_t *routed = lh_gl_web_player_get_routed_mut(self);
    lh_memory_set(routed, sizeof(lh_os_web_host_hooks_t), 0);
    if (hooks != lh_null)
    {
        lh_os_web_host_hooks_set_ctx(routed, lh_os_web_host_hooks_get_ctx(hooks));
        lh_os_web_host_hooks_set_tick_fn(routed, lh_os_web_host_hooks_get_tick_fn(hooks));
        lh_os_web_host_hooks_set_input_fn(routed, lh_os_web_host_hooks_get_input_fn(hooks));
        lh_os_web_host_hooks_set_resize_fn(routed, lh_os_web_host_hooks_get_resize_fn(hooks));
        lh_os_web_host_hooks_set_recognizer(routed, lh_os_web_host_hooks_get_recognizer(hooks));
    }
    /* With neither an input hook nor a recognizer, the view gets every event as
       ::lh_gl_view_input would have, behind a recognizer of the player's own: the
       gestures go along and are ignored by it. */
    lh_input_recognizer_init(lh_gl_web_player_get_recognizer_mut(self), lh_null);
    if (lh_null_eq(lh_ptr_rcast(lh_void, lh_os_web_host_hooks_get_input_fn(routed))) &&
        lh_null_eq(lh_gl_web_player_get_routed_recognizer(self)))
    {
        lh_os_web_host_hooks_set_recognizer(routed, lh_gl_web_player_get_recognizer_mut(self));
    }
    const lh_str_view_t method = lh_http_method_get_name(lh_http_method_get);
    for (lh_usize_t i = 0u; i < LH_GL_WEB_PLAYER_ROUTES; i = lh_math_add_one(i))
    {
        lh_os_web_app_route_set_method(routes + i, lh_str_view_get_data(lh_addr_of(method)));
        lh_os_web_app_route_set_method_len(routes + i, lh_str_view_get_size(lh_addr_of(method)));
        lh_os_web_app_route_set_path(routes + i, m_ui_web_player_paths[i]);
        lh_os_web_app_route_set_path_len(routes + i, m_ui_web_player_path_lens[i]);
        lh_os_web_app_route_set_handler(routes + i, m_ui_web_player_handlers[i]);
    }
    lh_os_web_app_init(lh_gl_web_player_get_app(self), routes, LH_GL_WEB_PLAYER_ROUTES);
    lh_os_web_app_set_ctx(lh_gl_web_player_get_app(self), self);
    lh_os_web_app_set_ws_path(lh_gl_web_player_get_app(self), "/ws");
    lh_os_web_app_set_ws_path_len(lh_gl_web_player_get_app(self), lh_str_lit_size("/ws"));
    lh_os_web_app_set_frame_fn(lh_gl_web_player_get_app(self), lh_gl_web_player_frame);
    lh_os_web_app_set_message_fn(lh_gl_web_player_get_app(self), lh_gl_web_player_message);
    lh_os_web_app_set_tick_fn(lh_gl_web_player_get_app(self), lh_gl_web_player_tick);
    lh_os_web_app_set_generation(lh_gl_web_player_get_app(self), 1u);
    lh_usize_t page_len = 0u;
    const lh_bool_t built = lh_gl_web_player_page_build(
        lh_gl_web_player_get_page(self), lh_gl_web_player_get_page_cap(self), lh_addr_of(page_len));
    lh_gl_web_player_set_page_len(self, page_len);
    return built;
}
