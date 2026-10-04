/**
 * @file window.c
 * @brief POSIX/Linux X11 backend for `lh/os/system/window.h`.
 *
 * Mirrors the Win32 backend's API (`open` / `close` / `show` /
 * `pump_messages`) so callers (the lh `os/window.c` wrapper, `pa`) compile
 * against a single set of public functions regardless of platform. Both
 * backends store the OS handle as a bit-pattern `lh_ssize_t` and round-trip
 * it on every call.
 *
 * Xlib/X11 is the lowest-common-denominator windowing on the user's stated
 * XP-equivalent floor. No modern extensions (XRender, XRandR, XInput):
 * every API used here exists in the original X11R6 core shipped with XFree86
 * in the 1990s.
 *
 * Window-close handling: we register `WM_DELETE_WINDOW` against
 * `WM_PROTOCOLS` on the root of the new window; the event handler maps that
 * to `lh_bool_true` from `pump_messages`, mirroring Win32's `WM_QUIT`.
 */

#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/window.h>
#include <lh/os/system/posix/x11.h>
#include <lh/math.h>
#include <lh/util/ptr.h>
#include <lh/wstr.h>

#include <stddef.h>

/* Background fill, project orange (#FF7A18), as a native Xlib pixel: 24-bit
   `0xRRGGBB` on a TrueColor visual (no alpha in core Xlib). */
#define LH_OS_SYSTEM_POSIX_BRUSH_PIXEL 0xFF7A18UL

/* ── Process-wide X11 connection (lazy, single-threaded) ────────────────── */

/* `pa` is single-threaded in this iteration (the Win32 backend pumps from one
   thread too), so a process-wide Display* + GC is sufficient. When
   `LH_LIBRARY_OPTION_THREAD_LOCAL` lands, this becomes thread-local. */
static lh_os_system_posix_display_p lh_os_system_posix_display;
static lh_os_system_window_xid_t lh_os_system_posix_root_window;
static lh_os_system_atom_t lh_os_system_posix_atom_wm_protocols;
static lh_os_system_atom_t lh_os_system_posix_atom_wm_delete_window;

/* Connection-keep-alive: count active windows; close the Display when the
   last one shuts. Mirrors the Win32 backend's class-refcount logic. */
static lh_int_t lh_os_system_posix_connection_refcount;

static lh_bool_t
lh_os_system_posix_connect(void)
{
    if (lh_os_system_posix_connection_refcount > 0)
    {
        lh_os_system_posix_connection_refcount += 1;
        return lh_bool_true;
    }

    /* `XOpenDisplay(NULL)` → DISPLAY env or default ":0". Returns `NULL` on
       failure (no $DISPLAY, no server, no auth). */
    lh_os_system_posix_display = XOpenDisplay(lh_null);
    if (lh_null_eq(lh_os_system_posix_display))
    {
        return lh_bool_false;
    }

    lh_os_system_posix_root_window =
        lh_cast_static(lh_os_system_window_xid_t, lh_cast_static(lh_ulong_t, 0));
    lh_os_system_posix_atom_wm_protocols =
        XInternAtom(lh_os_system_posix_display, "WM_PROTOCOLS", 0);
    lh_os_system_posix_atom_wm_delete_window =
        XInternAtom(lh_os_system_posix_display, "WM_DELETE_WINDOW", 0);

    lh_os_system_posix_connection_refcount = 1;
    return lh_bool_true;
}

static void
lh_os_system_posix_disconnect(void)
{
    if (lh_os_system_posix_connection_refcount <= 0)
    {
        return;
    }
    lh_os_system_posix_connection_refcount -= 1;
    if (lh_os_system_posix_connection_refcount == 0 && !lh_null_eq(lh_os_system_posix_display))
    {
        (void)XCloseDisplay(lh_os_system_posix_display);
        lh_os_system_posix_display = lh_null;
    }
}

/* ── Public backend surface (matches Win32 signatures) ──────────────────── */

lh_os_system_window_handle_t
lh_os_system_window_open(const lh_ptr title, lh_int_t width, lh_int_t height)
{
    if (width <= 0 || height <= 0)
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    if (!lh_os_system_posix_connect())
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    /* `XCreateWindow` always needs an event mask on the new window for
       `Expose` (paint) and `KeyPress`/`ButtonPress` (input) to be
       delivered. `StructureNotifyMask` covers resize / close. */
    lh_os_system_posix_xset_window_attributes_t attrs;
    attrs.background_pixel = LH_OS_SYSTEM_POSIX_BRUSH_PIXEL;
    attrs.event_mask = LH_OS_SYSTEM_POSIX_EXPOSURE_MASK
        | LH_OS_SYSTEM_POSIX_KEY_PRESS_MASK | LH_OS_SYSTEM_POSIX_KEY_RELEASE_MASK
        | LH_OS_SYSTEM_POSIX_BUTTON_PRESS_MASK | LH_OS_SYSTEM_POSIX_BUTTON_RELEASE_MASK
        | LH_OS_SYSTEM_POSIX_STRUCTURE_NOTIFY_MASK | LH_OS_SYSTEM_POSIX_POINTER_MOTION_MASK;

    lh_os_system_window_xid_t window = XCreateWindow(
        lh_os_system_posix_display,
        lh_os_system_posix_root_window,
        0, 0, lh_cast_static(lh_uint_t, width), lh_cast_static(lh_uint_t, height),
        0, LH_OS_SYSTEM_POSIX_COPY_FROM_PARENT_DEPTH,
        LH_OS_SYSTEM_POSIX_INPUT_OUTPUT,
        lh_cast_static(lh_os_system_posix_visual_t *, lh_null),
        LH_OS_SYSTEM_POSIX_CW_BACK_PIXEL | LH_OS_SYSTEM_POSIX_CW_EVENT_MASK,
        lh_addr_of(attrs));

    if (lh_math_eq(window, LH_OS_SYSTEM_POSIX_XID_NONE))
    {
        lh_os_system_posix_disconnect();
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }

    /* Register `WM_DELETE_WINDOW` against `WM_PROTOCOLS` on this window so
       the user clicking the X button delivers a `ClientMessage` we can
       detect, instead of unmappable `DestroyNotify`. */
    lh_os_system_atom_t protocols[1];
    protocols[0] = lh_os_system_posix_atom_wm_delete_window;
    (void)XSetWMProtocols(lh_os_system_posix_display, window, protocols, 1);

    /* Title — X11 expects a `char *` (Latin-1, not UTF-8). For UTF-8 paths
       we'd need `Xutf8SetWMProperties` / XIC + ICU; out of scope for the
       first iteration. The `pa` smoke test passes ASCII so this works. */
    if (!lh_null_eq(title))
    {
        (void)XStoreName(lh_os_system_posix_display, window, (const char *)title);
    }

    /* Store the XID as a bit pattern — same trick the Win32 backend uses
       for `HWND`. */
    return lh_cast_static(lh_os_system_window_handle_t,
                          lh_cast_static(lh_ssize_t, (lh_ulong_t)window));
}

void
lh_os_system_window_close(lh_os_system_window_handle_t self)
{
    if (lh_math_eq(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID))
    {
        return;
    }
    lh_os_system_window_xid_t xid =
        lh_cast_static(lh_os_system_window_xid_t,
                       lh_cast_static(lh_ulong_t,
                                      lh_cast_static(lh_ssize_t, self)));
    (void)XDestroyWindow(lh_os_system_posix_display, xid);
    lh_os_system_posix_disconnect();
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
    lh_os_system_window_xid_t xid =
        lh_cast_static(lh_os_system_window_xid_t,
                       lh_cast_static(lh_ulong_t,
                                      lh_cast_static(lh_ssize_t, self)));
    (void)XMapWindow(lh_os_system_posix_display, xid);
    /* `XFlush` pushes the request to the server so the window appears
       immediately rather than after the next `XSync`. */
    (void)XFlush(lh_os_system_posix_display);
}

lh_bool_t
lh_os_system_window_pump_messages(void)
{
    if (lh_null_eq(lh_os_system_posix_display))
    {
        return lh_bool_false;
    }

    /* Drain every event the server already queued (non-blocking), so the
       caller's run loop drives both paint and idle input in lockstep. */
    while (XPending(lh_os_system_posix_display) > 0)
    {
        lh_os_system_posix_xevent_t ev;
        (void)XNextEvent(lh_os_system_posix_display, lh_addr_of(ev));

        if (ev.type == LH_OS_SYSTEM_POSIX_CLIENT_MESSAGE)
        {
            /* `WM_DELETE_WINDOW` is delivered as a `ClientMessage` whose
               first `long` is the atom; we registered `wm_delete_window`
               via `XSetWMProtocols` above. Treat it like Win32's `WM_QUIT`. */
            if (ev.as.client.data[0] == lh_cast_static(lh_ulong_t, lh_os_system_posix_atom_wm_delete_window))
            {
                return lh_bool_true;
            }
        }
        /* Expose / ConfigureNotify / KeyPress / ButtonPress: for now we
           simply rely on the `pa` redraw loop to repaint every frame. When
           a `pa_dirty_rects_t` layer lands, this becomes "mark the
           configured rect as dirty". */
    }
    return lh_bool_false;
}
lh_bool_t
lh_os_system_window_present(lh_os_system_window_handle_t self, const lh_ptr pixels,
                            lh_int_t stride, lh_int_t x, lh_int_t y, lh_int_t width,
                            lh_int_t height)
{
    /* Not yet on X11: needs XCreateImage / XPutImage with the visual's pixel
       layout. Reports that nothing was shown. */
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
    /* Not yet (X11: XPeekEvent would block until one): returns at once, so a
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

lh_int_t
lh_os_system_posix_window_span(lh_os_system_window_handle_t self, lh_bool_t outer,
                               lh_bool_t horizontal)
{
    lh_os_system_window_xid_t root = LH_OS_SYSTEM_POSIX_XID_NONE;
    lh_int_t origin_x = 0;
    lh_int_t origin_y = 0;
    lh_uint_t width = 0;
    lh_uint_t height = 0;
    lh_uint_t border = 0;
    lh_uint_t depth = 0;
    lh_os_system_window_xid_t xid;

    if (!lh_os_system_window_is_valid(self) || lh_null_eq(lh_os_system_posix_display))
    {
        return 0;
    }
    xid = lh_cast_static(lh_os_system_window_xid_t,
                         lh_cast_static(lh_ulong_t, lh_cast_static(lh_ssize_t, self)));
    if (XGetGeometry(lh_os_system_posix_display, xid, lh_addr_of(root), lh_addr_of(origin_x),
                     lh_addr_of(origin_y), lh_addr_of(width), lh_addr_of(height), lh_addr_of(border),
                     lh_addr_of(depth)) == 0)
    {
        return 0;
    }
    if (outer != lh_bool_false)
    {
        width += border * 2U;
        height += border * 2U;
    }
    if (horizontal != lh_bool_false)
    {
        return lh_cast_static(lh_int_t, width);
    }
    return lh_cast_static(lh_int_t, height);
}

lh_int_t
lh_os_system_window_get_width(lh_os_system_window_handle_t self)
{
    return lh_os_system_posix_window_span(self, lh_bool_true, lh_bool_true);
}

lh_int_t
lh_os_system_window_get_height(lh_os_system_window_handle_t self)
{
    return lh_os_system_posix_window_span(self, lh_bool_true, lh_bool_false);
}

lh_int_t
lh_os_system_window_get_client_width(lh_os_system_window_handle_t self)
{
    return lh_os_system_posix_window_span(self, lh_bool_false, lh_bool_true);
}

lh_int_t
lh_os_system_window_get_client_height(lh_os_system_window_handle_t self)
{
    return lh_os_system_posix_window_span(self, lh_bool_false, lh_bool_false);
}
