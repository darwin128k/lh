/**
 * @file frame.c
 * @brief Implementation of `lh/gl/web/player.h`: the frame: the size, the draw, the bytes of one
 * frame, the tick.
 */

#include <lh/gl/web/player.h>

#include <lh/core/attribute/unused.h>
#include <lh/os/web/host.h>
#include <lh/os/tick.h>

lh_bool_t
lh_gl_web_player_resize(lh_gl_web_player_t *self, lh_sint_t w, lh_sint_t h)
{
    if (w < LH_GL_WEB_PLAYER_SIZE_MIN)
    {
        w = LH_GL_WEB_PLAYER_SIZE_MIN;
    }
    if (h < LH_GL_WEB_PLAYER_SIZE_MIN)
    {
        h = LH_GL_WEB_PLAYER_SIZE_MIN;
    }
    if (w > LH_GL_WEB_PLAYER_SIZE_MAX)
    {
        w = LH_GL_WEB_PLAYER_SIZE_MAX;
    }
    if (h > LH_GL_WEB_PLAYER_SIZE_MAX)
    {
        h = LH_GL_WEB_PLAYER_SIZE_MAX;
    }
    if (lh_cast_static(lh_uint_t, w) == lh_gl_web_player_get_width(self) &&
        lh_cast_static(lh_uint_t, h) == lh_gl_web_player_get_height(self))
    {
        return lh_bool_false;
    }
    lh_gl_web_player_set_width(self, lh_cast_static(lh_uint_t, w));
    lh_gl_web_player_set_height(self, lh_cast_static(lh_uint_t, h));
    if (lh_os_web_host_hooks_get_resize_fn(lh_gl_web_player_get_routed(self)) != lh_null)
    {
        lh_os_web_host_hooks_get_resize_fn(lh_gl_web_player_get_routed(self))(
            w, h, lh_os_web_host_hooks_get_ctx(lh_gl_web_player_get_routed(self)));
    }
    return lh_bool_true;
}

lh_bool_t
lh_gl_web_player_draw(lh_gl_web_player_t *self, lh_gl_canvas_web_glyphs_t *glyphs,
                      const lh_str_ptr *bytes, lh_usize_t *len)
{
    lh_gl_canvas_web_t *web = lh_gl_web_player_get_web_mut(self);
    if (!lh_gl_canvas_web_open_mem(
            web, lh_gl_web_player_get_json(self), lh_gl_web_player_get_json_cap(self),
            lh_gl_web_player_get_width(self), lh_gl_web_player_get_height(self)))
    {
        return lh_bool_false;
    }
    lh_gl_canvas_web_set_glyphs_v(web, glyphs);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_gl_canvas_backend_web), web);
    lh_gl_view_set_canvas(lh_gl_web_player_get_view(self), lh_addr_of(canvas));
    lh_gl_view_draw(lh_gl_web_player_get_view(self), lh_null);
    lh_gl_view_set_canvas(lh_gl_web_player_get_view(self), lh_null);
    if (!lh_gl_canvas_web_close(web))
    {
        return lh_bool_false;
    }
    if (bytes != lh_null)
    {
        *bytes = lh_gl_canvas_web_bytes(web);
    }
    if (len != lh_null)
    {
        *len = lh_gl_canvas_web_length(web);
    }
    return lh_bool_true;
}

lh_void
lh_gl_web_player_tick(lh_ptr ctx)
{
    lh_gl_web_player_t *self = lh_ptr_rcast(lh_gl_web_player_t, ctx);

    /* The recognizer's timers before the app's tick, as a window does: a long
       press that came due is in the view by the time the app looks. */
    lh_input_recognizer_t *recognizer = lh_gl_web_player_get_routed_recognizer(self);

    if (lh_null_ne(recognizer))
    {
        lh_input_recognizer_update(recognizer, lh_os_tick_ms(), lh_gl_web_player_get_sink(self),
                                   lh_gl_web_player_get_sink_context(self));
    }
    if (lh_os_web_host_hooks_get_tick_fn(lh_gl_web_player_get_routed(self)) != lh_null)
    {
        lh_os_web_host_hooks_get_tick_fn(lh_gl_web_player_get_routed(self))(
            lh_os_web_host_hooks_get_ctx(lh_gl_web_player_get_routed(self)));
    }
    lh_gl_rects_t damage;
    if (lh_gl_view_take_damage(lh_gl_web_player_get_view(self), lh_addr_of(damage)))
    {
        lh_os_web_app_bump(lh_gl_web_player_get_app(self));
    }
}

lh_void
lh_gl_web_player_frame(lh_ptr ctx, lh_bool_t restart, const lh_str_ptr *bytes, lh_usize_t *len)
{
    lh_gl_web_player_t *self = lh_ptr_rcast(lh_gl_web_player_t, ctx);
    lh_gl_canvas_web_glyphs_t *glyphs = lh_gl_web_player_get_glyphs_mut(self);

    if (restart)
    {
        lh_gl_canvas_web_glyphs_reset(glyphs);
    }
    LH_ATTRIBUTE_UNUSED(lh_gl_web_player_draw(self, glyphs, bytes, len));
}
