/**
 * @file window.m
 * @brief macOS Cocoa backend for `lh/os/system/window.h`.
 *
 * Mirrors the Win32 / Xlib backends' public API (open / close / show /
 * pump_messages) so the lh `os/window.c` wrapper (and `pa`) compile against
 * a single set of public functions regardless of platform. All three
 * backends store the OS handle as a bit-pattern `lh_ssize_t`.
 *
 * Unlike Win32 / Xlib (frozen C ABIs that we declare by hand), Cocoa is
 * Objective-C with a runtime-typed message-passing ABI that evolves. We
 * therefore `#import <AppKit/AppKit.h>` and use the canonical Cocoatypes
 * — but **only** as opaque references in the four exported functions, so
 * the public header (`lh/os/system/window.h`) stays C-only.
 *
 * Drawing: set the NSWindow's `backgroundColor` to the same orange fill the
 * Win32 / X11 backends paint with. No custom NSView subclass needed for
 * the smoke test — the system's default content view shows the
 * backgroundColor.
 *
 * Close handling: an NSWindowDelegate whose `windowShouldClose:` returns
 * YES and sets a static flag the next `pump_messages` observes.
 *
 * macOS deployment target: 10.5+ (every API used here was available by then).
 */

#import <AppKit/AppKit.h>

#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/monitor.h>
#include <lh/os/system/window.h>
#include <lh/math.h>
#include <lh/util/ptr.h>
#include <lh/wstr.h>

#include <stddef.h>

/* Background fill, project orange (#FF7A18), as native Cocoa channels (0..1). */
#define LH_OS_SYSTEM_MACOS_BRUSH_RED ((CGFloat)0xFF / 255.0)
#define LH_OS_SYSTEM_MACOS_BRUSH_GREEN ((CGFloat)0x7A / 255.0)
#define LH_OS_SYSTEM_MACOS_BRUSH_BLUE ((CGFloat)0x18 / 255.0)

/* ── Process-wide state ─────────────────────────────────────────────────── */

/* `pa` is single-threaded; the Win32 / Xlib backends are also. When
   LH_LIBRARY_OPTION_THREAD_LOCAL lands, this becomes thread-local. */
static BOOL lh_os_system_macos_should_quit;
static NSApplication *lh_os_system_macos_app;

/* ── NSWindowDelegate: detects window-close and flips the static flag ───── */

@interface LHOsSystemMacosWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation LHOsSystemMacosWindowDelegate
- (BOOL)windowShouldClose:(NSWindow *)sender {
    (void)sender;
    lh_os_system_macos_should_quit = YES;
    /* We deliberately don't call `[NSApp terminate:]` here — it would
       post an `NSApplicationWillTerminateNotification` and exit the
       process. The flag above lets the caller's run loop notice the
       close and unwind cleanly. */
    return YES;
}
@end

static id
lh_os_system_macos_get_delegate(void)
{
    static id cached;
    if (cached != nil)
    {
        return cached;
    }
    cached = [[LHOsSystemMacosWindowDelegate alloc] init];
    return cached;
}

/* ── Public backend surface (matches Win32 / Xlib signatures) ───────────── */

lh_os_system_window_handle_t
lh_os_system_window_open(const lh_ptr title, lh_int_t width, lh_int_t height)
{
    if (width <= 0 || height <= 0)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    /* `NSApplication.sharedApplication` is a class method that returns the
       process-wide singleton; calling it lazily keeps the first
       `lh_os_system_window_open` as the only place we set up the AppKit
       machinery. */
    if (lh_os_system_macos_app == nil)
    {
        lh_os_system_macos_app = [NSApplication sharedApplication];
        /* `NSApplicationActivationPolicyRegular` shows the app in the Dock
           and lets it take key focus. The other policies (`.Accessory`,
           `.Prohibited`) hide it. */
        [lh_os_system_macos_app setActivationPolicy:NSApplicationActivationPolicyRegular];
    }

    /* `NSWindow`'s `initWithContentRect:styleMask:backing:defer:` is the
       canonical initializer. We pass `NSRect` zero and the size via
       `setContentSize:` after — `styleMask` selects titlebar / close
       button / resizability (`NSWindowStyleMaskTitled | .Closable |
       .Miniaturizable | .Resizable` matches `WS_OVERLAPPED | WS_CAPTION
       | WS_SYSMENU | ...` on Win32). */
    NSRect frame = NSMakeRect(0, 0, (CGFloat)width, (CGFloat)height);
    NSWindow *window = [[NSWindow alloc]
        initWithContentRect:frame
                      styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                              | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                        backing:NSBackingStoreBuffered
                          defer:NO];
    if (window == nil)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    /* Title — `NSString`'s `stringWithUTF8String:` accepts a NUL-terminated
       UTF-8 C string. For a wide-string title we'd add a UTF-16→UTF-8
       conversion here; the `pa` smoke test passes ASCII so this works. */
    if (title != lh_null)
    {
        NSString *titleStr = [NSString stringWithUTF8String:(const char *)title];
        if (titleStr != nil)
        {
            [window setTitle:titleStr];
        }
    }

    [window setBackgroundColor:[NSColor colorWithCalibratedRed:LH_OS_SYSTEM_MACOS_BRUSH_RED
                                                         green:LH_OS_SYSTEM_MACOS_BRUSH_GREEN
                                                          blue:LH_OS_SYSTEM_MACOS_BRUSH_BLUE
                                                         alpha:1.0]];

    [window setDelegate:lh_os_system_macos_get_delegate()];

    /* Store the NSWindow* as a bit pattern in `lh_ssize_t`, same trick the
       Win32 / Xlib backends use. (`NSWindow *` is a tagged pointer on
       64-bit macOS, but the `lh_ssize_t` round-trip is defined by the
       caller — we never inspect the value, only round-trip it.) */
    return lh_cast_static(lh_os_system_window_handle_t,
                           lh_cast_static(lh_ssize_t, (void *)window));
}

void
lh_os_system_window_close(lh_os_system_window_handle_t self)
{
    if (lh_math_eq(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID))
    {
        return;
    }
    NSWindow *window = (NSWindow *)(void *)lh_cast_static(lh_ssize_t, self);
    [window close];
}

lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t self)
{
    if (lh_math_eq(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    if (lh_math_eq(self, LH_OS_SYSTEM_WINDOW_HANDLE_NULL))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

void
lh_os_system_window_show(lh_os_system_window_handle_t self)
{
    if (!lh_os_system_window_is_valid(self))
    {
        return;
    }
    NSWindow *window = (NSWindow *)(void *)lh_cast_static(lh_ssize_t, self);
    [window makeKeyAndOrderFront:nil];
}

lh_bool_t
lh_os_system_window_pump_messages(void)
{
    if (lh_os_system_macos_app == nil)
    {
        return lh_bool_false;
    }

    /* The `NSDate distantPast` argument tells Cocoa "do not block waiting
       for an event" — pump one already-queued event (or return at once if
       none). Mirrors Win32's `PeekMessageW(..., PM_REMOVE)` and Xlib's
       `while (XPending > 0) { XNextEvent }` semantics. */
    NSEvent *event = [lh_os_system_macos_app
        nextEventMatchingMask:NSEventMaskAny
                    untilDate:[NSDate distantPast]
                       inMode:NSDefaultRunLoopMode
                      dequeue:YES];
    if (event != nil)
    {
        [lh_os_system_macos_app sendEvent:event];
    }

    return lh_os_system_macos_should_quit;
}
lh_bool_t
lh_os_system_window_present(lh_os_system_window_handle_t self, const lh_ptr pixels,
                            lh_int_t stride, lh_int_t x, lh_int_t y, lh_int_t width,
                            lh_int_t height)
{
    /* Not yet on Cocoa: needs a CGImage drawn from the view's drawRect.
       Reports that nothing was shown. */
    (void)self;
    (void)pixels;
    (void)stride;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    return lh_bool_false;
}

void
lh_os_system_window_wait_messages(void)
{
    /* Not yet (Cocoa: nextEventMatchingMask with distantFuture would block until one): returns at once, so a
       wait-then-pump loop still works, only without sleeping. */
}

void
lh_os_system_window_set_frame(lh_os_system_window_handle_t self, lh_int_t frame)
{
    (void)self;
    (void)frame;
}

lh_int_t
lh_os_system_window_get_frame(lh_os_system_window_handle_t self)
{
    (void)self;
    return LH_OS_SYSTEM_WINDOW_FRAME_SYSTEM;
}

void
lh_os_system_window_set_corner_radius(lh_os_system_window_handle_t self, lh_int_t radius)
{
    (void)self;
    (void)radius;
}

lh_int_t
lh_os_system_window_get_corner_radius(lh_os_system_window_handle_t self)
{
    (void)self;
    return 0;
}

void
lh_os_system_window_set_dark(lh_os_system_window_handle_t self, lh_bool_t dark)
{
    (void)self;
    (void)dark;
}

lh_bool_t
lh_os_system_window_get_dark(lh_os_system_window_handle_t self)
{
    (void)self;
    return lh_bool_false;
}

void
lh_os_system_window_set_chrome(lh_os_system_window_handle_t self, lh_uint_t caption, lh_uint_t text,
                               lh_uint_t border)
{
    (void)self;
    (void)caption;
    (void)text;
    (void)border;
}

void
lh_os_system_window_close_frame(lh_os_system_window_handle_t self)
{
    (void)self;
}

void
lh_os_system_window_minimize(lh_os_system_window_handle_t self)
{
    (void)self;
}

void
lh_os_system_window_zoom(lh_os_system_window_handle_t self)
{
    (void)self;
}

void
lh_os_system_window_begin_move(lh_os_system_window_handle_t self)
{
    (void)self;
}

void
lh_os_system_window_set_shadow(lh_os_system_window_handle_t self, const struct lh_ui_shadow *shadow)
{
    (void)self;
    (void)shadow;
}

const struct lh_ui_shadow *
lh_os_system_window_get_shadow(lh_os_system_window_handle_t self)
{
    (void)self;
    return lh_null;
}

lh_int_t
lh_os_system_window_get_width(lh_os_system_window_handle_t self)
{
    (void)self;
    return 0;
}

lh_int_t
lh_os_system_window_get_height(lh_os_system_window_handle_t self)
{
    (void)self;
    return 0;
}

lh_int_t
lh_os_system_window_get_client_width(lh_os_system_window_handle_t self)
{
    (void)self;
    return 0;
}

lh_int_t
lh_os_system_window_get_client_height(lh_os_system_window_handle_t self)
{
    (void)self;
    return 0;
}

void
lh_os_system_window_set_origin(lh_os_system_window_handle_t self, lh_int_t x, lh_int_t y)
{
    (void)self;
    (void)x;
    (void)y;
}

void
lh_os_system_window_set_tick(lh_os_system_window_handle_t self, lh_int_t milliseconds)
{
    (void)self;
    (void)milliseconds;
}

lh_int_t
lh_os_system_monitor_get_count(void)
{
    return 0;
}

lh_int_t
lh_os_system_monitor_get_width(lh_int_t index)
{
    (void)index;
    return 0;
}

lh_int_t
lh_os_system_monitor_get_height(lh_int_t index)
{
    (void)index;
    return 0;
}

lh_int_t
lh_os_system_monitor_get_work_width(lh_int_t index)
{
    (void)index;
    return 0;
}

lh_int_t
lh_os_system_monitor_get_work_height(lh_int_t index)
{
    (void)index;
    return 0;
}

lh_int_t
lh_os_system_monitor_get_x(lh_int_t index)
{
    (void)index;
    return 0;
}

lh_int_t
lh_os_system_monitor_get_y(lh_int_t index)
{
    (void)index;
    return 0;
}

lh_int_t
lh_os_system_monitor_get_work_x(lh_int_t index)
{
    (void)index;
    return 0;
}

lh_int_t
lh_os_system_monitor_get_work_y(lh_int_t index)
{
    (void)index;
    return 0;
}
