/**
 * @file x11.h
 * @brief Backend-private: the part of libX11 the Linux window backend uses,
 *        declared by us instead of `<X11/Xlib.h>`.
 *
 * Same rules as ws2_32.h / user32.h on Windows: only what is called, `lh`-prefixed
 * types and constants, Xlib function and member names (Xlib is an old ABI,
 * names are frozen).
 *
 * `Xlib` types come in two flavours: opaque pointers (`Display`, `GC`,
 * `Visual`, `Screen`) and integer `XID`s (`Window`, `Drawable`, `Atom`,
 * `Pixmap`, `Cursor`). We mirror that split — opaque types become
 * `struct foo *` (forward-declared here), XIDs become a single
 * `lh_os_system_posix_xid_t` `typedef` of `lh_ulong_t`. Keeps the rest of
 * the backend from ever naming `<X11/X*.h>` directly.
 *
 * XP-equivalent note: we never use any modern X extension (XInput, XRender
 * for visual effects, XRandR for runtime resize). The basic Xlib core is
 * what shipped with X11R6 in the 1990s and works everywhere.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_H

#include <lh/cast/static.h>
#include <lh/cast/reinterpret.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>
#include <lh/size.h>

/* `XID` — every X server resource token (Window, Drawable, Atom, Pixmap,
   Cursor, ...). Same width as `unsigned long` on every platform lh targets
   (LP64 Unix = 64-bit, LLP64 Windows = 32-bit, but we never build the
   POSIX window backend on Windows — `LH_OS_SYSTEM_BACKEND` excludes it). */
typedef lh_ulong_t lh_os_system_posix_xid_t;

/* Forward-declared opaque types — only used as pointers. */
typedef struct lh_os_system_posix_display lh_os_system_posix_display_t;
typedef struct lh_os_system_posix_visual  lh_os_system_posix_visual_t;
typedef struct lh_os_system_posix_screen   lh_os_system_posix_screen_t;
typedef struct lh_os_system_posix_gc       lh_os_system_posix_gc_t;

/* Opaque-pointer aliases — matches Xlib's `Display *` etc. so that
   `XOpenDisplay` etc. line up with no extra casts at call sites. */
typedef lh_os_system_posix_display_t *lh_os_system_posix_display_p;

/* `Window`, `Drawable`, `Atom`, `Pixmap`, `Cursor` — all XIDs. */
typedef lh_os_system_posix_xid_t lh_os_system_window_xid_t;
typedef lh_os_system_posix_xid_t lh_os_system_drawable_xid_t;
typedef lh_os_system_posix_xid_t lh_os_system_atom_t;

/* No window / no atom / no XID sentinels — Xlib's only "no" sentinel is
   the all-ones pattern (`(XID)-1`), which `LH_OS_SYSTEM_POSIX_XID_NONE`
   captures. */
#define LH_OS_SYSTEM_POSIX_XID_NONE                                                              \
    (lh_cast_static(lh_os_system_posix_xid_t, lh_cast_static(lh_ssize_t, -1)))

/* `XEvent` — Xlib's tagged union has 30+ variants. We declare only the
   fields we actually read in the window proc (Expose / KeyPress /
   ButtonPress / ConfigureNotify / ClientMessage), with enough storage
   (`data[5]`) for the largest variant (ClientMessage.data.l). Wrong-by-ABI
   for the unused variants, but the union tag makes that impossible to
   observe: every read is gated by `v == XXXX_Event`. */
typedef struct lh_os_system_posix_xevent
{
    lh_int_t type;
    union {
        struct {
            lh_int_t x;
            lh_int_t y;
            lh_uint_t width;
            lh_uint_t height;
            lh_int_t count;
        } expose;
        struct {
            lh_uint_t button;
            lh_uint_t x;
            lh_uint_t y;
        } button;
        struct {
            lh_uint_t keycode;
        } key;
        struct {
            lh_int_t x;
            lh_int_t y;
            lh_uint_t width;
            lh_uint_t height;
        } configure;
        struct {
            lh_ulong_t data[5];
        } client;
    } as;
} lh_os_system_posix_xevent_t;

/* `XSetWindowAttributes` — passed to `XCreateWindow` with a valuemask
   describing which fields are set. We only ever set `background_pixel` and
   `event_mask`. */
typedef struct lh_os_system_posix_xset_window_attributes
{
    lh_ulong_t background_pixel;
    lh_ulong_t event_mask;
} lh_os_system_posix_xset_window_attributes_t;

/* ── Constants we actually use ──────────────────────────────────────────── */

/* Event types (`type` field of `XEvent`). */
#define LH_OS_SYSTEM_POSIX_EXPOSE          12
#define LH_OS_SYSTEM_POSIX_CONFIGURE_NOTIFY 22
#define LH_OS_SYSTEM_POSIX_BUTTON_PRESS     4
#define LH_OS_SYSTEM_POSIX_BUTTON_RELEASE   5
#define LH_OS_SYSTEM_POSIX_KEY_PRESS        2
#define LH_OS_SYSTEM_POSIX_KEY_RELEASE      3
#define LH_OS_SYSTEM_POSIX_CLIENT_MESSAGE   33

/* `XCreateWindow` valuemask bits. */
#define LH_OS_SYSTEM_POSIX_CW_BACK_PIXEL (1L << 1)
#define LH_OS_SYSTEM_POSIX_CW_EVENT_MASK  (1L << 17)

/* `XCreateWindow` class. */
#define LH_OS_SYSTEM_POSIX_INPUT_OUTPUT      1
#define LH_OS_SYSTEM_POSIX_COPY_FROM_PARENT  0

/* `Depth` (XCreateWindow's `depth` arg). `CopyFromParent` means "match the
   parent window's depth", which is what we always want for a top-level. */
#define LH_OS_SYSTEM_POSIX_COPY_FROM_PARENT_DEPTH  0

/* Event mask bits. */
#define LH_OS_SYSTEM_POSIX_EXPOSURE_MASK         (1L << 15)
#define LH_OS_SYSTEM_POSIX_BUTTON_PRESS_MASK     (1L << 2)
#define LH_OS_SYSTEM_POSIX_BUTTON_RELEASE_MASK   (1L << 3)
#define LH_OS_SYSTEM_POSIX_KEY_PRESS_MASK        (1L << 0)
#define LH_OS_SYSTEM_POSIX_KEY_RELEASE_MASK      (1L << 1)
#define LH_OS_SYSTEM_POSIX_STRUCTURE_NOTIFY_MASK (1L << 17)
#define LH_OS_SYSTEM_POSIX_POINTER_MOTION_MASK   (1L << 6)

/* `XInternAtoms` — names of well-known atoms we register once per
   connection (we only need `WM_PROTOCOLS` and `WM_DELETE_WINDOW` for
   window-close handling). */
#define LH_OS_SYSTEM_POSIX_ATOM_WM_PROTOCOLS   1
#define LH_OS_SYSTEM_POSIX_ATOM_WM_DELETE_WINDOW 2

/* ── Function declarations (libX11, link with `-lX11`) ──────────────────── */

lh_os_system_posix_display_p
XOpenDisplay(const char *display_name);

int
XCloseDisplay(lh_os_system_posix_display_p display);

lh_os_system_window_xid_t
XCreateWindow(lh_os_system_posix_display_p display,
              lh_os_system_window_xid_t parent, lh_int_t x, lh_int_t y,
              lh_uint_t width, lh_uint_t height, lh_uint_t border_width,
              lh_int_t depth, lh_uint_t class,
              lh_os_system_posix_visual_t *visual,
              lh_ulong_t valuemask,
              lh_os_system_posix_xset_window_attributes_t *attributes);

int
XDestroyWindow(lh_os_system_posix_display_p display,
               lh_os_system_window_xid_t window);

int
XMapWindow(lh_os_system_posix_display_p display,
           lh_os_system_window_xid_t window);

int
XStoreName(lh_os_system_posix_display_p display,
           lh_os_system_window_xid_t window, const char *window_name);

int
XSelectInput(lh_os_system_posix_display_p display,
             lh_os_system_window_xid_t window, lh_long_t event_mask);

lh_os_system_posix_gc_t *
XCreateGC(lh_os_system_posix_display_p display,
          lh_os_system_drawable_xid_t drawable, lh_ulong_t valuemask, void *values);

int
XFreeGC(lh_os_system_posix_display_p display, lh_os_system_posix_gc_t *gc);

int
XSetForeground(lh_os_system_posix_display_p display,
               lh_os_system_posix_gc_t *gc, lh_ulong_t foreground);

int
XFillRectangle(lh_os_system_posix_display_p display,
               lh_os_system_drawable_xid_t drawable,
               lh_os_system_posix_gc_t *gc, lh_int_t x, lh_int_t y,
               lh_uint_t width, lh_uint_t height);

int
XPending(lh_os_system_posix_display_p display);

int
XNextEvent(lh_os_system_posix_display_p display,
           lh_os_system_posix_xevent_t *event_return);

int
XFlush(lh_os_system_posix_display_p display);

int
XSync(lh_os_system_posix_display_p display, int discard);

lh_os_system_atom_t
XInternAtom(lh_os_system_posix_display_p display, const char *atom_name, int only_if_exists);

int
XSetWMProtocols(lh_os_system_posix_display_p display,
                lh_os_system_window_xid_t window,
                lh_os_system_atom_t *protocols, int count);

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_H */