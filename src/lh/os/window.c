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

/** Take @p self out of its app's window list and forget its app, parent and modal state. */
static lh_void
lh_os_window_unlink(lh_os_window_t *self)
{
    if (lh_list_node_is_linked(lh_addr_of(self->link)))
    {
        lh_list_node_unlink(lh_addr_of(self->link));
    }
    self->app = lh_null;
    self->parent = lh_null;
    self->modal = lh_bool_false;
}

lh_void
lh_os_window_init(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    self->app = lh_null;
    self->parent = lh_null;
    lh_list_init(lh_addr_of(self->children));
    lh_list_node_init(lh_addr_of(self->link));
    self->modal = lh_bool_false;
    self->closing = lh_bool_false;
    self->frame = lh_os_window_frame_system;
    self->corner = 0;
    self->placement = lh_os_window_placement_default;
    self->maximized = lh_bool_false;
    self->on_zone = lh_null;
    self->on_zone_context = lh_null;
    self->on_resize = lh_null;
    self->on_resize_context = lh_null;
    self->paint_dc = lh_null;
    self->paint_left = 0;
    self->paint_top = 0;
    self->paint_right = 0;
    self->paint_bottom = 0;
    self->on_tick = lh_null;
    self->on_tick_context = lh_null;
    self->tick_ms = 0U;
    self->on_paint = lh_null;
    self->on_paint_context = lh_null;
    self->frame_stats = lh_null;
    self->on_input = lh_null;
    self->on_input_context = lh_null;
    self->controller = lh_null;
    self->controller_armed = lh_bool_false;
    self->pressed_buttons = 0U;
    self->on_close = lh_null;
    self->on_close_context = lh_null;
}

lh_bool_t
lh_os_window_open(lh_os_app_t *app, lh_os_window_t *self, const lh_char_t *title, int width,
                  int height)
{
    lh_assert_runtime_ref(app);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(title);
    lh_assert_runtime_if(lh_null_ne(self->handle), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_addr_of(self->link)),
                         lh_runtime_error_code_invalid_argument);
    self->handle =
        lh_os_system_window_open(title, width, height, self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID,
                                 self->frame, self->corner, self->placement);
    if (!lh_os_system_window_is_valid(self->handle))
    {
        return lh_bool_false;
    }
    self->app = app;
    self->parent = lh_null;
    self->modal = lh_bool_false;
    /* A period asked for **before** the window existed is armed here, so that where the
       call sits does not decide whether it works. A period of `0` is not a timer. */
    if (self->tick_ms != 0U)
    {
        lh_os_system_window_set_timer(self->handle, self->tick_ms);
    }
    /* First open becomes main (index 0); later opens append. */
    lh_list_push_back(lh_addr_of(app->windows), lh_addr_of(self->link));
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
    lh_assert_runtime_if(lh_null_ne(self->handle), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_addr_of(self->link)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_os_system_window_is_valid(parent->handle),
                          lh_runtime_error_code_invalid_argument);
    self->handle = lh_os_system_window_open(title, width, height, self, parent->handle, self->frame,
                                            self->corner, self->placement);
    if (!lh_os_system_window_is_valid(self->handle))
    {
        return lh_bool_false;
    }
    self->app = lh_null;
    self->parent = parent;
    self->modal = lh_bool_true;
    lh_list_push_back(lh_addr_of(parent->children), lh_addr_of(self->link));
    lh_os_system_window_set_enabled(parent->handle, lh_bool_false);
    return lh_bool_true;
}

lh_bool_t
lh_os_window_is_open(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_os_system_window_is_valid(self->handle);
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
    return lh_list_entry(lh_os_window_t, link, lh_list_get_first(lh_addr_of(self->children)));
}

lh_os_window_t *
lh_os_window_get_next_child(const lh_os_window_t *self, const lh_os_window_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    return lh_list_entry(lh_os_window_t, link,
                         lh_list_get_next(lh_addr_of(self->children), lh_addr_of(child->link)));
}

lh_void
lh_os_window_set_on_close(lh_os_window_t *self, lh_os_window_on_close_cb on_close, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_close = on_close;
    self->on_close_context = context;
}

lh_void
lh_os_window_set_on_paint(lh_os_window_t *self, lh_os_window_on_paint_cb on_paint, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_paint = on_paint;
    self->on_paint_context = context;
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
    self->on_input = on_input;
    self->on_input_context = context;
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

/** When the recognizer of @p self's controller next has something to send, into @p out;
   false, @p out untouched, with no controller, no gestures or nothing waiting on time. */
static lh_bool_t
lh_os_window_get_controller_deadline(lh_os_window_t *self, lh_tick_t *out)
{
    lh_os_controller_t *controller = lh_os_window_get_controller(self);

    lh_return_if(lh_null_eq(controller), lh_bool_false);
    lh_return_if(!lh_os_controller_has_flag(controller, lh_os_controller_flags_gestures),
                 lh_bool_false);
    return lh_os_input_recognizer_get_next_deadline(lh_os_controller_get_recognizer(controller),
                                                    out);
}

/** The window timer of @p self for its controller, when no `on_tick` period runs it: armed
   for the recognizer's next deadline -- again when @p rearm, else only when it is not
   running yet -- and stopped once nothing waits on time. */
static lh_void
lh_os_window_arm_controller(lh_os_window_t *self, lh_bool_t rearm)
{
    lh_tick_t deadline = 0U;
    lh_tick_t now;
    lh_bool_t waits;

    lh_return_if(self->tick_ms != 0U || !lh_os_system_window_is_valid(self->handle));
    waits = lh_os_window_get_controller_deadline(self, lh_addr_of(deadline));
    if (!waits && self->controller_armed)
    {
        self->controller_armed = lh_bool_false;
        lh_os_system_window_set_timer(self->handle, 0U);
    }
    lh_return_if(!waits || (self->controller_armed && !rearm));
    now = lh_os_tick_ms();
    /* Due already is due at the next tick the system can make, never a timer of 0 ms:
       that is the call that stops it. */
    lh_os_system_window_set_timer(self->handle,
                                  lh_tick_is_after(deadline, now)
                                      ? lh_type_cast(lh_u32_t, lh_tick_elapsed(now, deadline))
                                      : 1U);
    self->controller_armed = lh_bool_true;
}

/** The controller's timers at @p self's tick: what is due by now, then the timer for the
   next deadline. */
static lh_void
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
    self->on_tick = on_tick;
    self->on_tick_context = context;
    self->tick_ms = ms;
    /* A timer asked for before the window exists is kept as the **period** and armed on
       open, the same way `frame` and `placement` are read at creation. A timer asked
       for after is armed now. Anything else makes the call's position decide whether it
       works, which is a thing an app has to remember about every call it makes. */
    lh_return_if(!lh_os_system_window_is_valid(self->handle));
    lh_os_system_window_set_timer(self->handle, ms);
    /* The period, or none: either way the controller's own timer is over, and without
       a period the controller may still have a deadline to be woken for. */
    self->controller_armed = lh_bool_false;
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
    on_tick = self->on_tick;
    /* Read the pair before the call and not after: a tick that stops its own timer -- or
       that stops it and starts it at another period -- would otherwise be talking about
       a slot the callback had already been replaced in. */
    context = self->on_tick_context;
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
    lh_ptr_deref(left) = self->paint_left;
    lh_ptr_deref(top) = self->paint_top;
    lh_ptr_deref(right) = self->paint_right;
    lh_ptr_deref(bottom) = self->paint_bottom;
}

lh_void
lh_os_window_invalidate(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_invalidate(self->handle);
    }
}

lh_void
lh_os_window_invalidate_rect(lh_os_window_t *self, int left, int top, int right, int bottom)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_invalidate_rect(self->handle, left, top, right, bottom);
    }
}

lh_void
lh_os_window_set_frame(lh_os_window_t *self, lh_os_window_frame_t frame)
{
    lh_assert_runtime_ref(self);
    /* Only while closed: this is read at creation, and a live window's style and
       region are the OS's to change now, not ours to set behind its back. */
    lh_assert_runtime_if(lh_null_ne(self->handle), lh_runtime_error_code_invalid_argument);
    self->frame = frame;
}

lh_void
lh_os_window_set_corner_radius(lh_os_window_t *self, int radius)
{
    lh_assert_runtime_ref(self);
    self->corner = radius > 0 ? radius : 0;
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_set_corner_radius(self->handle, self->corner);
    }
}

lh_void
lh_os_window_set_placement(lh_os_window_t *self, lh_os_window_placement_t placement)
{
    lh_assert_runtime_ref(self);
    /* Only while closed, like ::lh_os_window_set_frame: this is read at creation,
       and a window that is already up has a position the user may have moved. */
    lh_assert_runtime_if(lh_null_ne(self->handle), lh_runtime_error_code_invalid_argument);
    self->placement = placement;
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
    self->on_zone = on_zone;
    self->on_zone_context = context;
}

lh_os_window_zone_t
lh_os_window_zone_at(lh_os_window_t *self, int x, int y)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, self->on_zone)), lh_os_window_zone_client);
    return self->on_zone(self, x, y, self->on_zone_context);
}

lh_void
lh_os_window_set_on_resize(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize,
                           lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_resize = on_resize;
    self->on_resize_context = context;
}

lh_void
lh_os_window_on_native_resize(lh_os_window_t *self, int width, int height, lh_bool_t maximized)
{
    lh_os_window_on_resize_cb on_resize;
    lh_ptr on_resize_context;

    lh_assert_runtime_ref(self);
    self->maximized = maximized;
    /* An empty client is not a size to lay out for, and it arrives from two
       directions: the first size message of a window's life carries nothing before
       creation applies the size it was asked for, and a minimized window really
       does have no client at all. An app that laid out for one would put every
       entity at the origin or off it — a title bar 0 wide is invisible, and the
       next message with a real size is what the app should be laying out for. The
       maximized flag is kept either way: it is a state, not a size. */
    lh_return_if(width <= 0 || height <= 0);
    on_resize = self->on_resize;
    on_resize_context = self->on_resize_context;
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_resize)))
    {
        on_resize(self, width, height, on_resize_context);
    }
}

lh_void
lh_os_window_minimize(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_minimize(self->handle);
    }
}

lh_void
lh_os_window_set_maximized(lh_os_window_t *self, lh_bool_t maximized)
{
    lh_assert_runtime_ref(self);
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_set_maximized(self->handle, maximized);
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
    return lh_os_system_window_get_client_size(self->handle, width, height);
}

lh_bool_t
lh_os_window_get_position(const lh_os_window_t *self, int *x, int *y)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(x);
    lh_assert_runtime_ref(y);
    return lh_os_system_window_get_position(self->handle, x, y);
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
    self->paint_dc = paint_dc;
    self->paint_left = left;
    self->paint_top = top;
    self->paint_right = right;
    self->paint_bottom = bottom;
    on_paint = self->on_paint;
    on_paint_context = self->on_paint_context;
#if LH_LIBRARY_OPTION_UI
    if (lh_null_ne(self->frame_stats))
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
    if (lh_null_ne(self->frame_stats))
    {
        frame.us = lh_os_tick_us() - started_us;
        lh_ui_frame_stats_record(self->frame_stats, lh_addr_of(frame));
    }
#endif
    self->paint_dc = lh_null;
    self->paint_left = 0;
    self->paint_top = 0;
    self->paint_right = 0;
    self->paint_bottom = 0;
}

/** The mouse buttons @p event leaves held over @p self: an event of pointer `0` says, its
   cancel or a focus loss lets go of all of them, anything else -- a touch contact
   included -- leaves them as they were. */
static lh_void
lh_os_window_track_buttons(lh_os_window_t *self, lh_os_input_event_cptr event)
{
    lh_return_if(lh_os_input_event_is_pointer(event) && lh_os_input_event_get_pointer(event) != 0U);
    switch (lh_os_input_event_get_kind(event))
    {
    case lh_os_input_kind_pointer_down:
    case lh_os_input_kind_pointer_up:
    case lh_os_input_kind_pointer_move:
        self->pressed_buttons = lh_os_input_event_get_buttons(event);
        break;
    case lh_os_input_kind_pointer_cancel:
    case lh_os_input_kind_focus_out:
        self->pressed_buttons = 0U;
        break;
    default:
        break;
    }
}

/** @p event to @p self's controller, if any: fed, the whole controller cancelled after a
   focus loss, and its timer armed for a deadline the event may have set. */
static lh_void
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
    if (lh_null_ne(lh_ptr_rcast(lh_void, self->on_input)))
    {
        self->on_input(self->on_input_context, event);
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
    app = self->app;
    parent = self->parent;
    modal = self->modal;
    was_main = lh_bool_false;
    if (lh_null_ne(app))
    {
        was_main = lh_cast_static(lh_bool_t, self == lh_os_app_get_window(app));
    }
    reason = self->closing ? lh_os_window_close_reason_api : lh_os_window_close_reason_os;
    self->closing = lh_bool_false;
    self->controller_armed = lh_bool_false;
    self->handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    if (modal && lh_null_ne(parent) && lh_os_system_window_is_valid(parent->handle))
    {
        lh_os_system_window_set_enabled(parent->handle, lh_bool_true);
    }
    on_close = self->on_close;
    on_close_context = self->on_close_context;
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_close)))
    {
        on_close(self, reason, on_close_context);
    }
    lh_os_window_unlink(self);
    if (lh_null_ne(app))
    {
        if (was_main || lh_list_is_empty(lh_addr_of(app->windows)))
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
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_set_timer(self->handle, 0U);
    }
    self->tick_ms = 0U;
    self->controller_armed = lh_bool_false;
    for (;;)
    {
        child = lh_os_window_get_first_child(self);
        if (lh_null_eq(child))
        {
            break;
        }
        lh_os_window_close(child);
    }
    if (lh_os_system_window_is_valid(self->handle))
    {
        self->closing = lh_bool_true;
        lh_os_system_window_close(self->handle);
    }
    if (lh_null_ne(self->handle) || lh_list_node_is_linked(lh_addr_of(self->link)))
    {
        lh_os_window_on_native_destroy(self);
    }
}

lh_void
lh_os_window_deinit(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_window_close(self);
    self->on_close = lh_null;
    self->on_close_context = lh_null;
}
