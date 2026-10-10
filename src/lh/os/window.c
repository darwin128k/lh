/**
 * @file window.c
 * @brief Portable ::lh_os_window_t on top of `lh/os/system/window.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/null.h>
#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    include <lh/os/system/window.h>
#endif
#if LH_LIBRARY_OPTION_UI
#    include <lh/os/tick.h>
#    include <lh/ui/frame/stats.h>
#endif

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
    self->on_press = lh_null;
    self->on_press_context = lh_null;
    self->on_move = lh_null;
    self->on_move_context = lh_null;
    self->on_release = lh_null;
    self->on_release_context = lh_null;
    self->on_wheel = lh_null;
    self->on_wheel_context = lh_null;
    self->on_key = lh_null;
    self->on_key_context = lh_null;
    self->on_text = lh_null;
    self->on_text_context = lh_null;
    self->on_click = lh_null;
    self->on_click_context = lh_null;
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
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    self->handle = lh_os_system_window_open(title, width, height, self,
                                            LH_OS_SYSTEM_WINDOW_HANDLE_INVALID, self->frame, self->corner,
                                            self->placement);
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
#else
    (void)width;
    (void)height;
    return lh_bool_false;
#endif
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
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
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
#else
    (void)width;
    (void)height;
    return lh_bool_false;
#endif
}

lh_bool_t
lh_os_window_is_open(const lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_os_system_window_is_valid(self->handle);
#else
    return lh_bool_false;
#endif
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
lh_os_window_set_on_press(lh_os_window_t *self, lh_os_window_on_press_cb on_press, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_press = on_press;
    self->on_press_context = context;
}

lh_void
lh_os_window_set_on_move(lh_os_window_t *self, lh_os_window_on_move_cb on_move, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_move = on_move;
    self->on_move_context = context;
}

lh_void
lh_os_window_set_on_release(lh_os_window_t *self, lh_os_window_on_release_cb on_release,
                            lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_release = on_release;
    self->on_release_context = context;
}

lh_void
lh_os_window_set_on_wheel(lh_os_window_t *self, lh_os_window_on_wheel_cb on_wheel, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_wheel = on_wheel;
    self->on_wheel_context = context;
}

lh_void
lh_os_window_set_on_key(lh_os_window_t *self, lh_os_window_on_key_cb on_key, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_key = on_key;
    self->on_key_context = context;
}

lh_void
lh_os_window_set_on_text(lh_os_window_t *self, lh_os_window_on_text_cb on_text, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_text = on_text;
    self->on_text_context = context;
}

lh_void
lh_os_window_set_on_click(lh_os_window_t *self, lh_os_window_on_click_cb on_click, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->on_click = on_click;
    self->on_click_context = context;
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
}

lh_void
lh_os_window_on_native_tick(lh_os_window_t *self)
{
    lh_os_window_on_tick_cb on_tick;
    lh_ptr context;

    lh_assert_runtime_ref(self);
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
lh_os_window_get_paint_rect(const lh_os_window_t *self, int *left, int *top, int *right, int *bottom)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(left);
    lh_assert_runtime_ref(top);
    lh_assert_runtime_ref(right);
    lh_assert_runtime_ref(bottom);
    *left = self->paint_left;
    *top = self->paint_top;
    *right = self->paint_right;
    *bottom = self->paint_bottom;
}

lh_void
lh_os_window_invalidate(lh_os_window_t *self)
{
    lh_assert_runtime_ref(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_invalidate(self->handle);
    }
#else
    (void)self;
#endif
}

lh_void
lh_os_window_invalidate_rect(lh_os_window_t *self, int left, int top, int right, int bottom)
{
    lh_assert_runtime_ref(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_invalidate_rect(self->handle, left, top, right, bottom);
    }
#else
    (void)left;
    (void)top;
    (void)right;
    (void)bottom;
#endif
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
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_set_timer(self->handle, 0U);
    }
#endif
    self->tick_ms = 0U;
    for (;;)
    {
        child = lh_os_window_get_first_child(self);
        if (lh_null_eq(child))
        {
            break;
        }
        lh_os_window_close(child);
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        self->closing = lh_bool_true;
        lh_os_system_window_close(self->handle);
    }
#endif
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
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_set_corner_radius(self->handle, self->corner);
    }
#else
    (void)radius;
#endif
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
lh_os_window_set_on_resize(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize, lh_ptr context)
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
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_minimize(self->handle);
    }
#else
    (void)self;
#endif
}

lh_void
lh_os_window_set_maximized(lh_os_window_t *self, lh_bool_t maximized)
{
    lh_assert_runtime_ref(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (lh_os_system_window_is_valid(self->handle))
    {
        lh_os_system_window_set_maximized(self->handle, maximized);
    }
#else
    (void)self;
    (void)maximized;
#endif
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
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_os_system_window_get_client_size(self->handle, width, height);
#else
    *width = 0;
    *height = 0;
    return lh_bool_false;
#endif
}

lh_bool_t
lh_os_window_get_position(const lh_os_window_t *self, int *x, int *y)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(x);
    lh_assert_runtime_ref(y);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_os_system_window_get_position(self->handle, x, y);
#else
    *x = 0;
    *y = 0;
    return lh_bool_false;
#endif
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
#    if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
        frame.asked_known =
            lh_os_system_window_get_paint_region(paint_dc, lh_addr_of(frame.asked_px), lh_addr_of(frame.rects));
#    else
        frame.asked_known = lh_bool_false;
        frame.asked_px = 0U;
        frame.rects = 0U;
#    endif
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

lh_void
lh_os_window_on_native_press(lh_os_window_t *self, int x, int y)
{
    lh_os_window_on_press_cb on_press;
    lh_ptr on_press_context;

    lh_assert_runtime_ref(self);
    on_press = self->on_press;
    on_press_context = self->on_press_context;
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_press)))
    {
        on_press(self, x, y, on_press_context);
    }
}

lh_void
lh_os_window_on_native_move(lh_os_window_t *self, int x, int y)
{
    lh_os_window_on_move_cb on_move;
    lh_ptr on_move_context;

    lh_assert_runtime_ref(self);
    on_move = self->on_move;
    on_move_context = self->on_move_context;
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_move)))
    {
        on_move(self, x, y, on_move_context);
    }
}

lh_void
lh_os_window_on_native_release(lh_os_window_t *self, int x, int y)
{
    lh_os_window_on_release_cb on_release;
    lh_ptr on_release_context;

    lh_assert_runtime_ref(self);
    on_release = self->on_release;
    on_release_context = self->on_release_context;
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_release)))
    {
        on_release(self, x, y, on_release_context);
    }
}

lh_void
lh_os_window_on_native_key(lh_os_window_t *self, lh_key_t key, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, self->on_key)));
    self->on_key(self, key, pressed, self->on_key_context);
}

lh_void
lh_os_window_on_native_text(lh_os_window_t *self, lh_u32_t code)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, self->on_text)));
    self->on_text(self, code, self->on_text_context);
}

lh_void
lh_os_window_on_native_wheel(lh_os_window_t *self, int x, int y, int dx, int dy)
{
    lh_os_window_on_wheel_cb on_wheel;
    lh_ptr on_wheel_context;

    lh_assert_runtime_ref(self);
    on_wheel = self->on_wheel;
    on_wheel_context = self->on_wheel_context;
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_wheel)))
    {
        on_wheel(self, x, y, dx, dy, on_wheel_context);
    }
}

lh_void
lh_os_window_on_native_click(lh_os_window_t *self, int x, int y)
{
    lh_os_window_on_click_cb on_click;
    lh_ptr on_click_context;

    lh_assert_runtime_ref(self);
    on_click = self->on_click;
    on_click_context = self->on_click_context;
    if (lh_null_ne(lh_ptr_rcast(lh_void, on_click)))
    {
        on_click(self, x, y, on_click_context);
    }
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
    self->handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (modal && lh_null_ne(parent) && lh_os_system_window_is_valid(parent->handle))
    {
        lh_os_system_window_set_enabled(parent->handle, lh_bool_true);
    }
#else
    (void)modal;
    (void)parent;
#endif
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
