/**
 * @file window.m
 * @brief AppKit backend for `lh/os/system/window.h`.
 *
 * One `NSWindow` and a flipped content view, so the client origin is the top
 * left, the same way Win32 and X11 count. The pump is `nextEventMatchingMask`
 * — this file does not call `[NSApp run]`, because the portable app already
 * owns the loop. A paint's "DC" is ::lh_os_system_apple_paint_t. Key codes are
 * the virtual codes in `Events.h` (`kVK_Return` and the rest).
 */

#import <AppKit/AppKit.h>

#include <lh/os/system/window.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/cast/static.h>
#include <lh/core/memory.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/util/return.h>
#include <lh/core/input/event.h>
#include <lh/os/system/apple/paint.h>
#include <lh/os/tick.h>
#include <lh/os/window.h>

/** Virtual key codes (`Events.h`), not characters. */
#define LH_OS_SYSTEM_APPLE_KEY_RETURN 0x24
#define LH_OS_SYSTEM_APPLE_KEY_TAB 0x30
#define LH_OS_SYSTEM_APPLE_KEY_SPACE 0x31
#define LH_OS_SYSTEM_APPLE_KEY_DELETE 0x33
#define LH_OS_SYSTEM_APPLE_KEY_ESCAPE 0x35
#define LH_OS_SYSTEM_APPLE_KEY_SHIFT 0x38
#define LH_OS_SYSTEM_APPLE_KEY_OPTION 0x3A
#define LH_OS_SYSTEM_APPLE_KEY_CONTROL 0x3B
#define LH_OS_SYSTEM_APPLE_KEY_RIGHT_SHIFT 0x3C
#define LH_OS_SYSTEM_APPLE_KEY_RIGHT_OPTION 0x3D
#define LH_OS_SYSTEM_APPLE_KEY_RIGHT_CONTROL 0x3E
#define LH_OS_SYSTEM_APPLE_KEY_HOME 0x73
#define LH_OS_SYSTEM_APPLE_KEY_PAGE_UP 0x74
#define LH_OS_SYSTEM_APPLE_KEY_FORWARD_DELETE 0x75
#define LH_OS_SYSTEM_APPLE_KEY_END 0x77
#define LH_OS_SYSTEM_APPLE_KEY_PAGE_DOWN 0x79
#define LH_OS_SYSTEM_APPLE_KEY_LEFT 0x7B
#define LH_OS_SYSTEM_APPLE_KEY_RIGHT 0x7C
#define LH_OS_SYSTEM_APPLE_KEY_DOWN 0x7D
#define LH_OS_SYSTEM_APPLE_KEY_UP 0x7E

/** One open window. `native` is a retained `NSWindow *`. */
struct lh_os_system_apple_window
{
    struct lh_os_system_apple_window *next;
    lh_ptr native;
    struct lh_os_window *user;
    int corner;
    lh_bool_t enabled;
    lh_bool_t maximized;
    lh_bool_t has_restore;
    lh_u32_t tick_ms;
    lh_u64_t tick_due_us;
    double restore_x;
    double restore_y;
    double restore_w;
    double restore_h;
};

static struct lh_os_system_apple_window *lh_os_system_apple_window_list;
static struct lh_os_system_apple_window *lh_os_system_apple_window_dead;
static lh_bool_t lh_os_system_apple_quit;
static lh_bool_t lh_os_system_apple_started;
static int lh_os_system_apple_window_notifying;
static NSEventModifierFlags lh_os_system_apple_modifier_flags;

struct lh_os_system_apple_window *
lh_os_system_apple_window_get_next(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_void
lh_os_system_apple_window_set_next(struct lh_os_system_apple_window *self,
                                   struct lh_os_system_apple_window *next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_ptr
lh_os_system_apple_window_get_native(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->native;
}

lh_void
lh_os_system_apple_window_set_native(struct lh_os_system_apple_window *self, lh_ptr native)
{
    lh_assert_runtime_ref(self);
    self->native = native;
}

struct lh_os_window *
lh_os_system_apple_window_get_user(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_system_apple_window_set_user(struct lh_os_system_apple_window *self,
                                   struct lh_os_window *user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

int
lh_os_system_apple_window_get_corner(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->corner;
}

lh_void
lh_os_system_apple_window_set_corner(struct lh_os_system_apple_window *self, int corner)
{
    lh_assert_runtime_ref(self);
    self->corner = corner;
}

lh_bool_t
lh_os_system_apple_window_is_enabled(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->enabled;
}

lh_void
lh_os_system_apple_window_set_enabled_flag(struct lh_os_system_apple_window *self,
                                           lh_bool_t enabled)
{
    lh_assert_runtime_ref(self);
    self->enabled = enabled;
}

lh_bool_t
lh_os_system_apple_window_is_maximized(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->maximized;
}

lh_void
lh_os_system_apple_window_set_maximized_flag(struct lh_os_system_apple_window *self,
                                             lh_bool_t maximized)
{
    lh_assert_runtime_ref(self);
    self->maximized = maximized;
}

lh_bool_t
lh_os_system_apple_window_has_restore(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->has_restore;
}

lh_void
lh_os_system_apple_window_set_has_restore(struct lh_os_system_apple_window *self,
                                          lh_bool_t has_restore)
{
    lh_assert_runtime_ref(self);
    self->has_restore = has_restore;
}

lh_u32_t
lh_os_system_apple_window_get_tick_ms(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->tick_ms;
}

lh_void
lh_os_system_apple_window_set_tick_ms(struct lh_os_system_apple_window *self, lh_u32_t tick_ms)
{
    lh_assert_runtime_ref(self);
    self->tick_ms = tick_ms;
}

lh_u64_t
lh_os_system_apple_window_get_tick_due_us(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->tick_due_us;
}

lh_void
lh_os_system_apple_window_set_tick_due_us(struct lh_os_system_apple_window *self,
                                          lh_u64_t tick_due_us)
{
    lh_assert_runtime_ref(self);
    self->tick_due_us = tick_due_us;
}

double
lh_os_system_apple_window_get_restore_x(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->restore_x;
}

lh_void
lh_os_system_apple_window_set_restore_x(struct lh_os_system_apple_window *self, double restore_x)
{
    lh_assert_runtime_ref(self);
    self->restore_x = restore_x;
}

double
lh_os_system_apple_window_get_restore_y(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->restore_y;
}

lh_void
lh_os_system_apple_window_set_restore_y(struct lh_os_system_apple_window *self, double restore_y)
{
    lh_assert_runtime_ref(self);
    self->restore_y = restore_y;
}

double
lh_os_system_apple_window_get_restore_w(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->restore_w;
}

lh_void
lh_os_system_apple_window_set_restore_w(struct lh_os_system_apple_window *self, double restore_w)
{
    lh_assert_runtime_ref(self);
    self->restore_w = restore_w;
}

double
lh_os_system_apple_window_get_restore_h(const struct lh_os_system_apple_window *self)
{
    lh_assert_runtime_ref(self);
    return self->restore_h;
}

lh_void
lh_os_system_apple_window_set_restore_h(struct lh_os_system_apple_window *self, double restore_h)
{
    lh_assert_runtime_ref(self);
    self->restore_h = restore_h;
}

struct lh_os_system_apple_window *
lh_os_system_apple_window_find(lh_ptr native)
{
    for (struct lh_os_system_apple_window *self = lh_os_system_apple_window_list;
         lh_null_ne(self); self = lh_os_system_apple_window_get_next(self))
    {
        if (lh_os_system_apple_window_get_native(self) == native)
        {
            return self;
        }
    }
    return lh_null;
}

lh_void
lh_os_system_apple_window_unlink(struct lh_os_system_apple_window **head,
                                 struct lh_os_system_apple_window *self)
{
    struct lh_os_system_apple_window *previous = lh_null;

    for (struct lh_os_system_apple_window *it = lh_ptr_deref(head); lh_null_ne(it);
         it = lh_os_system_apple_window_get_next(it))
    {
        if (it == self)
        {
            struct lh_os_system_apple_window *next = lh_os_system_apple_window_get_next(self);

            if (lh_null_eq(previous))
            {
                lh_ptr_deref(head) = next;
            }
            else
            {
                lh_os_system_apple_window_set_next(previous, next);
            }
            lh_os_system_apple_window_set_next(self, lh_null);
            return;
        }
        previous = it;
    }
}

lh_void
lh_os_system_apple_window_free_dead(void)
{
    if (lh_os_system_apple_window_notifying > 0)
    {
        return;
    }
    while (lh_null_ne(lh_os_system_apple_window_dead))
    {
        struct lh_os_system_apple_window *self = lh_os_system_apple_window_dead;

        lh_os_system_apple_window_dead = lh_os_system_apple_window_get_next(self);
        NSWindow *window =
            (__bridge_transfer NSWindow *)lh_os_system_apple_window_get_native(self);

        [window setDelegate:nil];
        lh_os_system_apple_window_set_native(self, lh_null);
        lh_runtime_allocator_free(self);
    }
}

lh_void
lh_os_system_apple_window_start(void)
{
    if (lh_os_system_apple_started)
    {
        return;
    }
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp finishLaunching];
    lh_os_system_apple_started = lh_bool_true;
}

lh_key_t
lh_os_system_apple_key_from_code(unsigned short code)
{
    switch (code)
    {
    case LH_OS_SYSTEM_APPLE_KEY_TAB:
        return lh_key_tab;
    case LH_OS_SYSTEM_APPLE_KEY_RETURN:
        return lh_key_enter;
    case LH_OS_SYSTEM_APPLE_KEY_ESCAPE:
        return lh_key_escape;
    case LH_OS_SYSTEM_APPLE_KEY_DELETE:
        return lh_key_backspace;
    case LH_OS_SYSTEM_APPLE_KEY_FORWARD_DELETE:
        return lh_key_delete;
    case LH_OS_SYSTEM_APPLE_KEY_SPACE:
        return lh_key_space;
    case LH_OS_SYSTEM_APPLE_KEY_LEFT:
        return lh_key_left;
    case LH_OS_SYSTEM_APPLE_KEY_RIGHT:
        return lh_key_right;
    case LH_OS_SYSTEM_APPLE_KEY_UP:
        return lh_key_up;
    case LH_OS_SYSTEM_APPLE_KEY_DOWN:
        return lh_key_down;
    case LH_OS_SYSTEM_APPLE_KEY_HOME:
        return lh_key_home;
    case LH_OS_SYSTEM_APPLE_KEY_END:
        return lh_key_end;
    case LH_OS_SYSTEM_APPLE_KEY_PAGE_UP:
        return lh_key_page_up;
    case LH_OS_SYSTEM_APPLE_KEY_PAGE_DOWN:
        return lh_key_page_down;
    case LH_OS_SYSTEM_APPLE_KEY_SHIFT:
    case LH_OS_SYSTEM_APPLE_KEY_RIGHT_SHIFT:
        return lh_key_shift;
    case LH_OS_SYSTEM_APPLE_KEY_CONTROL:
    case LH_OS_SYSTEM_APPLE_KEY_RIGHT_CONTROL:
        return lh_key_control;
    case LH_OS_SYSTEM_APPLE_KEY_OPTION:
    case LH_OS_SYSTEM_APPLE_KEY_RIGHT_OPTION:
        return lh_key_alt;
    default:
        return lh_key_other;
    }
}

lh_input_mods_t
lh_os_system_apple_mods(NSEvent *event)
{
    NSEventModifierFlags flags = [event modifierFlags];
    lh_input_mods_t mods = LH_INPUT_MODS_NONE;

    mods = lh_cast_static(
        lh_input_mods_t,
        mods | lh_input_mods_if((flags & NSEventModifierFlagShift) != 0, LH_INPUT_MODS_SHIFT));
    mods = lh_cast_static(
        lh_input_mods_t,
        mods | lh_input_mods_if((flags & NSEventModifierFlagControl) != 0, LH_INPUT_MODS_CONTROL));
    mods = lh_cast_static(
        lh_input_mods_t,
        mods | lh_input_mods_if((flags & NSEventModifierFlagOption) != 0, LH_INPUT_MODS_ALT));
    mods = lh_cast_static(
        lh_input_mods_t,
        mods | lh_input_mods_if((flags & NSEventModifierFlagCommand) != 0, LH_INPUT_MODS_META));
    mods = lh_cast_static(lh_input_mods_t,
                          mods | lh_input_mods_if((flags & NSEventModifierFlagCapsLock) != 0,
                                                  LH_INPUT_MODS_CAPS_LOCK));
    return mods;
}

lh_u32_t
lh_os_system_apple_buttons(void)
{
    NSUInteger held = [NSEvent pressedMouseButtons];
    lh_u32_t mask = 0U;

    mask |= lh_input_button_mask_if((held & 1U) != 0U, lh_input_button_primary);
    mask |= lh_input_button_mask_if((held & 2U) != 0U, lh_input_button_secondary);
    mask |= lh_input_button_mask_if((held & 4U) != 0U, lh_input_button_middle);
    return mask;
}

lh_void
lh_os_system_apple_window_send(struct lh_os_window *user, lh_input_event_t *event, NSEvent *native)
{
    lh_input_event_set_mods(event, lh_os_system_apple_mods(native));
    lh_input_event_set_buttons(event, lh_os_system_apple_buttons());
    lh_os_window_on_native_input(user, event);
}

lh_void
lh_os_system_apple_window_apply_corner(NSWindow *window, int radius)
{
    NSView *view = [window contentView];

    [view setWantsLayer:YES];
    if (radius > 0)
    {
        [[view layer] setCornerRadius:radius];
        [[view layer] setMasksToBounds:YES];
        [window setOpaque:NO];
        [window setBackgroundColor:[NSColor clearColor]];
        return;
    }
    [[view layer] setCornerRadius:0];
    [[view layer] setMasksToBounds:NO];
    [window setOpaque:YES];
    [window setBackgroundColor:[NSColor windowBackgroundColor]];
}

NSRect
lh_os_system_apple_resized(NSRect frame, double dx, double dy, lh_os_window_zone_t zone)
{
    NSRect next = frame;
    const lh_bool_t left = zone == lh_os_window_zone_left || zone == lh_os_window_zone_top_left ||
                           zone == lh_os_window_zone_bottom_left;
    const lh_bool_t right = zone == lh_os_window_zone_right ||
                            zone == lh_os_window_zone_top_right ||
                            zone == lh_os_window_zone_bottom_right;
    const lh_bool_t bottom = zone == lh_os_window_zone_bottom ||
                             zone == lh_os_window_zone_bottom_left ||
                             zone == lh_os_window_zone_bottom_right;
    const lh_bool_t top = zone == lh_os_window_zone_top || zone == lh_os_window_zone_top_left ||
                          zone == lh_os_window_zone_top_right;

    if (left)
    {
        next.origin.x += dx;
        next.size.width -= dx;
    }
    if (right)
    {
        next.size.width += dx;
    }
    if (bottom)
    {
        next.origin.y += dy;
        next.size.height -= dy;
    }
    if (top)
    {
        next.size.height += dy;
    }
    if (next.size.width < 1.0)
    {
        next.size.width = 1.0;
    }
    if (next.size.height < 1.0)
    {
        next.size.height = 1.0;
    }
    return next;
}

lh_void
lh_os_system_apple_window_track(NSWindow *window, NSEvent *event, lh_os_window_zone_t zone)
{
    if (zone == lh_os_window_zone_caption)
    {
        [window performWindowDragWithEvent:event];
        return;
    }
    NSPoint start = [NSEvent mouseLocation];
    NSRect frame = [window frame];

    for (;;)
    {
        NSEvent *step = [NSApp nextEventMatchingMask:(NSEventMaskLeftMouseDragged |
                                                       NSEventMaskLeftMouseUp)
                                            untilDate:[NSDate distantFuture]
                                               inMode:NSEventTrackingRunLoopMode
                                              dequeue:YES];

        if (step == nil || [step type] == NSEventTypeLeftMouseUp)
        {
            break;
        }
        NSPoint now = [NSEvent mouseLocation];

        [window setFrame:lh_os_system_apple_resized(frame, now.x - start.x, now.y - start.y, zone)
                 display:YES];
    }
}

lh_void
lh_os_system_apple_window_pointer(NSWindow *window, NSEvent *event, lh_input_kind_t kind,
                                  lh_input_button_t button)
{
    struct lh_os_system_apple_window *self =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);

    if (lh_null_eq(self) || !lh_os_system_apple_window_is_enabled(self) ||
        lh_null_eq(lh_os_system_apple_window_get_user(self)))
    {
        return;
    }
    NSPoint point = [[window contentView] convertPoint:[event locationInWindow] fromView:nil];

    if (kind == lh_input_kind_pointer_down && button == lh_input_button_primary)
    {
        lh_os_window_zone_t zone = lh_os_window_zone_at(
            lh_os_system_apple_window_get_user(self), lh_cast_static(int, point.x),
            lh_cast_static(int, point.y));

        if (zone != lh_os_window_zone_client)
        {
            lh_os_system_apple_window_track(window, event, zone);
            return;
        }
    }
    lh_input_event_t input;

    lh_input_event_init_pointer(lh_addr_of(input), kind, lh_input_device_kind_mouse, 0U, button,
                                lh_input_coord_from_int(lh_cast_static(int, point.x)),
                                lh_input_coord_from_int(lh_cast_static(int, point.y)),
                                lh_os_tick_ms());
    lh_os_system_apple_window_send(lh_os_system_apple_window_get_user(self), lh_addr_of(input),
                                   event);
}

lh_void
lh_os_system_apple_window_key(NSWindow *window, NSEvent *event, lh_bool_t down)
{
    struct lh_os_system_apple_window *self =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);

    if (lh_null_eq(self) || !lh_os_system_apple_window_is_enabled(self) ||
        lh_null_eq(lh_os_system_apple_window_get_user(self)))
    {
        return;
    }
    lh_input_event_t input;

    lh_input_event_init_key(lh_addr_of(input), lh_os_system_apple_key_from_code([event keyCode]),
                            down, [event isARepeat] ? lh_bool_true : lh_bool_false, [event keyCode],
                            lh_os_tick_ms());
    lh_os_system_apple_window_send(lh_os_system_apple_window_get_user(self), lh_addr_of(input),
                                   event);
    if (!down)
    {
        return;
    }
    NSString *text = [event characters];

    if (lh_null_eq(text) || [text length] == 0U)
    {
        return;
    }
    unichar first = [text characterAtIndex:0];
    lh_u32_t code = first;

    if (first >= 0xD800 && first <= 0xDBFF && [text length] > 1U)
    {
        unichar second = [text characterAtIndex:1];

        code = 0x10000U + ((lh_cast_static(lh_u32_t, first) - 0xD800U) << 10) +
               (lh_cast_static(lh_u32_t, second) - 0xDC00U);
    }
    if (code < 0x20U || code == 0x7FU)
    {
        return;
    }
    lh_input_event_init_text(lh_addr_of(input), code, lh_os_tick_ms());
    lh_os_system_apple_window_send(lh_os_system_apple_window_get_user(self), lh_addr_of(input),
                                   event);
}

lh_void
lh_os_system_apple_window_flags(NSWindow *window, NSEvent *event)
{
    struct lh_os_system_apple_window *self =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);
    NSEventModifierFlags now = [event modifierFlags];
    NSEventModifierFlags changed = now ^ lh_os_system_apple_modifier_flags;

    lh_os_system_apple_modifier_flags = now;
    if (lh_null_eq(self) || !lh_os_system_apple_window_is_enabled(self) ||
        lh_null_eq(lh_os_system_apple_window_get_user(self)))
    {
        return;
    }
    const lh_key_t keys[3] = {lh_key_shift, lh_key_control, lh_key_alt};
    const NSEventModifierFlags bits[3] = {NSEventModifierFlagShift, NSEventModifierFlagControl,
                                          NSEventModifierFlagOption};

    for (int i = 0; i < 3; ++i)
    {
        if ((changed & bits[i]) == 0)
        {
            continue;
        }
        lh_input_event_t input;

        lh_input_event_init_key(lh_addr_of(input), keys[i],
                                (now & bits[i]) != 0 ? lh_bool_true : lh_bool_false, lh_bool_false,
                                0U, lh_os_tick_ms());
        lh_os_system_apple_window_send(lh_os_system_apple_window_get_user(self), lh_addr_of(input),
                                       event);
    }
}

lh_void
lh_os_system_apple_window_paint(NSWindow *window, NSRect dirty)
{
    struct lh_os_system_apple_window *self =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);

    if (lh_null_eq(self) || lh_null_eq(lh_os_system_apple_window_get_user(self)))
    {
        return;
    }
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
    lh_os_system_apple_paint_t paint;
    int left = lh_cast_static(int, dirty.origin.x);
    int top = lh_cast_static(int, dirty.origin.y);
    int right = left + lh_cast_static(int, dirty.size.width);
    int bottom = top + lh_cast_static(int, dirty.size.height);

    lh_memory_set(lh_addr_of(paint), sizeof(paint), 0);
    lh_os_system_apple_paint_set_context(lh_addr_of(paint), lh_ptr_rcast(lh_ptr, context));
    if (right < left)
    {
        right = left;
    }
    if (bottom < top)
    {
        bottom = top;
    }
    lh_os_system_apple_paint_set_area(lh_addr_of(paint),
                                      lh_cast_static(lh_u64_t, right - left) *
                                          lh_cast_static(lh_u64_t, bottom - top));
    lh_os_system_apple_paint_set_rects(lh_addr_of(paint), 1U);
    lh_os_window_on_native_paint(lh_os_system_apple_window_get_user(self), lh_addr_of(paint), left,
                                 top, right, bottom);
}

lh_void
lh_os_system_apple_window_will_close(NSWindow *window)
{
    struct lh_os_system_apple_window *self =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);

    if (lh_null_eq(self))
    {
        return;
    }
    lh_os_system_apple_window_notifying += 1;
    lh_os_system_apple_window_unlink(lh_addr_of(lh_os_system_apple_window_list), self);
    lh_os_system_apple_window_set_next(self, lh_os_system_apple_window_dead);
    lh_os_system_apple_window_dead = self;
    if (lh_null_ne(lh_os_system_apple_window_get_user(self)))
    {
        lh_os_window_on_native_destroy(lh_os_system_apple_window_get_user(self));
    }
    lh_os_system_apple_window_notifying -= 1;
}

lh_void
lh_os_system_apple_window_did_resize(NSWindow *window)
{
    struct lh_os_system_apple_window *self =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);

    if (lh_null_eq(self) || lh_null_eq(lh_os_system_apple_window_get_user(self)))
    {
        return;
    }
    NSRect bounds = [[window contentView] bounds];

    lh_os_window_on_native_resize(lh_os_system_apple_window_get_user(self),
                                  lh_cast_static(int, bounds.size.width),
                                  lh_cast_static(int, bounds.size.height),
                                  lh_os_system_apple_window_is_maximized(self));
}

lh_void
lh_os_system_apple_window_focus(NSWindow *window, lh_bool_t inside)
{
    struct lh_os_system_apple_window *self =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);

    if (lh_null_eq(self) || lh_null_eq(lh_os_system_apple_window_get_user(self)))
    {
        return;
    }
    lh_input_event_t input;

    lh_input_event_init(lh_addr_of(input),
                        inside ? lh_input_kind_focus_in : lh_input_kind_focus_out,
                        lh_input_device_kind_keyboard, lh_os_tick_ms());
    lh_os_window_on_native_input(lh_os_system_apple_window_get_user(self), lh_addr_of(input));
}

lh_void
lh_os_system_apple_window_fire_ticks(void)
{
    lh_u64_t now = lh_os_tick_us();
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_list;

    while (lh_null_ne(self))
    {
        if (lh_os_system_apple_window_get_tick_ms(self) != 0U &&
            now >= lh_os_system_apple_window_get_tick_due_us(self))
        {
            lh_u64_t period =
                lh_cast_static(lh_u64_t, lh_os_system_apple_window_get_tick_ms(self)) * 1000U;
            struct lh_os_window *user = lh_os_system_apple_window_get_user(self);

            lh_os_system_apple_window_set_tick_due_us(self, now + period);
            if (lh_null_ne(user))
            {
                lh_os_window_on_native_tick(user);
            }
            self = lh_os_system_apple_window_list;
            continue;
        }
        self = lh_os_system_apple_window_get_next(self);
    }
}

NSDate *
lh_os_system_apple_window_tick_date(void)
{
    lh_u64_t now = lh_os_tick_us();
    lh_u64_t soon = UINT64_MAX;
    lh_bool_t any = lh_bool_false;

    for (struct lh_os_system_apple_window *self = lh_os_system_apple_window_list; lh_null_ne(self);
         self = lh_os_system_apple_window_get_next(self))
    {
        if (lh_os_system_apple_window_get_tick_ms(self) == 0U)
        {
            continue;
        }
        any = lh_bool_true;
        if (lh_os_system_apple_window_get_tick_due_us(self) < soon)
        {
            soon = lh_os_system_apple_window_get_tick_due_us(self);
        }
    }
    if (!any)
    {
        return nil;
    }
    if (soon <= now)
    {
        return [NSDate date];
    }
    return [NSDate dateWithTimeIntervalSinceNow:lh_cast_static(NSTimeInterval, soon - now) /
                                                 1000000.0];
}

lh_void
lh_os_system_apple_window_place(NSWindow *window, NSWindow *owner,
                                lh_os_window_placement_t placement)
{
    if (placement != lh_os_window_placement_center)
    {
        NSRect work = [[NSScreen mainScreen] visibleFrame];

        [window cascadeTopLeftFromPoint:NSMakePoint(NSMinX(work), NSMaxY(work))];
        return;
    }
    NSScreen *screen = lh_null_ne(owner) ? [owner screen] : [window screen];

    if (screen == nil)
    {
        screen = [NSScreen mainScreen];
    }
    NSRect work = [screen visibleFrame];
    NSRect frame = [window frame];

    frame.origin.x = work.origin.x + (work.size.width - frame.size.width) / 2.0;
    frame.origin.y = work.origin.y + (work.size.height - frame.size.height) / 2.0;
    if (frame.size.width > work.size.width)
    {
        frame.origin.x = work.origin.x;
    }
    if (frame.size.height > work.size.height)
    {
        frame.origin.y = NSMaxY(work) - frame.size.height;
    }
    [window setFrame:frame display:NO];
}

@interface LHView : NSView
{
    NSTrackingArea *tracking;
}
@end

@interface LHDelegate : NSObject <NSWindowDelegate>
@end

@implementation LHView

- (BOOL)isFlipped
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)updateTrackingAreas
{
    [super updateTrackingAreas];
    if (tracking != nil)
    {
        [self removeTrackingArea:tracking];
    }
    tracking = [[NSTrackingArea alloc]
        initWithRect:[self bounds]
             options:(NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways |
                      NSTrackingInVisibleRect | NSTrackingMouseMoved)
               owner:self
            userInfo:nil];
    [self addTrackingArea:tracking];
}

- (void)drawRect:(NSRect)dirty
{
    lh_os_system_apple_window_paint([self window], dirty);
}

- (void)mouseDown:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_down,
                                      lh_input_button_primary);
}

- (void)mouseUp:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_up,
                                      lh_input_button_primary);
}

- (void)mouseDragged:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_move,
                                      lh_input_button_none);
}

- (void)rightMouseDown:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_down,
                                      lh_input_button_secondary);
}

- (void)rightMouseUp:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_up,
                                      lh_input_button_secondary);
}

- (void)rightMouseDragged:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_move,
                                      lh_input_button_none);
}

- (void)otherMouseDown:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_down,
                                      lh_input_button_middle);
}

- (void)otherMouseUp:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_up,
                                      lh_input_button_middle);
}

- (void)otherMouseDragged:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_move,
                                      lh_input_button_none);
}

- (void)mouseMoved:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_move,
                                      lh_input_button_none);
}

- (void)mouseEntered:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_enter,
                                      lh_input_button_none);
}

- (void)mouseExited:(NSEvent *)event
{
    lh_os_system_apple_window_pointer([self window], event, lh_input_kind_pointer_leave,
                                      lh_input_button_none);
}

- (void)scrollWheel:(NSEvent *)event
{
    NSWindow *window = [self window];
    struct lh_os_system_apple_window *record =
        lh_os_system_apple_window_find((__bridge lh_ptr)window);

    if (lh_null_eq(record) || !lh_os_system_apple_window_is_enabled(record) ||
        lh_null_eq(lh_os_system_apple_window_get_user(record)))
    {
        return;
    }
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    double lines_x = [event hasPreciseScrollingDeltas] ? [event scrollingDeltaX] / 10.0
                                                       : [event scrollingDeltaX];
    double lines_y = [event hasPreciseScrollingDeltas] ? [event scrollingDeltaY] / 10.0
                                                       : [event scrollingDeltaY];
    lh_input_event_t input;

    lh_input_event_init_wheel(
        lh_addr_of(input), lh_input_coord_from_int(lh_cast_static(int, point.x)),
        lh_input_coord_from_int(lh_cast_static(int, point.y)),
        lh_cast_static(lh_s32_t, -lines_x * LH_INPUT_WHEEL_NOTCH),
        lh_cast_static(lh_s32_t, lines_y * LH_INPUT_WHEEL_NOTCH), lh_os_tick_ms());
    lh_os_system_apple_window_send(lh_os_system_apple_window_get_user(record), lh_addr_of(input),
                                   event);
}

- (void)keyDown:(NSEvent *)event
{
    lh_os_system_apple_window_key([self window], event, lh_bool_true);
}

- (void)keyUp:(NSEvent *)event
{
    lh_os_system_apple_window_key([self window], event, lh_bool_false);
}

- (void)flagsChanged:(NSEvent *)event
{
    lh_os_system_apple_window_flags([self window], event);
}

@end

@implementation LHDelegate

- (BOOL)windowShouldClose:(NSWindow *)sender
{
    (void)sender;
    return YES;
}

- (void)windowWillClose:(NSNotification *)note
{
    lh_os_system_apple_window_will_close([note object]);
}

- (void)windowDidResize:(NSNotification *)note
{
    lh_os_system_apple_window_did_resize([note object]);
}

- (void)windowDidBecomeKey:(NSNotification *)note
{
    lh_os_system_apple_window_focus([note object], lh_bool_true);
}

- (void)windowDidResignKey:(NSNotification *)note
{
    lh_os_system_apple_window_focus([note object], lh_bool_false);
}

@end

lh_os_system_window_handle_t
lh_os_system_window_open(const lh_char_t *title, int width, int height, lh_ptr user,
                         lh_os_system_window_handle_t owner, lh_os_window_frame_t frame, int corner,
                         lh_os_window_placement_t placement)
{
    lh_return_if(width <= 0 || height <= 0, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    lh_os_system_apple_window_start();
    NSUInteger style = frame == lh_os_window_frame_own
                           ? NSWindowStyleMaskBorderless
                           : (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                              NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);
    NSRect content = NSMakeRect(0, 0, width, height);
    NSWindow *window = [[NSWindow alloc] initWithContentRect:content
                                                   styleMask:style
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    LHView *view = [[LHView alloc] initWithFrame:content];
    struct lh_os_system_apple_window *self = lh_ptr_rcast(
        struct lh_os_system_apple_window,
        lh_runtime_allocator_try_alloc(sizeof(lh_ptr_deref(self))));

    if (lh_null_eq(window) || lh_null_eq(view) || lh_null_eq(self))
    {
        lh_runtime_allocator_free(self);
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }
    lh_memory_set(self, sizeof(lh_ptr_deref(self)), 0);
    [view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [window setContentView:view];
    [window setReleasedWhenClosed:NO];
    [window setAcceptsMouseMovedEvents:YES];
    NSString *caption = lh_null_eq(title) ? @"" : [NSString stringWithUTF8String:title];

    [window setTitle:caption != nil ? caption : @""];
    LHDelegate *delegate = [[LHDelegate alloc] init];

    [window setDelegate:delegate];
    lh_os_system_apple_window_set_native(self, (__bridge_retained lh_ptr)window);
    lh_os_system_apple_window_set_user(self, lh_ptr_rcast(struct lh_os_window, user));
    lh_os_system_apple_window_set_enabled_flag(self, lh_bool_true);
    lh_os_system_apple_window_set_corner(self, corner > 0 ? corner : 0);
    lh_os_system_apple_window_apply_corner(window, lh_os_system_apple_window_get_corner(self));
    NSWindow *parent = (__bridge NSWindow *)owner;

    if (lh_null_ne(owner))
    {
        [parent addChildWindow:window ordered:NSWindowAbove];
    }
    lh_os_system_apple_window_place(window, parent, placement);
    lh_os_system_apple_window_set_next(self, lh_os_system_apple_window_list);
    lh_os_system_apple_window_list = self;
    [window makeKeyAndOrderFront:nil];
    [[window contentView] updateTrackingAreas];
    [window makeFirstResponder:[window contentView]];
    [NSApp activateIgnoringOtherApps:YES];
    return (__bridge lh_ptr)window;
}

lh_void
lh_os_system_window_set_corner_radius(lh_os_system_window_handle_t handle, int radius)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    int corner = radius > 0 ? radius : 0;

    lh_os_system_apple_window_set_corner(self, corner);
    lh_os_system_apple_window_apply_corner((__bridge NSWindow *)handle, corner);
}

lh_void
lh_os_system_window_minimize(lh_os_system_window_handle_t handle)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    [(__bridge NSWindow *)handle miniaturize:nil];
}

lh_void
lh_os_system_window_set_maximized(lh_os_system_window_handle_t handle, lh_bool_t maximized)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    NSWindow *window = (__bridge NSWindow *)handle;

    if (maximized && !lh_os_system_apple_window_is_maximized(self))
    {
        NSRect frame = [window frame];

        lh_os_system_apple_window_set_restore_x(self, frame.origin.x);
        lh_os_system_apple_window_set_restore_y(self, frame.origin.y);
        lh_os_system_apple_window_set_restore_w(self, frame.size.width);
        lh_os_system_apple_window_set_restore_h(self, frame.size.height);
        lh_os_system_apple_window_set_has_restore(self, lh_bool_true);
        lh_os_system_apple_window_set_maximized_flag(self, lh_bool_true);
        [window setFrame:[[window screen] visibleFrame] display:YES];
        return;
    }
    if (!maximized && lh_os_system_apple_window_has_restore(self))
    {
        NSRect frame;

        frame.origin.x = lh_os_system_apple_window_get_restore_x(self);
        frame.origin.y = lh_os_system_apple_window_get_restore_y(self);
        frame.size.width = lh_os_system_apple_window_get_restore_w(self);
        frame.size.height = lh_os_system_apple_window_get_restore_h(self);
        lh_os_system_apple_window_set_has_restore(self, lh_bool_false);
        lh_os_system_apple_window_set_maximized_flag(self, lh_bool_false);
        [window setFrame:frame display:YES];
    }
}

lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t handle)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(handle));
}

lh_void
lh_os_system_window_set_enabled(lh_os_system_window_handle_t handle, lh_bool_t enabled)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    lh_os_system_apple_window_set_enabled_flag(self, enabled);
    [(__bridge NSWindow *)handle setIgnoresMouseEvents:enabled ? NO : YES];
}

lh_void
lh_os_system_window_close(lh_os_system_window_handle_t handle)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    [(__bridge NSWindow *)handle close];
    lh_os_system_apple_window_free_dead();
}

lh_os_system_window_pump_result_t
lh_os_system_window_pump_wait(lh_tick_t timeout_ms)
{
    lh_os_system_apple_window_free_dead();
    if (lh_os_system_apple_quit)
    {
        return lh_os_system_window_pump_quit;
    }
    lh_os_system_apple_window_start();
    lh_os_system_apple_window_fire_ticks();
    NSDate *until = timeout_ms == LH_TICK_T_MAX
                        ? [NSDate distantFuture]
                        : [NSDate dateWithTimeIntervalSinceNow:lh_cast_static(NSTimeInterval,
                                                                               timeout_ms) /
                                                               1000.0];
    NSDate *tick = lh_os_system_apple_window_tick_date();

    if (tick != nil && [tick compare:until] == NSOrderedAscending)
    {
        until = tick;
    }
    NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                        untilDate:until
                                           inMode:NSDefaultRunLoopMode
                                          dequeue:YES];

    if (lh_os_system_apple_quit)
    {
        lh_os_system_apple_window_free_dead();
        return lh_os_system_window_pump_quit;
    }
    if (event == nil)
    {
        lh_os_system_apple_window_fire_ticks();
        lh_os_system_apple_window_free_dead();
        return lh_os_system_window_pump_timeout;
    }
    do
    {
        [NSApp sendEvent:event];
        if (lh_os_system_apple_quit)
        {
            lh_os_system_apple_window_free_dead();
            return lh_os_system_window_pump_quit;
        }
        event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                    untilDate:[NSDate date]
                                       inMode:NSDefaultRunLoopMode
                                      dequeue:YES];
    } while (event != nil);
    lh_os_system_apple_window_fire_ticks();
    lh_os_system_apple_window_free_dead();
    return lh_os_system_window_pump_message;
}

lh_void
lh_os_system_window_post_quit(void)
{
    lh_os_system_apple_quit = lh_bool_true;
    if (!lh_os_system_apple_started)
    {
        return;
    }
    NSEvent *wake = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                       location:NSMakePoint(0, 0)
                                  modifierFlags:0
                                      timestamp:0
                                   windowNumber:0
                                        context:nil
                                        subtype:0
                                          data1:0
                                          data2:0];

    [NSApp postEvent:wake atStart:YES];
}

lh_void
lh_os_system_window_invalidate(lh_os_system_window_handle_t handle)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    [[(__bridge NSWindow *)handle contentView] setNeedsDisplay:YES];
}

lh_void
lh_os_system_window_invalidate_rect(lh_os_system_window_handle_t handle, int left, int top,
                                    int right, int bottom)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    [[(__bridge NSWindow *)handle contentView]
        setNeedsDisplayInRect:NSMakeRect(left, top, right - left, bottom - top)];
}

lh_void
lh_os_system_window_set_timer(lh_os_system_window_handle_t handle, lh_u32_t ms)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    if (lh_null_eq(self))
    {
        return;
    }
    lh_os_system_apple_window_set_tick_ms(self, ms);
    if (ms == 0U)
    {
        return;
    }
    lh_os_system_apple_window_set_tick_due_us(
        self, lh_os_tick_us() + lh_cast_static(lh_u64_t, ms) * 1000U);
}

lh_bool_t
lh_os_system_window_get_client_size(lh_os_system_window_handle_t handle, int *width, int *height)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);

    lh_return_if(lh_null_eq(self) || lh_null_eq(width) || lh_null_eq(height), lh_bool_false);
    NSRect bounds = [[(__bridge NSWindow *)handle contentView] bounds];

    lh_ptr_deref(width) = lh_cast_static(int, bounds.size.width);
    lh_ptr_deref(height) = lh_cast_static(int, bounds.size.height);
    return lh_bool_true;
}

lh_bool_t
lh_os_system_window_get_paint_region(lh_ptr paint_dc, lh_u64_t *area, lh_u32_t *count)
{
    lh_return_if(lh_null_eq(area) || lh_null_eq(count), lh_bool_false);
    lh_ptr_deref(area) = 0U;
    lh_ptr_deref(count) = 0U;
    lh_return_if(lh_null_eq(paint_dc), lh_bool_false);
    const lh_os_system_apple_paint_t *paint =
        lh_ptr_rcast(const lh_os_system_apple_paint_t, paint_dc);

    lh_ptr_deref(area) = lh_os_system_apple_paint_get_area(paint);
    lh_ptr_deref(count) = lh_os_system_apple_paint_get_rects(paint);
    return lh_bool_true;
}

lh_bool_t
lh_os_system_window_get_position(lh_os_system_window_handle_t handle, int *x, int *y)
{
    struct lh_os_system_apple_window *self = lh_os_system_apple_window_find(handle);
    CGFloat primary = 0;

    lh_return_if(lh_null_eq(self) || lh_null_eq(x) || lh_null_eq(y), lh_bool_false);
    for (NSScreen *screen in [NSScreen screens])
    {
        if (NSMinX([screen frame]) == 0.0 && NSMinY([screen frame]) == 0.0)
        {
            primary = [screen frame].size.height;
        }
    }
    NSRect frame = [(__bridge NSWindow *)handle frame];

    lh_ptr_deref(x) = lh_cast_static(int, frame.origin.x);
    lh_ptr_deref(y) = lh_cast_static(int, primary - (frame.origin.y + frame.size.height));
    return lh_bool_true;
}
