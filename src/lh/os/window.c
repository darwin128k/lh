/**
 * @file window.c
 * @brief Portable ::lh_os_window_t on top of `lh/os/system/window.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/util/return.h>

#include <lh/os/system/window.h>
#include <lh/os/tick.h>
#if LH_LIBRARY_OPTION_UI
#    include <lh/ui/frame/stats.h>
#endif

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
    lh_assert_runtime_ref(self);
    lh_os_window_set_handle(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    lh_os_window_set_app(self, lh_null);
    lh_os_window_set_parent(self, lh_null);
    lh_list_init(lh_os_window_get_children_mut(self));
    lh_list_node_init(lh_os_window_get_link_mut(self));
    lh_os_window_set_modal(self, lh_bool_false);
    lh_os_window_set_closing(self, lh_bool_false);
    lh_os_window_set_frame(self, lh_os_window_frame_system);
    lh_os_window_set_corner(self, 0);
    lh_os_window_set_placement(self, lh_os_window_placement_default);
    lh_os_window_set_maximized(self, lh_bool_false);
    lh_os_window_set_on_zone_fn(self, lh_null);
    lh_os_window_set_on_zone_context(self, lh_null);
    lh_os_window_set_on_resize_fn(self, lh_null);
    lh_os_window_set_on_resize_context(self, lh_null);
    lh_os_window_set_paint_dc(self, lh_null);
    lh_os_window_set_paint_left(self, 0);
    lh_os_window_set_paint_top(self, 0);
    lh_os_window_set_paint_right(self, 0);
    lh_os_window_set_paint_bottom(self, 0);
    lh_os_window_set_on_tick_fn(self, lh_null);
    lh_os_window_set_on_tick_context(self, lh_null);
    lh_os_window_set_tick_ms(self, 0U);
    lh_os_window_set_on_paint_fn(self, lh_null);
    lh_os_window_set_on_paint_context(self, lh_null);
    lh_os_window_set_frame_stats(self, lh_null);
    lh_os_window_set_on_input_fn(self, lh_null);
    lh_os_window_set_on_input_context(self, lh_null);
    lh_os_window_set_controller(self, lh_null);
    lh_os_window_set_controller_armed(self, lh_bool_false);
    lh_os_window_set_pressed_buttons(self, 0U);
    lh_os_window_set_on_close_fn(self, lh_null);
    lh_os_window_set_on_close_context(self, lh_null);
}

lh_bool_t
lh_os_window_open(lh_os_app_t *app, lh_os_window_t *self, const lh_char_t *title, int width,
                  int height)
{
    lh_assert_runtime_ref(app);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(title);
    lh_assert_runtime_if(lh_null_ne(lh_os_window_get_handle(self)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_os_window_get_link_mut(self)),
                         lh_runtime_error_code_invalid_argument);
    lh_os_window_set_handle(self, lh_os_system_window_open(title, width, height, self,
                                                           LH_OS_SYSTEM_WINDOW_HANDLE_INVALID,
                                                           lh_os_window_get_frame(self),
                                                           lh_os_window_get_corner(self),
                                                           lh_os_window_get_placement(self)));
    if (!lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        return lh_bool_false;
    }
    lh_os_window_set_app(self, app);
    lh_os_window_set_parent(self, lh_null);
    lh_os_window_set_modal(self, lh_bool_false);
    /* A period asked for **before** the window existed is armed here, so that where the
       call sits does not decide whether it works. A period of `0` is not a timer. */
    if (lh_os_window_get_tick_ms(self) != 0U)
    {
        lh_os_system_window_set_timer(lh_os_window_get_handle(self),
                                      lh_os_window_get_tick_ms(self));
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
    lh_assert_runtime_if(lh_null_ne(lh_os_window_get_handle(self)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_os_window_get_link_mut(self)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_os_system_window_is_valid(lh_os_window_get_handle(parent)),
                          lh_runtime_error_code_invalid_argument);
    lh_os_window_set_handle(
        self, lh_os_system_window_open(title, width, height, self, lh_os_window_get_handle(parent),
                                       lh_os_window_get_frame(self), lh_os_window_get_corner(self),
                                       lh_os_window_get_placement(self)));
    if (!lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        return lh_bool_false;
    }
    lh_os_window_set_app(self, lh_null);
    lh_os_window_set_parent(self, parent);
    lh_os_window_set_modal(self, lh_bool_true);
    lh_list_push_back(lh_os_window_get_children_mut(parent), lh_os_window_get_link_mut(self));
    lh_os_system_window_set_enabled(lh_os_window_get_handle(parent), lh_bool_false);
    return lh_bool_true;
}

lh_bool_t
lh_os_window_is_open(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_os_system_window_is_valid(lh_os_window_get_handle(self));
}

lh_os_system_window_handle_t
lh_os_window_get_handle(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

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
    lh_assert_runtime_ref(self);
    lh_os_window_set_on_close_fn(self, on_close);
    lh_os_window_set_on_close_context(self, context);
}

lh_void
lh_os_window_set_on_paint(lh_os_window_t *self, lh_os_window_on_paint_cb on_paint, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_os_window_set_on_paint_fn(self, on_paint);
    lh_os_window_set_on_paint_context(self, context);
}

lh_void
lh_os_window_set_frame_stats(lh_os_window_t *self, struct lh_ui_frame_stats *stats)
{
    lh_assert_runtime_ref(self);
    self->frame_stats = stats;
}

struct lh_ui_frame_stats *
lh_os_window_get_frame_stats(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frame_stats;
}

lh_void
lh_os_window_set_on_input(lh_os_window_t *self, lh_os_input_sink_cb on_input, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_os_window_set_on_input_fn(self, on_input);
    lh_os_window_set_on_input_context(self, context);
}

lh_void
lh_os_window_set_controller(lh_os_window_t *self, lh_os_controller_t *controller)
{
    lh_assert_runtime_ref(self);
    self->controller = controller;
}

lh_os_controller_t *
lh_os_window_get_controller(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->controller;
}

lh_u32_t
lh_os_window_get_pressed_buttons(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed_buttons;
}

/* -- Controller timer ------------------------------------------------------ */

lh_bool_t
lh_os_window_get_controller_deadline(lh_os_window_t *self, lh_tick_t *out)
{
    lh_os_controller_t *controller = lh_os_window_get_controller(self);

    lh_return_if(lh_null_eq(controller), lh_bool_false);
    lh_return_if(!lh_os_controller_is_enabled(controller), lh_bool_false);
    lh_return_if(!lh_os_controller_has_flag(controller, lh_os_controller_flags_gestures),
                 lh_bool_false);
    return lh_os_input_recognizer_get_next_deadline(lh_os_controller_get_recognizer(controller),
                                                    out);
}

lh_void
lh_os_window_arm_controller(lh_os_window_t *self, lh_bool_t rearm)
{
    lh_tick_t deadline = 0U;
    lh_tick_t now;
    lh_bool_t waits;

    lh_return_if(lh_os_window_get_tick_ms(self) != 0U ||
                 !lh_os_system_window_is_valid(lh_os_window_get_handle(self)));
    waits = lh_os_window_get_controller_deadline(self, lh_addr_of(deadline));
    if (!waits && lh_os_window_is_controller_armed(self))
    {
        lh_os_window_set_controller_armed(self, lh_bool_false);
        lh_os_system_window_set_timer(lh_os_window_get_handle(self), 0U);
    }
    lh_return_if(!waits || (lh_os_window_is_controller_armed(self) && !rearm));
    now = lh_os_tick_ms();
    /* Due already is due at the next tick the system can make, never a timer of 0 ms:
       that is the call that stops it. */
    lh_os_system_window_set_timer(lh_os_window_get_handle(self),
                                  lh_tick_is_after(deadline, now)
                                      ? lh_type_cast(lh_u32_t, lh_tick_elapsed(now, deadline))
                                      : 1U);
    lh_os_window_set_controller_armed(self, lh_bool_true);
}

lh_void
lh_os_window_update_controller(lh_os_window_t *self)
{
    lh_os_controller_t *controller = lh_os_window_get_controller(self);

    lh_return_if(lh_null_eq(controller));
    (void)lh_os_controller_update(controller, lh_os_tick_ms());
    lh_os_window_arm_controller(self, lh_bool_true);
}

lh_void
lh_os_window_set_on_tick(lh_os_window_t *self, lh_os_window_on_tick_cb on_tick, lh_ptr context,
                         lh_u32_t ms)
{
    lh_assert_runtime_ref(self);
    lh_os_window_set_on_tick_fn(self, on_tick);
    lh_os_window_set_on_tick_context(self, context);
    lh_os_window_set_tick_ms(self, ms);
    /* A timer asked for before the window exists is kept as the **period** and armed on
       open, the same way `frame` and `placement` are read at creation. A timer asked
       for after is armed now. Anything else makes the call's position decide whether it
       works, which is a thing an app has to remember about every call it makes. */
    lh_return_if(!lh_os_system_window_is_valid(lh_os_window_get_handle(self)));
    lh_os_system_window_set_timer(lh_os_window_get_handle(self), ms);
    /* The period, or none: either way the controller's own timer is over, and without
       a period the controller may still have a deadline to be woken for. */
    lh_os_window_set_controller_armed(self, lh_bool_false);
    lh_os_window_arm_controller(self, lh_bool_true);
}

lh_void
lh_os_window_on_native_tick(lh_os_window_t *self)
{
    lh_os_window_on_tick_cb on_tick;
    lh_ptr context;

    lh_assert_runtime_ref(self);
    /* The controller first, so `on_tick` paints what a long press that just came due
       did to the view. */
    lh_os_window_update_controller(self);
    on_tick = lh_os_window_get_on_tick_fn(self);
    /* Read the pair before the call and not after: a tick that stops its own timer -- or
       that stops it and starts it at another period -- would otherwise be talking about
       a slot the callback had already been replaced in. */
    context = lh_os_window_get_on_tick_context(self);
    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, on_tick)));
    on_tick(self, context);
}

lh_ptr
lh_os_window_get_paint_dc(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paint_dc;
}

lh_void
lh_os_window_get_paint_rect(const lh_os_window_t *self, int *left, int *top, int *right,
                            int *bottom)
{
    lh_assert_runtime_ref(self);
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
lh_os_window_invalidate(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        lh_os_system_window_invalidate(lh_os_window_get_handle(self));
    }
}

lh_void
lh_os_window_invalidate_rect(lh_os_window_t *self, int left, int top, int right, int bottom)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        lh_os_system_window_invalidate_rect(lh_os_window_get_handle(self), left, top, right,
                                            bottom);
    }
}

lh_void
lh_os_window_set_frame_v(lh_os_window_t *self, lh_os_window_frame_t frame)
{
    lh_assert_runtime_ref(self);
    /* Only while closed: this is read at creation, and a live window's style and
       region are the OS's to change now, not ours to set behind its back. */
    lh_assert_runtime_if(lh_null_ne(lh_os_window_get_handle(self)),
                         lh_runtime_error_code_invalid_argument);
    lh_os_window_set_frame(self, frame);
}

lh_void
lh_os_window_set_corner_radius(lh_os_window_t *self, int radius)
{
    lh_assert_runtime_ref(self);
    lh_os_window_set_corner(self, radius > 0 ? radius : 0);
    if (lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        lh_os_system_window_set_corner_radius(lh_os_window_get_handle(self),
                                              lh_os_window_get_corner(self));
    }
}

lh_void
lh_os_window_set_placement_v(lh_os_window_t *self, lh_os_window_placement_t placement)
{
    lh_assert_runtime_ref(self);
    /* Only while closed, like ::lh_os_window_set_frame_v: this is read at creation,
       and a window that is already up has a position the user may have moved. */
    lh_assert_runtime_if(lh_null_ne(lh_os_window_get_handle(self)),
                         lh_runtime_error_code_invalid_argument);
    lh_os_window_set_placement(self, placement);
}

lh_os_window_placement_t
lh_os_window_get_placement(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->placement;
}

lh_void
lh_os_window_set_on_zone(lh_os_window_t *self, lh_os_window_on_zone_cb on_zone, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_os_window_set_on_zone_fn(self, on_zone);
    lh_os_window_set_on_zone_context(self, context);
}

lh_os_window_zone_t
lh_os_window_zone_at(lh_os_window_t *self, int x, int y)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, lh_os_window_get_on_zone_fn(self))),
                 lh_os_window_zone_client);
    return lh_os_window_get_on_zone_fn(self)(self, x, y, lh_os_window_get_on_zone_context(self));
}

lh_void
lh_os_window_set_on_resize(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize,
                           lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_os_window_set_on_resize_fn(self, on_resize);
    lh_os_window_set_on_resize_context(self, context);
}

lh_void
lh_os_window_on_native_resize(lh_os_window_t *self, int width, int height, lh_bool_t maximized)
{
    lh_os_window_on_resize_cb on_resize;
    lh_ptr on_resize_context;

    lh_assert_runtime_ref(self);
    lh_os_window_set_maximized(self, maximized);
    /* An empty client is not a size to lay out for, and it arrives from two
       directions: the first size message of a window's life carries nothing before
       creation applies the size it was asked for, and a minimized window really
       does have no client at all. An app that laid out for one would put every
       entity at the origin or off it — a title bar 0 wide is invisible, and the
       next message with a real size is what the app should be laying out for. The
       maximized flag is kept either way: it is a state, not a size. */
    lh_return_if(width <= 0 || height <= 0);
    on_resize = lh_os_window_get_on_resize_fn(self);
    on_resize_context = lh_os_window_get_on_resize_context(self);
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_resize)))
    {
        on_resize(self, width, height, on_resize_context);
    }
}

lh_void
lh_os_window_minimize(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        lh_os_system_window_minimize(lh_os_window_get_handle(self));
    }
}

lh_void
lh_os_window_maximize(lh_os_window_t *self, lh_bool_t maximized)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        lh_os_system_window_set_maximized(lh_os_window_get_handle(self), maximized);
    }
}

lh_bool_t
lh_os_window_is_maximized(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->maximized;
}

lh_bool_t
lh_os_window_get_client_size(const lh_os_window_t *self, int *width, int *height)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(width);
    lh_assert_runtime_ref(height);
    return lh_os_system_window_get_client_size(lh_os_window_get_handle(self), width, height);
}

lh_bool_t
lh_os_window_get_position(const lh_os_window_t *self, int *x, int *y)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(x);
    lh_assert_runtime_ref(y);
    return lh_os_system_window_get_position(lh_os_window_get_handle(self), x, y);
}

lh_void
lh_os_window_on_native_paint(lh_os_window_t *self, lh_ptr paint_dc, int left, int top, int right,
                             int bottom)
{
    lh_os_window_on_paint_cb on_paint;
    lh_ptr on_paint_context;

#if LH_LIBRARY_OPTION_UI
    lh_ui_frame_t frame;
    lh_u64_t started_us = 0U;
#endif

    lh_assert_runtime_ref(self);
    lh_os_window_set_paint_dc(self, paint_dc);
    lh_os_window_set_paint_left(self, left);
    lh_os_window_set_paint_top(self, top);
    lh_os_window_set_paint_right(self, right);
    lh_os_window_set_paint_bottom(self, bottom);
    on_paint = lh_os_window_get_on_paint_fn(self);
    on_paint_context = lh_os_window_get_on_paint_context(self);
#if LH_LIBRARY_OPTION_UI
    if (lh_null_ne(lh_os_window_get_frame_stats(self)))
    {
        /* The request is read before the paint, off the DC the paint is about to draw
           on: what the window system collected from every invalidation, against the
           one rectangle it hands the paint. */
        lh_ui_rect_init(lh_addr_of(frame.drawn), left, top, right - left, bottom - top);
        frame.asked_known = lh_os_system_window_get_paint_region(
            paint_dc, lh_addr_of(frame.asked_px), lh_addr_of(frame.rects));
        started_us = lh_os_tick_us();
    }
#endif
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_paint)))
    {
        on_paint(self, on_paint_context);
    }
#if LH_LIBRARY_OPTION_UI
    if (lh_null_ne(lh_os_window_get_frame_stats(self)))
    {
        frame.us = lh_os_tick_us() - started_us;
        lh_ui_frame_stats_record(lh_os_window_get_frame_stats(self), lh_addr_of(frame));
    }
#endif
    lh_os_window_set_paint_dc(self, lh_null);
    lh_os_window_set_paint_left(self, 0);
    lh_os_window_set_paint_top(self, 0);
    lh_os_window_set_paint_right(self, 0);
    lh_os_window_set_paint_bottom(self, 0);
}

lh_void
lh_os_window_track_buttons(lh_os_window_t *self, lh_os_input_event_cptr event)
{
    lh_return_if(lh_os_input_event_is_pointer(event) && lh_os_input_event_get_pointer(event) != 0U);
    switch (lh_os_input_event_get_kind(event))
    {
    case lh_os_input_kind_pointer_down:
    case lh_os_input_kind_pointer_up:
    case lh_os_input_kind_pointer_move:
        lh_os_window_set_pressed_buttons(self, lh_os_input_event_get_buttons(event));
        break;
    case lh_os_input_kind_pointer_cancel:
    case lh_os_input_kind_focus_out:
        lh_os_window_set_pressed_buttons(self, 0U);
        break;
    default:
        break;
    }
}

lh_void
lh_os_window_feed_controller(lh_os_window_t *self, lh_os_input_event_cptr event)
{
    lh_os_controller_t *controller = lh_os_window_get_controller(self);
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(event);

    lh_return_if(lh_null_eq(controller));
    (void)lh_os_controller_feed(controller, event);
    if (kind == lh_os_input_kind_focus_out)
    {
        lh_os_controller_cancel(controller);
    }
    /* Only a press starts a new wait; anything else can only end one, so the timer that
       already runs is left alone instead of being set again on every move. */
    lh_os_window_arm_controller(self,
                                lh_type_cast(lh_bool_t, kind == lh_os_input_kind_pointer_down));
}

lh_void
lh_os_window_on_native_input(lh_os_window_t *self, lh_os_input_event_cptr event)
{
    lh_assert_runtime_ref(self);
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

lh_void
lh_os_window_on_native_destroy(lh_os_window_t *self)
{
    lh_os_app_t *app;
    lh_os_window_t *parent;
    lh_bool_t modal;
    lh_bool_t was_main;
    lh_os_window_close_reason_t reason;
    lh_os_window_on_close_cb on_close;
    lh_ptr on_close_context;

    lh_assert_runtime_ref(self);
    app = lh_os_window_get_app(self);
    parent = lh_os_window_get_parent(self);
    modal = lh_os_window_is_modal(self);
    was_main = lh_bool_false;
    if (lh_null_ne(app))
    {
        was_main = lh_cast_static(lh_bool_t, self == lh_os_app_get_window(app));
    }
    reason = lh_os_window_is_closing(self) ? lh_os_window_close_reason_api
                                           : lh_os_window_close_reason_os;
    lh_os_window_set_closing(self, lh_bool_false);
    lh_os_window_set_controller_armed(self, lh_bool_false);
    lh_os_window_set_handle(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    if (modal && lh_null_ne(parent) &&
        lh_os_system_window_is_valid(lh_os_window_get_handle(parent)))
    {
        lh_os_system_window_set_enabled(lh_os_window_get_handle(parent), lh_bool_true);
    }
    on_close = lh_os_window_get_on_close_fn(self);
    on_close_context = lh_os_window_get_on_close_context(self);
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
lh_os_window_close(lh_os_window_t *self)
{
    lh_os_window_t *child;

    lh_assert_runtime_ref(self);
    /* The timer stops with the window, and not on the way out of the native one alone:
       a window that is closed and reopened from the same struct must not come back with
       the timer it had before, ticking into an app that has already put the session
       away. The native side kills it on destroy; this makes the **period** zero too, so
       an open that follows is not a window that starts ticking by itself. */
    if (lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        lh_os_system_window_set_timer(lh_os_window_get_handle(self), 0U);
    }
    lh_os_window_set_tick_ms(self, 0U);
    lh_os_window_set_controller_armed(self, lh_bool_false);
    for (;;)
    {
        child = lh_os_window_get_first_child(self);
        if (lh_null_eq(child))
        {
            break;
        }
        lh_os_window_close(child);
    }
    if (lh_os_system_window_is_valid(lh_os_window_get_handle(self)))
    {
        lh_os_window_set_closing(self, lh_bool_true);
        lh_os_system_window_close(lh_os_window_get_handle(self));
    }
    if (lh_null_ne(lh_os_window_get_handle(self)) ||
        lh_list_node_is_linked(lh_os_window_get_link_mut(self)))
    {
        lh_os_window_on_native_destroy(self);
    }
}

lh_void
lh_os_window_deinit(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_window_close(self);
    lh_os_window_set_on_close_fn(self, lh_null);
    lh_os_window_set_on_close_context(self, lh_null);
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
lh_os_window_set_handle(lh_os_window_t *self, lh_os_system_window_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
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

lh_void
lh_os_window_set_placement(lh_os_window_t *self, lh_os_window_placement_t placement)
{
    lh_assert_runtime_ref(self);
    self->placement = placement;
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

lh_void
lh_os_window_set_paint_dc(lh_os_window_t *self, lh_ptr paint_dc)
{
    lh_assert_runtime_ref(self);
    self->paint_dc = paint_dc;
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

lh_os_input_sink_cb
lh_os_window_get_on_input_fn(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_input;
}

lh_void
lh_os_window_set_on_input_fn(lh_os_window_t *self, lh_os_input_sink_cb on_input)
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

lh_void
lh_os_window_set_pressed_buttons(lh_os_window_t *self, lh_u32_t pressed_buttons)
{
    lh_assert_runtime_ref(self);
    self->pressed_buttons = pressed_buttons;
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
