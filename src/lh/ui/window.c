/**
 * @file window.c
 * @brief Composition of the native window, software canvas and LH view.
 */

#include <lh/ui/window.h>
#include <lh/ui/view/controller.h>
#include <lh/core/attribute/unused.h>

const lh_os_window_t *
lh_ui_window_get_window(const lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->window);
}

lh_os_window_t *
lh_ui_window_get_window_mut(lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->window);
}

const lh_os_render_backend_gdi_context_t *
lh_ui_window_get_screen(const lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->screen);
}

lh_os_render_backend_gdi_context_t *
lh_ui_window_get_screen_mut(lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->screen);
}

const lh_ui_canvas_t *
lh_ui_window_get_canvas(const lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->canvas);
}

lh_ui_canvas_t *
lh_ui_window_get_canvas_mut(lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->canvas);
}

const lh_ui_view_t *
lh_ui_window_get_view(const lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->view);
}

lh_ui_view_t *
lh_ui_window_get_view_mut(lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->view);
}

const lh_os_controller_t *
lh_ui_window_get_controller(const lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->controller);
}

lh_os_controller_t *
lh_ui_window_get_controller_mut(lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->controller);
}

lh_os_window_on_tick_cb
lh_ui_window_get_on_update_fn(const lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_update_fn;
}

lh_void
lh_ui_window_set_on_update_fn(lh_ui_window_t *self, lh_os_window_on_tick_cb on_update)
{
    lh_assert_runtime_ref(self);
    self->on_update_fn = on_update;
}

lh_ptr
lh_ui_window_get_on_update_context(const lh_ui_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_update_context;
}

lh_void
lh_ui_window_set_on_update_context(lh_ui_window_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_update_context = context;
}

lh_void
lh_ui_window_set_on_update(lh_ui_window_t *self, lh_os_window_on_tick_cb on_update, lh_ptr context)
{
    lh_ui_window_set_on_update_fn(self, on_update);
    lh_ui_window_set_on_update_context(self, context);
}

lh_bool_t
lh_ui_window_try_open(lh_ui_window_t *self, struct lh_os_app *app, const char *title, int width,
                      int height)
{
    return lh_os_window_open(app, lh_ui_window_get_window_mut(self), title, width, height);
}

lh_void
lh_ui_window_open(lh_ui_window_t *self, struct lh_os_app *app, const char *title, int width,
                  int height)
{
    lh_runtime_check_ifn(lh_ui_window_try_open(self, app, title, width, height),
                         lh_runtime_error_code_interrupt, "Could not open native UI window");
}

lh_bool_t
lh_ui_window_needs_paint(const lh_ui_window_t *self)
{
    return lh_ui_view_is_has_pending(lh_ui_window_get_view(self)) ||
           lh_ui_canvas_has_damage(lh_ui_window_get_canvas(self));
}

lh_void
lh_ui_window_update(lh_ui_window_t *self)
{
    lh_os_window_on_tick_cb on_update = lh_ui_window_get_on_update_fn(self);
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_update)))
    {
        on_update(lh_ui_window_get_window_mut(self), lh_ui_window_get_on_update_context(self));
    }
    lh_bool_t animated = lh_ui_view_tick(lh_ui_window_get_view_mut(self));
    if (animated || lh_ui_window_needs_paint(self))
    {
        lh_os_window_invalidate(lh_ui_window_get_window_mut(self));
    }
}

lh_void
lh_ui_window_paint(lh_ui_window_t *self)
{
    lh_os_render_backend_gdi_context_set_hwnd(
        lh_ui_window_get_screen_mut(self), lh_os_window_get_handle(lh_ui_window_get_window(self)));
    lh_os_render_backend_gdi_context_set_hdc(
        lh_ui_window_get_screen_mut(self),
        lh_os_window_get_paint_dc(lh_ui_window_get_window(self)));
    lh_ui_rect_t damage;
    LH_ATTRIBUTE_UNUSED(
        lh_ui_view_take_damage(lh_ui_window_get_view_mut(self), lh_addr_of(damage)));
    lh_ui_view_paint(lh_ui_window_get_view_mut(self));
    lh_os_render_backend_gdi_context_set_hdc(lh_ui_window_get_screen_mut(self), lh_null);
}

lh_void
lh_ui_window_on_tick(lh_os_window_t *window, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(window);
    lh_ui_window_update(lh_ptr_rcast(lh_ui_window_t, context));
}

lh_void
lh_ui_window_on_paint(lh_os_window_t *window, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(window);
    lh_ui_window_paint(lh_ptr_rcast(lh_ui_window_t, context));
}

lh_void
lh_ui_window_on_resize(lh_os_window_t *window, int width, int height, lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(width);
    LH_ATTRIBUTE_UNUSED(height);
    lh_ui_window_t *self = lh_ptr_rcast(lh_ui_window_t, context);
    lh_os_controller_cancel(lh_ui_window_get_controller_mut(self));
    lh_os_window_invalidate(window);
}

lh_void
lh_ui_window_init(lh_ui_window_t *self, lh_ui_entity_t *root)
{
    lh_os_window_init(lh_ui_window_get_window_mut(self));
    lh_os_render_backend_gdi_context_init(lh_ui_window_get_screen_mut(self));
    lh_ui_canvas_init(lh_ui_window_get_canvas_mut(self), lh_addr_of(lh_os_render_backend_gdi),
                      lh_ui_window_get_screen_mut(self));
    lh_ui_view_init(lh_ui_window_get_view_mut(self));
    lh_ui_view_set_canvas(lh_ui_window_get_view_mut(self), lh_ui_window_get_canvas_mut(self));
    lh_ui_view_set_root_v(lh_ui_window_get_view_mut(self), root);
    lh_os_controller_init(lh_ui_window_get_controller_mut(self),
                          lh_ui_view_controller_handle_event_is_handled,
                          lh_ui_window_get_view_mut(self), lh_null);
    lh_os_controller_set_cancel_cb(lh_ui_window_get_controller_mut(self),
                                   lh_ui_view_controller_cancel);
    lh_os_window_set_controller(lh_ui_window_get_window_mut(self),
                                lh_ui_window_get_controller_mut(self));
    lh_os_window_set_on_paint(lh_ui_window_get_window_mut(self), lh_ui_window_on_paint, self);
    lh_os_window_set_on_resize(lh_ui_window_get_window_mut(self), lh_ui_window_on_resize, self);
    lh_os_window_set_on_tick(lh_ui_window_get_window_mut(self), lh_ui_window_on_tick, self,
                             LH_UI_VIEW_FRAME_MS);
    lh_ui_window_set_on_update(self, lh_null, lh_null);
}

lh_void
lh_ui_window_deinit(lh_ui_window_t *self)
{
    lh_os_controller_cancel(lh_ui_window_get_controller_mut(self));
    lh_os_window_set_controller(lh_ui_window_get_window_mut(self), lh_null);
    lh_os_window_deinit(lh_ui_window_get_window_mut(self));
    lh_ui_view_set_root_v(lh_ui_window_get_view_mut(self), lh_null);
    lh_ui_view_deinit(lh_ui_window_get_view_mut(self));
    lh_ui_canvas_deinit(lh_ui_window_get_canvas_mut(self));
    lh_os_render_backend_gdi_context_deinit(lh_ui_window_get_screen_mut(self));
    lh_ui_window_set_on_update(self, lh_null, lh_null);
}
