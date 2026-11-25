/**
 * @file window.c
 * @brief Portable ::lh_os_window_t on top of the contract ::lh_os_window_backend_t.
 */

#include <lh/os/window.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>
#include <lh/os/app.h>

lh_os_app_t *
lh_os_window_get_app(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->app;
}

lh_os_window_t *
lh_os_window_get_parent(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

lh_void
lh_os_window_set_controller(lh_os_window_t *self, lh_controller_t *controller)
{
    lh_assert_runtime_ref(self);
    self->controller = controller;
}

lh_controller_t *
lh_os_window_get_controller(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->controller;
}

const lh_list_node_t *
lh_os_window_get_link(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_list_node_t *
lh_os_window_get_link_mut(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_void
lh_os_window_set_app(lh_os_window_t *self, struct lh_os_app *app)
{
    lh_assert_runtime_ref(self);
    self->app = app;
}

lh_void
lh_os_window_set_parent(lh_os_window_t *self, struct lh_os_window *parent)
{
    lh_assert_runtime_ref(self);
    self->parent = parent;
}

const lh_list_t *
lh_os_window_get_children(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_list_t *
lh_os_window_get_children_mut(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_bool_t
lh_os_window_is_modal(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->modal;
}

lh_void
lh_os_window_set_modal(lh_os_window_t *self, lh_bool_t modal)
{
    lh_assert_runtime_ref(self);
    self->modal = modal;
}

lh_bool_t
lh_os_window_is_closing(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->closing;
}

lh_void
lh_os_window_set_closing(lh_os_window_t *self, lh_bool_t closing)
{
    lh_assert_runtime_ref(self);
    self->closing = closing;
}

int
lh_os_window_get_x(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_void
lh_os_window_set_x(lh_os_window_t *self, int x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

int
lh_os_window_get_y(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_void
lh_os_window_set_y(lh_os_window_t *self, int y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

int
lh_os_window_get_width(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_os_window_set_width(lh_os_window_t *self, int width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

int
lh_os_window_get_height(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_os_window_set_height(lh_os_window_t *self, int height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_os_window_on_resize_cb
lh_os_window_get_on_resize_fn(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_resize;
}

lh_void
lh_os_window_set_on_resize_fn(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize)
{
    lh_assert_runtime_ref(self);
    self->on_resize = on_resize;
}

lh_ptr
lh_os_window_get_on_resize_context(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_resize_context;
}

lh_void
lh_os_window_set_on_resize_context(lh_os_window_t *self, lh_ptr on_resize_context)
{
    lh_assert_runtime_ref(self);
    self->on_resize_context = on_resize_context;
}

lh_os_window_on_paint_cb
lh_os_window_get_on_paint_fn(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_paint;
}

lh_void
lh_os_window_set_on_paint_fn(lh_os_window_t *self, lh_os_window_on_paint_cb on_paint)
{
    lh_assert_runtime_ref(self);
    self->on_paint = on_paint;
}

lh_ptr
lh_os_window_get_on_paint_context(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_paint_context;
}

lh_void
lh_os_window_set_on_paint_context(lh_os_window_t *self, lh_ptr on_paint_context)
{
    lh_assert_runtime_ref(self);
    self->on_paint_context = on_paint_context;
}

lh_input_sink_cb
lh_os_window_get_on_input_fn(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_input;
}

lh_void
lh_os_window_set_on_input_fn(lh_os_window_t *self, lh_input_sink_cb on_input)
{
    lh_assert_runtime_ref(self);
    self->on_input = on_input;
}

lh_ptr
lh_os_window_get_on_input_context(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_input_context;
}

lh_void
lh_os_window_set_on_input_context(lh_os_window_t *self, lh_ptr on_input_context)
{
    lh_assert_runtime_ref(self);
    self->on_input_context = on_input_context;
}

lh_os_window_on_close_cb
lh_os_window_get_on_close_fn(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_close;
}

lh_void
lh_os_window_set_on_close_fn(lh_os_window_t *self, lh_os_window_on_close_cb on_close)
{
    lh_assert_runtime_ref(self);
    self->on_close = on_close;
}

lh_ptr
lh_os_window_get_on_close_context(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_close_context;
}

lh_void
lh_os_window_set_on_close_context(lh_os_window_t *self, lh_ptr on_close_context)
{
    lh_assert_runtime_ref(self);
    self->on_close_context = on_close_context;
}

lh_os_window_on_tick_cb
lh_os_window_get_on_tick_fn(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_tick;
}

lh_void
lh_os_window_set_on_tick_fn(lh_os_window_t *self, lh_os_window_on_tick_cb on_tick)
{
    lh_assert_runtime_ref(self);
    self->on_tick = on_tick;
}

lh_ptr
lh_os_window_get_on_tick_context(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_tick_context;
}

lh_void
lh_os_window_set_on_tick_context(lh_os_window_t *self, lh_ptr on_tick_context)
{
    lh_assert_runtime_ref(self);
    self->on_tick_context = on_tick_context;
}

lh_u32_t
lh_os_window_get_tick_ms(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tick_ms;
}

lh_void
lh_os_window_set_tick_ms(lh_os_window_t *self, lh_u32_t tick_ms)
{
    lh_assert_runtime_ref(self);
    self->tick_ms = tick_ms;
}

lh_bool_t
lh_os_window_is_controller_armed(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->controller_armed;
}

lh_void
lh_os_window_set_controller_armed(lh_os_window_t *self, lh_bool_t controller_armed)
{
    lh_assert_runtime_ref(self);
    self->controller_armed = controller_armed;
}

const lh_os_window_backend_t *
lh_os_window_get_backend(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend;
}

lh_void
lh_os_window_set_backend(lh_os_window_t *self, const lh_os_window_backend_t *backend)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
}

lh_ptr
lh_os_window_get_backend_context(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend_context;
}

lh_void
lh_os_window_set_backend_context(lh_os_window_t *self, lh_ptr backend_context)
{
    lh_assert_runtime_ref(self);
    self->backend_context = backend_context;
}

lh_ptr
lh_os_window_get_native(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->native;
}

lh_void
lh_os_window_set_native(lh_os_window_t *self, lh_ptr native)
{
    lh_assert_runtime_ref(self);
    self->native = native;
}

lh_ptr
lh_os_window_get_paint_target(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paint_target;
}

lh_void
lh_os_window_set_paint_target(lh_os_window_t *self, lh_ptr paint_target)
{
    lh_assert_runtime_ref(self);
    self->paint_target = paint_target;
}

int
lh_os_window_get_paint_left(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paint_left;
}

lh_void
lh_os_window_set_paint_left(lh_os_window_t *self, int paint_left)
{
    lh_assert_runtime_ref(self);
    self->paint_left = paint_left;
}

int
lh_os_window_get_paint_top(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paint_top;
}

lh_void
lh_os_window_set_paint_top(lh_os_window_t *self, int paint_top)
{
    lh_assert_runtime_ref(self);
    self->paint_top = paint_top;
}

int
lh_os_window_get_paint_right(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paint_right;
}

lh_void
lh_os_window_set_paint_right(lh_os_window_t *self, int paint_right)
{
    lh_assert_runtime_ref(self);
    self->paint_right = paint_right;
}

int
lh_os_window_get_paint_bottom(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paint_bottom;
}

lh_void
lh_os_window_set_paint_bottom(lh_os_window_t *self, int paint_bottom)
{
    lh_assert_runtime_ref(self);
    self->paint_bottom = paint_bottom;
}

lh_os_window_frame_t
lh_os_window_get_frame(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frame;
}

lh_void
lh_os_window_set_frame(lh_os_window_t *self, lh_os_window_frame_t frame)
{
    lh_assert_runtime_ref(self);
    self->frame = frame;
}

int
lh_os_window_get_corner(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->corner;
}

lh_void
lh_os_window_set_corner(lh_os_window_t *self, int corner)
{
    lh_assert_runtime_ref(self);
    self->corner = corner;
}

lh_os_window_placement_t
lh_os_window_get_placement(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->placement;
}

lh_void
lh_os_window_set_placement(lh_os_window_t *self, lh_os_window_placement_t placement)
{
    lh_assert_runtime_ref(self);
    self->placement = placement;
}

lh_bool_t
lh_os_window_is_maximized(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->maximized;
}

lh_void
lh_os_window_set_maximized(lh_os_window_t *self, lh_bool_t maximized)
{
    lh_assert_runtime_ref(self);
    self->maximized = maximized;
}

lh_os_window_on_zone_cb
lh_os_window_get_on_zone_fn(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_zone;
}

lh_void
lh_os_window_set_on_zone_fn(lh_os_window_t *self, lh_os_window_on_zone_cb on_zone)
{
    lh_assert_runtime_ref(self);
    self->on_zone = on_zone;
}

lh_ptr
lh_os_window_get_on_zone_context(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_zone_context;
}

lh_void
lh_os_window_set_on_zone_context(lh_os_window_t *self, lh_ptr on_zone_context)
{
    lh_assert_runtime_ref(self);
    self->on_zone_context = on_zone_context;
}

lh_u32_t
lh_os_window_get_pressed_buttons(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed_buttons;
}

lh_void
lh_os_window_set_pressed_buttons(lh_os_window_t *self, lh_u32_t pressed_buttons)
{
    lh_assert_runtime_ref(self);
    self->pressed_buttons = pressed_buttons;
}

lh_void
lh_os_window_unlink(lh_os_window_t *self)
{
    if (lh_list_node_is_linked(lh_os_window_get_link_mut(self)))
    {
        lh_list_node_unlink(lh_os_window_get_link_mut(self));
    }
    lh_os_window_set_app(self, lh_null);
    lh_os_window_set_parent(self, lh_null);
    lh_os_window_set_modal(self, lh_bool_false);
}

lh_void
lh_os_window_init(lh_os_window_t *self)
{
    lh_os_window_set_app(self, lh_null);
    lh_os_window_set_parent(self, lh_null);
    lh_list_init(lh_os_window_get_children_mut(self));
    lh_list_node_init(lh_os_window_get_link_mut(self));
    lh_os_window_set_modal(self, lh_bool_false);
    lh_os_window_set_closing(self, lh_bool_false);
    lh_os_window_set_x(self, 0);
    lh_os_window_set_y(self, 0);
    lh_os_window_set_width(self, 0);
    lh_os_window_set_height(self, 0);
    lh_os_window_set_on_resize_fn(self, lh_null);
    lh_os_window_set_on_resize_context(self, lh_null);
    lh_os_window_set_on_paint_fn(self, lh_null);
    lh_os_window_set_on_paint_context(self, lh_null);
    lh_os_window_set_on_input_fn(self, lh_null);
    lh_os_window_set_on_input_context(self, lh_null);
    lh_os_window_set_on_close_fn(self, lh_null);
    lh_os_window_set_on_close_context(self, lh_null);
    lh_os_window_set_on_tick_fn(self, lh_null);
    lh_os_window_set_on_tick_context(self, lh_null);
    lh_os_window_set_tick_ms(self, 0U);
    lh_os_window_set_controller(self, lh_null);
    lh_os_window_set_controller_armed(self, lh_bool_false);
    lh_os_window_set_backend(self, lh_null);
    lh_os_window_set_backend_context(self, lh_null);
    lh_os_window_set_native(self, lh_null);
    lh_os_window_set_paint_target(self, lh_null);
    lh_os_window_set_paint_left(self, 0);
    lh_os_window_set_paint_top(self, 0);
    lh_os_window_set_paint_right(self, 0);
    lh_os_window_set_paint_bottom(self, 0);
    lh_os_window_set_frame(self, lh_os_window_frame_system);
    lh_os_window_set_corner(self, 0);
    lh_os_window_set_placement(self, lh_os_window_placement_default);
    lh_os_window_set_maximized(self, lh_bool_false);
    lh_os_window_set_on_zone_fn(self, lh_null);
    lh_os_window_set_on_zone_context(self, lh_null);
    lh_os_window_set_pressed_buttons(self, 0U);
}

lh_os_window_t *
lh_os_window_get_first_child(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_list_entry(lh_os_window_t, link, lh_list_get_first(lh_os_window_get_children(self)));
}

lh_os_window_t *
lh_os_window_get_next_child(const lh_os_window_t *self, const lh_os_window_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    return lh_list_entry(
        lh_os_window_t, link,
        lh_list_get_next(lh_os_window_get_children(self), lh_os_window_get_link(child)));
}

lh_void
lh_os_window_set_on_close(lh_os_window_t *self, lh_os_window_on_close_cb on_close, lh_ptr context)
{
    lh_os_window_set_on_close_fn(self, on_close);
    lh_os_window_set_on_close_context(self, context);
}

lh_void
lh_os_window_set_on_paint(lh_os_window_t *self, lh_os_window_on_paint_cb on_paint, lh_ptr context)
{
    lh_os_window_set_on_paint_fn(self, on_paint);
    lh_os_window_set_on_paint_context(self, context);
}

lh_void
lh_os_window_set_on_resize(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize,
                           lh_ptr context)
{
    lh_os_window_set_on_resize_fn(self, on_resize);
    lh_os_window_set_on_resize_context(self, context);
}

lh_void
lh_os_window_set_on_input(lh_os_window_t *self, lh_input_sink_cb on_input, lh_ptr context)
{
    lh_os_window_set_on_input_fn(self, on_input);
    lh_os_window_set_on_input_context(self, context);
}

lh_void
lh_os_window_get_rect(const lh_os_window_t *self, int *x, int *y, int *width, int *height)
{
    lh_assert_runtime_ref(x);
    lh_assert_runtime_ref(y);
    lh_assert_runtime_ref(width);
    lh_assert_runtime_ref(height);
    lh_ptr_deref(x) = lh_os_window_get_x(self);
    lh_ptr_deref(y) = lh_os_window_get_y(self);
    lh_ptr_deref(width) = lh_os_window_get_width(self);
    lh_ptr_deref(height) = lh_os_window_get_height(self);
}

lh_void
lh_os_window_get_size(const lh_os_window_t *self, int *width, int *height)
{
    lh_assert_runtime_ref(width);
    lh_assert_runtime_ref(height);
    lh_ptr_deref(width) = lh_os_window_get_width(self);
    lh_ptr_deref(height) = lh_os_window_get_height(self);
}

lh_void
lh_os_window_set_size(lh_os_window_t *self, int width, int height)
{
    lh_os_window_set_width(self, width);
    lh_os_window_set_height(self, height);
    /* The application is told about the size it just gave, so a layout has one place
       to hear it from: the system's resize and the application's own door end in the
       same call. */
    lh_os_window_on_resize_cb on_resize = lh_os_window_get_on_resize_fn(self);
    lh_ptr on_resize_context = lh_os_window_get_on_resize_context(self);

    if (lh_null_ne(lh_ptr_rcast(lh_void, on_resize)))
    {
        on_resize(self, width, height, on_resize_context);
    }
}

lh_bool_t
lh_os_window_get_controller_deadline(lh_os_window_t *self, lh_tick_t *out)
{
    lh_controller_t *controller = lh_os_window_get_controller(self);

    lh_return_if(lh_null_eq(controller), lh_bool_false);
    lh_return_if(!lh_controller_is_enabled(controller), lh_bool_false);
    lh_return_if(!lh_controller_has_flag(controller, lh_controller_flags_gestures), lh_bool_false);
    return lh_input_recognizer_get_next_deadline(lh_controller_get_recognizer(controller), out);
}

lh_bool_t
lh_os_window_is_open(const lh_os_window_t *self)
{
    return lh_null_ne(lh_os_window_get_native(self));
}

lh_void
lh_os_window_set_on_zone(lh_os_window_t *self, lh_os_window_on_zone_cb on_zone, lh_ptr context)
{
    lh_os_window_set_on_zone_fn(self, on_zone);
    lh_os_window_set_on_zone_context(self, context);
}

lh_os_window_zone_t
lh_os_window_zone_at(lh_os_window_t *self, int x, int y)
{
    lh_os_window_on_zone_cb on_zone = lh_os_window_get_on_zone_fn(self);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, on_zone)), lh_os_window_zone_client);
    return on_zone(self, x, y, lh_os_window_get_on_zone_context(self));
}

lh_void
lh_os_window_get_paint_rect(const lh_os_window_t *self, int *left, int *top, int *right,
                            int *bottom)
{
    lh_assert_runtime_ref(left);
    lh_assert_runtime_ref(top);
    lh_assert_runtime_ref(right);
    lh_assert_runtime_ref(bottom);
    lh_ptr_deref(left) = lh_os_window_get_paint_left(self);
    lh_ptr_deref(top) = lh_os_window_get_paint_top(self);
    lh_ptr_deref(right) = lh_os_window_get_paint_right(self);
    lh_ptr_deref(bottom) = lh_os_window_get_paint_bottom(self);
}

lh_void
lh_os_window_on_native_paint(lh_os_window_t *self, lh_ptr target, int left, int top, int right,
                             int bottom)
{
    lh_os_window_set_paint_target(self, target);
    lh_os_window_set_paint_left(self, left);
    lh_os_window_set_paint_top(self, top);
    lh_os_window_set_paint_right(self, right);
    lh_os_window_set_paint_bottom(self, bottom);
    lh_os_window_on_paint_cb on_paint = lh_os_window_get_on_paint_fn(self);

    if (lh_null_ne(lh_ptr_rcast(lh_void, on_paint)))
    {
        on_paint(self, lh_os_window_get_on_paint_context(self));
    }
    lh_os_window_set_paint_target(self, lh_null);
    lh_os_window_set_paint_left(self, 0);
    lh_os_window_set_paint_top(self, 0);
    lh_os_window_set_paint_right(self, 0);
    lh_os_window_set_paint_bottom(self, 0);
}

lh_void
lh_os_window_track_buttons(lh_os_window_t *self, const lh_input_event_cptr event)
{
    lh_return_if(lh_input_event_is_pointer(event) && lh_input_event_get_pointer(event) != 0U);
    switch (lh_input_event_get_kind(event))
    {
    case lh_input_kind_pointer_down:
    case lh_input_kind_pointer_up:
    case lh_input_kind_pointer_move:
        lh_os_window_set_pressed_buttons(self, lh_input_event_get_buttons(event));
        break;
    case lh_input_kind_pointer_cancel:
    case lh_input_kind_focus_out:
        lh_os_window_set_pressed_buttons(self, 0U);
        break;
    default:
        break;
    }
}

lh_void
lh_os_window_set_rect(lh_os_window_t *self, int x, int y, int width, int height)
{
    lh_os_window_set_x(self, x);
    lh_os_window_set_y(self, y);
    lh_os_window_set_size(self, width, height);
}

lh_bool_t
lh_os_window_open(lh_os_app_t *app, lh_os_window_t *self, const lh_char_t *title, int width,
                  int height)
{
    lh_assert_runtime_ref(app);
    lh_assert_runtime_ref(title);
    lh_assert_runtime_if(lh_os_window_is_open(self), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_os_window_get_link_mut(self)),
                         lh_runtime_error_code_invalid_argument);
    const lh_os_window_backend_t *backend = lh_os_app_get_window_backend(app);

    /* Nothing is installed by default: an application that never said which backend
       its windows live on gets told so here, not a window of a kind it did not ask for. */
    lh_runtime_check_ifn(lh_null_ne(backend) && lh_os_window_backend_is_complete(backend),
                         lh_runtime_error_code_invalid_argument,
                         "lh_os_window_open: the application has no window backend");
    lh_os_window_set_backend(self, backend);
    lh_os_window_set_backend_context(self, lh_os_app_get_window_backend_context(app));
    lh_return_if(!lh_os_window_backend_get_open(backend)(lh_os_window_get_backend_context(self),
                                                         self, title, width, height, lh_null),
                 lh_bool_false);
    lh_os_window_set_app(self, app);
    lh_os_window_set_parent(self, lh_null);
    lh_os_window_set_modal(self, lh_bool_false);
    /* A period asked for **before** the window existed is armed here, so that where the
       call sits does not decide whether it works. A period of `0` is not a timer. */
    lh_os_window_backend_set_timer_fn *set_timer = lh_os_window_backend_get_set_timer(backend);

    if (lh_os_window_get_tick_ms(self) != 0U && lh_null_ne(lh_ptr_rcast(lh_void, set_timer)))
    {
        set_timer(lh_os_window_get_backend_context(self), self, lh_os_window_get_tick_ms(self));
    }
    /* First open becomes main (index 0); later opens append. */
    lh_list_push_back(lh_os_app_get_windows_mut(app), lh_os_window_get_link_mut(self));
    return lh_bool_true;
}

lh_bool_t
lh_os_window_open_modal(lh_os_window_t *parent, lh_os_window_t *self, const lh_char_t *title,
                        int width, int height)
{
    lh_assert_runtime_ref(parent);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(title);
    lh_assert_runtime_if(self == parent, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_os_window_is_open(self), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_os_window_get_link_mut(self)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_os_window_is_open(parent), lh_runtime_error_code_invalid_argument);
    const lh_os_window_backend_t *backend = lh_os_window_get_backend(parent);

    lh_os_window_set_backend(self, backend);
    lh_os_window_set_backend_context(self, lh_os_window_get_backend_context(parent));
    lh_return_if(!lh_os_window_backend_get_open(backend)(lh_os_window_get_backend_context(self),
                                                         self, title, width, height, parent),
                 lh_bool_false);
    lh_os_window_set_app(self, lh_null);
    lh_os_window_set_parent(self, parent);
    lh_os_window_set_modal(self, lh_bool_true);
    lh_list_push_back(lh_os_window_get_children_mut(parent), lh_os_window_get_link_mut(self));
    lh_os_window_backend_set_enabled_fn *set_enabled =
        lh_os_window_backend_get_set_enabled(backend);

    if (lh_null_ne(lh_ptr_rcast(lh_void, set_enabled)))
    {
        set_enabled(lh_os_window_get_backend_context(parent), parent, lh_bool_false);
    }
    return lh_bool_true;
}

lh_void
lh_os_window_invalidate(lh_os_window_t *self)
{
    lh_return_if(!lh_os_window_is_open(self));
    lh_os_window_backend_get_invalidate(lh_os_window_get_backend(self))(
        lh_os_window_get_backend_context(self), self);
}

lh_void
lh_os_window_invalidate_rect(lh_os_window_t *self, int left, int top, int right, int bottom)
{
    lh_return_if(!lh_os_window_is_open(self));
    lh_os_window_backend_get_invalidate_rect(lh_os_window_get_backend(self))(
        lh_os_window_get_backend_context(self), self, left, top, right, bottom);
}

lh_void
lh_os_window_minimize(lh_os_window_t *self)
{
    lh_return_if(!lh_os_window_is_open(self));
    lh_os_window_backend_minimize_fn *minimize =
        lh_os_window_backend_get_minimize(lh_os_window_get_backend(self));

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, minimize)));
    minimize(lh_os_window_get_backend_context(self), self);
}

lh_void
lh_os_window_maximize(lh_os_window_t *self, lh_bool_t maximized)
{
    lh_return_if(!lh_os_window_is_open(self));
    lh_os_window_backend_set_maximized_fn *set_maximized =
        lh_os_window_backend_get_set_maximized(lh_os_window_get_backend(self));

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, set_maximized)));
    set_maximized(lh_os_window_get_backend_context(self), self, maximized);
}

lh_bool_t
lh_os_window_get_client_size(const lh_os_window_t *self, int *width, int *height)
{
    lh_assert_runtime_ref(width);
    lh_assert_runtime_ref(height);
    lh_return_if(!lh_os_window_is_open(self), lh_bool_false);
    lh_os_window_backend_get_client_size_fn *get_client_size =
        lh_os_window_backend_get_get_client_size(lh_os_window_get_backend(self));

    if (lh_null_ne(lh_ptr_rcast(lh_void, get_client_size)))
    {
        return get_client_size(lh_os_window_get_backend_context(self), self, width, height);
    }
    lh_os_window_get_size(self, width, height);
    return lh_bool_true;
}

lh_bool_t
lh_os_window_get_position(const lh_os_window_t *self, int *x, int *y)
{
    lh_assert_runtime_ref(x);
    lh_assert_runtime_ref(y);
    lh_return_if(!lh_os_window_is_open(self), lh_bool_false);
    lh_os_window_backend_get_position_fn *get_position =
        lh_os_window_backend_get_get_position(lh_os_window_get_backend(self));

    if (lh_null_ne(lh_ptr_rcast(lh_void, get_position)))
    {
        return get_position(lh_os_window_get_backend_context(self), self, x, y);
    }
    lh_ptr_deref(x) = lh_os_window_get_x(self);
    lh_ptr_deref(y) = lh_os_window_get_y(self);
    return lh_bool_true;
}

lh_void
lh_os_window_set_frame_v(lh_os_window_t *self, lh_os_window_frame_t frame)
{
    /* Only while closed: this is read at creation, and a live window's style and
       region are the backend's to change now, not ours to set behind its back. */
    lh_assert_runtime_if(lh_os_window_is_open(self), lh_runtime_error_code_invalid_argument);
    lh_os_window_set_frame(self, frame);
}

lh_void
lh_os_window_set_corner_radius(lh_os_window_t *self, int radius)
{
    lh_os_window_set_corner(self, lh_math_max(radius, 0));
    lh_return_if(!lh_os_window_is_open(self));
    lh_os_window_backend_set_corner_radius_fn *set_corner_radius =
        lh_os_window_backend_get_set_corner_radius(lh_os_window_get_backend(self));

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, set_corner_radius)));
    set_corner_radius(lh_os_window_get_backend_context(self), self, lh_os_window_get_corner(self));
}

lh_void
lh_os_window_arm_controller(lh_os_window_t *self, lh_bool_t rearm)
{
    lh_return_if(lh_os_window_get_tick_ms(self) != 0U || !lh_os_window_is_open(self));
    const lh_os_window_backend_t *backend = lh_os_window_get_backend(self);
    lh_os_window_backend_set_timer_fn *set_timer = lh_os_window_backend_get_set_timer(backend);

    /* A backend without a timer leaves the controller's deadlines to whoever calls the
       tick; there is nothing to arm. */
    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, set_timer)));
    lh_ptr context = lh_os_window_get_backend_context(self);
    lh_tick_t deadline = 0U;
    lh_bool_t waits = lh_os_window_get_controller_deadline(self, lh_addr_of(deadline));

    if (!waits && lh_os_window_is_controller_armed(self))
    {
        lh_os_window_set_controller_armed(self, lh_bool_false);
        set_timer(context, self, 0U);
    }
    lh_return_if(!waits || (lh_os_window_is_controller_armed(self) && !rearm));
    lh_tick_t now = lh_os_window_backend_get_now(backend)(context);

    /* Due already is due at the next tick the backend can make, never a timer of 0 ms:
       that is the call that stops it. */
    set_timer(context, self,
              lh_tick_is_after(deadline, now)
                  ? lh_type_cast(lh_u32_t, lh_tick_elapsed(now, deadline))
                  : 1U);
    lh_os_window_set_controller_armed(self, lh_bool_true);
}

lh_void
lh_os_window_on_native_destroy(lh_os_window_t *self)
{
    lh_os_app_t *app = lh_os_window_get_app(self);
    lh_os_window_t *parent = lh_os_window_get_parent(self);
    lh_bool_t modal = lh_os_window_is_modal(self);
    lh_bool_t was_main = lh_bool_false;

    if (lh_null_ne(app))
    {
        was_main = lh_cast_static(lh_bool_t, self == lh_os_app_get_window(app));
    }
    lh_os_window_close_reason_t reason = lh_os_window_is_closing(self)
                                             ? lh_os_window_close_reason_api
                                             : lh_os_window_close_reason_os;

    lh_os_window_set_closing(self, lh_bool_false);
    lh_os_window_set_controller_armed(self, lh_bool_false);
    lh_os_window_set_native(self, lh_null);
    if (modal && lh_null_ne(parent) && lh_os_window_is_open(parent))
    {
        lh_os_window_backend_set_enabled_fn *set_enabled =
            lh_os_window_backend_get_set_enabled(lh_os_window_get_backend(parent));

        if (lh_null_ne(lh_ptr_rcast(lh_void, set_enabled)))
        {
            set_enabled(lh_os_window_get_backend_context(parent), parent, lh_bool_true);
        }
    }
    lh_os_window_on_close_cb on_close = lh_os_window_get_on_close_fn(self);
    lh_ptr on_close_context = lh_os_window_get_on_close_context(self);

    if (lh_null_ne(lh_ptr_rcast(lh_void, on_close)))
    {
        on_close(self, reason, on_close_context);
    }
    lh_os_window_unlink(self);
    if (lh_null_ne(app))
    {
        if (was_main || lh_list_is_empty(lh_os_app_get_windows(app)))
        {
            lh_os_app_quit(app);
        }
    }
}

lh_void
lh_os_window_on_native_resize(lh_os_window_t *self, int width, int height, lh_bool_t maximized)
{
    lh_os_window_set_maximized(self, maximized);
    /* An empty client is not a size to lay out for, and it arrives from two
       directions: the first size message of a window's life carries nothing before
       creation applies the size it was asked for, and a minimized window really
       does have no client at all. An app that laid out for one would put every
       entity at the origin or off it — a title bar 0 wide is invisible, and the
       next message with a real size is what the app should be laying out for. The
       maximized flag is kept either way: it is a state, not a size. */
    lh_return_if(width <= 0 || height <= 0);
    lh_os_window_set_size(self, width, height);
}

lh_void
lh_os_window_set_on_tick(lh_os_window_t *self, lh_os_window_on_tick_cb on_tick, lh_ptr context,
                         lh_u32_t ms)
{
    lh_os_window_set_on_tick_fn(self, on_tick);
    lh_os_window_set_on_tick_context(self, context);
    lh_os_window_set_tick_ms(self, ms);
    /* A timer asked for before the window exists is kept as the **period** and armed on
       open, the same way `frame` and `placement` are read at creation. A timer asked
       for after is armed now. Anything else makes the call's position decide whether it
       works, which is a thing an app has to remember about every call it makes. */
    lh_return_if(!lh_os_window_is_open(self));
    lh_os_window_backend_set_timer_fn *set_timer =
        lh_os_window_backend_get_set_timer(lh_os_window_get_backend(self));

    if (lh_null_ne(lh_ptr_rcast(lh_void, set_timer)))
    {
        set_timer(lh_os_window_get_backend_context(self), self, ms);
    }
    /* The period, or none: either way the controller's own timer is over, and without
       a period the controller may still have a deadline to be woken for. */
    lh_os_window_set_controller_armed(self, lh_bool_false);
    lh_os_window_arm_controller(self, lh_bool_true);
}

lh_void
lh_os_window_close(lh_os_window_t *self)
{
    /* The timer stops with the window, and not on the way out of the backend's one
       alone: a window that is closed and reopened from the same struct must not come
       back with the timer it had before, ticking into an app that has already put the
       session away. The backend stops it on destroy; this makes the **period** zero
       too, so an open that follows is not a window that starts ticking by itself. */
    lh_bool_t open = lh_os_window_is_open(self);

    if (open)
    {
        lh_os_window_backend_set_timer_fn *set_timer =
            lh_os_window_backend_get_set_timer(lh_os_window_get_backend(self));

        if (lh_null_ne(lh_ptr_rcast(lh_void, set_timer)))
        {
            set_timer(lh_os_window_get_backend_context(self), self, 0U);
        }
    }
    lh_os_window_set_tick_ms(self, 0U);
    lh_os_window_set_controller_armed(self, lh_bool_false);
    for (;;)
    {
        lh_os_window_t *child = lh_os_window_get_first_child(self);

        if (lh_null_eq(child))
        {
            break;
        }
        lh_os_window_close(child);
    }
    if (open)
    {
        lh_os_window_set_closing(self, lh_bool_true);
        lh_os_window_backend_get_close(lh_os_window_get_backend(self))(
            lh_os_window_get_backend_context(self), self);
    }
    /* A backend that destroys later (X11 waits for its own message) has not called
       the up-call yet; the window is gone for the application now, so it is told now.
       The backend sees the cleared state and does not call it a second time. */
    if (lh_os_window_is_open(self) || lh_list_node_is_linked(lh_os_window_get_link_mut(self)))
    {
        lh_os_window_on_native_destroy(self);
    }
}

lh_void
lh_os_window_set_placement_v(lh_os_window_t *self, lh_os_window_placement_t placement)
{
    /* Only while closed, like ::lh_os_window_set_frame_v: this is read at creation,
       and a window that is already up has a position the user may have moved. */
    lh_assert_runtime_if(lh_os_window_is_open(self), lh_runtime_error_code_invalid_argument);
    lh_os_window_set_placement(self, placement);
}

lh_void
lh_os_window_update_controller(lh_os_window_t *self)
{
    lh_controller_t *controller = lh_os_window_get_controller(self);

    lh_return_if(lh_null_eq(controller));
    const lh_os_window_backend_t *backend = lh_os_window_get_backend(self);

    /* The time is the backend's: a window that was never opened has no clock to update
       its controller by. */
    lh_return_if(lh_null_eq(backend));
    LH_ATTRIBUTE_UNUSED(lh_controller_update(
        controller, lh_os_window_backend_get_now(backend)(lh_os_window_get_backend_context(self))));
    lh_os_window_arm_controller(self, lh_bool_true);
}

lh_void
lh_os_window_feed_controller(lh_os_window_t *self, const lh_input_event_cptr event)
{
    lh_controller_t *controller = lh_os_window_get_controller(self);
    const lh_input_kind_t kind = lh_input_event_get_kind(event);

    lh_return_if(lh_null_eq(controller));
    LH_ATTRIBUTE_UNUSED(lh_controller_feed(controller, event));
    if (kind == lh_input_kind_focus_out)
    {
        lh_controller_cancel(controller);
    }
    /* Only a press starts a new wait; anything else can only end one, so the timer that
       already runs is left alone instead of being set again on every move. */
    lh_os_window_arm_controller(self, lh_type_cast(lh_bool_t, kind == lh_input_kind_pointer_down));
}

lh_void
lh_os_window_deinit(lh_os_window_t *self)
{
    lh_os_window_close(self);
    lh_os_window_set_on_close_fn(self, lh_null);
    lh_os_window_set_on_close_context(self, lh_null);
}

lh_void
lh_os_window_on_native_tick(lh_os_window_t *self)
{
    /* The controller first, so `on_tick` paints what a long press that just came due
       did to the view. */
    lh_os_window_update_controller(self);
    lh_os_window_on_tick_cb on_tick = lh_os_window_get_on_tick_fn(self);
    /* Read the pair before the call and not after: a tick that stops its own timer -- or
       that stops it and starts it at another period -- would otherwise be talking about
       a slot the callback had already been replaced in. */
    lh_ptr context = lh_os_window_get_on_tick_context(self);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, on_tick)));
    on_tick(self, context);
}

lh_void
lh_os_window_on_native_input(lh_os_window_t *self, const lh_input_event_cptr event)
{
    lh_assert_runtime_ref(event);
    lh_os_window_track_buttons(self, event);
    /* The raw tap first, then the consumer: the sink is the low level and sees the event
       as the backend built it, whatever the controller's handler then does. */
    if (lh_null_ne(lh_ptr_rcast(lh_void, lh_os_window_get_on_input_fn(self))))
    {
        lh_os_window_get_on_input_fn(self)(lh_os_window_get_on_input_context(self), event);
    }
    lh_os_window_feed_controller(self, event);
}
