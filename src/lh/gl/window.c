/**
 * @file window.c
 * @brief Composition of the native window, software canvas and LH view.
 */

#include <lh/gl/window.h>
#include <lh/gl/canvas/round.h>
#include <lh/gl/view/sink.h>
#include <lh/os/window/backend/system.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

const lh_os_window_t *
lh_gl_window_get_window(const lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->window);
}

lh_os_window_t *
lh_gl_window_get_window_mut(lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->window);
}

const lh_os_render_backend_gdi_context_t *
lh_gl_window_get_screen(const lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->screen);
}

lh_os_render_backend_gdi_context_t *
lh_gl_window_get_screen_mut(lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->screen);
}

const lh_gl_canvas_t *
lh_gl_window_get_canvas(const lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->canvas);
}

lh_gl_canvas_t *
lh_gl_window_get_canvas_mut(lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->canvas);
}

const lh_gl_view_t *
lh_gl_window_get_view(const lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->view);
}

lh_gl_view_t *
lh_gl_window_get_view_mut(lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->view);
}

const lh_input_recognizer_t *
lh_gl_window_get_recognizer(const lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->recognizer);
}

lh_input_recognizer_t *
lh_gl_window_get_recognizer_mut(lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->recognizer);
}

lh_os_window_on_tick_cb
lh_gl_window_get_on_update_fn(const lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_update_fn;
}

lh_void
lh_gl_window_set_on_update_fn(lh_gl_window_t *self, lh_os_window_on_tick_cb on_update)
{
    lh_assert_runtime_ref(self);
    self->on_update_fn = on_update;
}

lh_ptr
lh_gl_window_get_on_update_context(const lh_gl_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_update_context;
}

lh_void
lh_gl_window_set_on_update_context(lh_gl_window_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_update_context = context;
}

lh_void
lh_gl_window_set_on_update(lh_gl_window_t *self, lh_os_window_on_tick_cb on_update, lh_ptr context)
{
    lh_gl_window_set_on_update_fn(self, on_update);
    lh_gl_window_set_on_update_context(self, context);
}

lh_bool_t
lh_gl_window_try_open(lh_gl_window_t *self, struct lh_os_app *app, const char *title, int width,
                      int height)
{
    return lh_os_window_open(app, lh_gl_window_get_window_mut(self), title, width, height);
}

lh_bool_t
lh_gl_window_needs_paint(const lh_gl_window_t *self)
{
    return lh_gl_view_has_damage(lh_gl_window_get_view(self)) ||
           lh_gl_canvas_has_damage(lh_gl_window_get_canvas(self));
}

lh_bool_t
lh_gl_window_try_get_paint_area(const lh_gl_window_t *self, lh_gl_rect_t *out)
{
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    lh_assert_runtime_ref(out);
    lh_os_window_get_paint_rect(lh_gl_window_get_window(self), lh_addr_of(left), lh_addr_of(top),
                                lh_addr_of(right), lh_addr_of(bottom));
    lh_return_if(right <= left || bottom <= top, lh_bool_false);
    lh_gl_rect_init(out, lh_cast_static(lh_gl_scalar_t, left), lh_cast_static(lh_gl_scalar_t, top),
                    lh_cast_static(lh_gl_scalar_t, right - left),
                    lh_cast_static(lh_gl_scalar_t, bottom - top));
    return lh_bool_true;
}

lh_void
lh_gl_window_invalidate_damage(lh_gl_window_t *self, const lh_gl_rect_t *painting)
{
    lh_os_window_t *window = lh_gl_window_get_window_mut(self);

    if (lh_gl_canvas_has_damage(lh_gl_window_get_canvas(self)))
    {
        lh_gl_canvas_reset_damage(lh_gl_window_get_canvas_mut(self));
        lh_os_window_invalidate(window);
    }
    lh_gl_rects_t damage;

    lh_return_if(!lh_gl_view_take_damage(lh_gl_window_get_view_mut(self), lh_addr_of(damage)));
    for (lh_u32_t i = 0U; i < lh_gl_rects_get_count(lh_addr_of(damage)); ++i)
    {
        const lh_gl_rect_t *rect = lh_gl_rects_get(lh_addr_of(damage), i);

        if (lh_null_ne(painting))
        {
            lh_gl_rect_t joined;

            /* Inside what is being drawn now when adding it changes nothing. */
            lh_gl_rect_union(rect, painting, lh_addr_of(joined));
            if (lh_gl_rect_equals(lh_addr_of(joined), painting))
            {
                continue;
            }
        }
        lh_os_window_invalidate_rect(window, lh_gl_canvas_round_left(rect),
                                     lh_gl_canvas_round_top(rect), lh_gl_canvas_round_right(rect),
                                     lh_gl_canvas_round_bottom(rect));
    }
}

lh_void
lh_gl_window_cancel_input(lh_gl_window_t *self)
{
    lh_input_recognizer_forget(lh_gl_window_get_recognizer_mut(self));
    lh_gl_view_cancel(lh_gl_window_get_view_mut(self));
}

lh_void
lh_gl_window_on_resize(lh_os_window_t *window, int width, int height, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(width);
    LH_ATTRIBUTE_UNUSED(height);
    lh_gl_window_t *self = lh_ptr_rcast(lh_gl_window_t, context);
    lh_gl_window_cancel_input(self);
    lh_os_window_invalidate(window);
}

lh_void
lh_gl_window_open(lh_gl_window_t *self, struct lh_os_app *app, const char *title, int width,
                  int height)
{
    lh_runtime_check_ifn(lh_gl_window_try_open(self, app, title, width, height),
                         lh_runtime_error_code_interrupt, "Could not open native UI window");
}

lh_void
lh_gl_window_update(lh_gl_window_t *self)
{
    lh_os_window_on_tick_cb on_update = lh_gl_window_get_on_update_fn(self);
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_update)))
    {
        on_update(lh_gl_window_get_window_mut(self), lh_gl_window_get_on_update_context(self));
    }
    /* A running animation says it moves, not where: the whole window. */
    if (lh_gl_view_tick(lh_gl_window_get_view_mut(self)))
    {
        lh_os_window_invalidate(lh_gl_window_get_window_mut(self));
    }
    lh_gl_window_invalidate_damage(self, lh_null);
}

lh_void
lh_gl_window_paint(lh_gl_window_t *self)
{
    lh_os_render_backend_gdi_context_set_hwnd(
        lh_gl_window_get_screen_mut(self),
        lh_os_window_backend_system_get_handle(lh_gl_window_get_window(self)));
    lh_os_render_backend_gdi_context_set_hdc(
        lh_gl_window_get_screen_mut(self),
        lh_os_window_get_paint_target(lh_gl_window_get_window(self)));
    lh_gl_rect_t area;
    const lh_bool_t asked = lh_gl_window_try_get_paint_area(self, lh_addr_of(area));

    if (asked)
    {
        lh_gl_window_invalidate_damage(self, lh_addr_of(area));
        lh_gl_view_draw(lh_gl_window_get_view_mut(self), lh_addr_of(area));
    }
    else
    {
        /* The whole frame covers whatever changed: nothing is left to ask for. */
        lh_gl_rects_t damage;

        LH_ATTRIBUTE_UNUSED(
            lh_gl_view_take_damage(lh_gl_window_get_view_mut(self), lh_addr_of(damage)));
        lh_gl_view_paint(lh_gl_window_get_view_mut(self));
    }
    lh_os_render_backend_gdi_context_set_hdc(lh_gl_window_get_screen_mut(self), lh_null);
}

lh_void
lh_gl_window_deinit(lh_gl_window_t *self)
{
    lh_gl_window_cancel_input(self);
    lh_os_window_set_recognizer(lh_gl_window_get_window_mut(self), lh_null);
    lh_os_window_set_on_input(lh_gl_window_get_window_mut(self), lh_null, lh_null);
    lh_os_window_deinit(lh_gl_window_get_window_mut(self));
    lh_gl_view_set_root_v(lh_gl_window_get_view_mut(self), lh_null);
    lh_gl_view_deinit(lh_gl_window_get_view_mut(self));
    lh_gl_canvas_deinit(lh_gl_window_get_canvas_mut(self));
    lh_os_render_backend_gdi_context_deinit(lh_gl_window_get_screen_mut(self));
    lh_gl_window_set_on_update(self, lh_null, lh_null);
}

lh_void
lh_gl_window_on_tick(lh_os_window_t *window, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(window);
    lh_gl_window_update(lh_ptr_rcast(lh_gl_window_t, context));
}

lh_void
lh_gl_window_on_paint(lh_os_window_t *window, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(window);
    lh_gl_window_paint(lh_ptr_rcast(lh_gl_window_t, context));
}

lh_void
lh_gl_window_init(lh_gl_window_t *self, lh_gl_entity_t *root)
{
    lh_os_window_init(lh_gl_window_get_window_mut(self));
    lh_os_render_backend_gdi_context_init(lh_gl_window_get_screen_mut(self));
    lh_gl_canvas_init(lh_gl_window_get_canvas_mut(self), lh_addr_of(lh_os_render_backend_gdi),
                      lh_gl_window_get_screen_mut(self));
    lh_gl_view_init(lh_gl_window_get_view_mut(self));
    lh_gl_view_set_canvas(lh_gl_window_get_view_mut(self), lh_gl_window_get_canvas_mut(self));
    lh_gl_view_set_root_v(lh_gl_window_get_view_mut(self), root);
    lh_input_recognizer_init(lh_gl_window_get_recognizer_mut(self), lh_null);
    lh_os_window_set_on_input(lh_gl_window_get_window_mut(self), lh_gl_view_sink,
                              lh_gl_window_get_view_mut(self));
    lh_os_window_set_recognizer(lh_gl_window_get_window_mut(self),
                                lh_gl_window_get_recognizer_mut(self));
    lh_os_window_set_on_paint(lh_gl_window_get_window_mut(self), lh_gl_window_on_paint, self);
    lh_os_window_set_on_resize(lh_gl_window_get_window_mut(self), lh_gl_window_on_resize, self);
    lh_os_window_set_on_tick(lh_gl_window_get_window_mut(self), lh_gl_window_on_tick, self,
                             LH_GL_VIEW_FRAME_MS);
    lh_gl_window_set_on_update(self, lh_null, lh_null);
}
