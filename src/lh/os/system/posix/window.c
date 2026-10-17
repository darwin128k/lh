/**
 * @file window.c
 * @brief X11 backend for `lh/os/system/window.h`.
 *
 * Windows has a message queue per thread, a window procedure per window, and a
 * paint the system schedules. X has one connection to a server and a stream of
 * events on it, so the parts Windows does for us are done here: the area that
 * waits to be painted is kept per window and painted at the end of a pump, the
 * tick is a deadline per window that the pump waits for, and a window that is
 * disabled for a modal child simply has its input dropped.
 *
 * Core Xlib only, plus the EWMH hints every current window manager reads
 * (`_NET_WM_NAME`, `_NET_WM_STATE`, `_NET_WM_MOVERESIZE`, `_NET_WORKAREA`) and
 * `_MOTIF_WM_HINTS` for a window that draws its own frame.
 */

#include <lh/assert/runtime.h>
#include <lh/byte/ptr.h>
#include <lh/memory.h>
#include <lh/os/alloc.h>
#include <lh/os/system/posix/connection.h>
#include <lh/os/system/posix/input.h>
#include <lh/os/system/posix/key.h>
#include <lh/os/system/posix/paint.h>
#include <lh/os/system/posix/window/fields.h>
#include <lh/os/system/posix/x11.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>
#include <lh/os/window.h>
#include <lh/runtime/allocator.h>
#include <lh/util/return.h>

#include <poll.h>

/** @brief One window of ours. */
struct lh_os_system_posix_window
{
    /** Members: ::lh_os_system_posix_window_fields. */
    lh_os_system_posix_window_fields;
};
/** @copybrief lh_os_system_posix_window */
typedef struct lh_os_system_posix_window lh_os_system_posix_window_t;

/** How long an open waits for the window to be on the screen. */
#define LH_OS_SYSTEM_POSIX_WINDOW_MAP_WAIT_MS 1000

/** @{ */
/** `_NET_WM_MOVERESIZE` directions. */
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_TOP_LEFT 0L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_TOP 1L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_TOP_RIGHT 2L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_RIGHT 3L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_BOTTOM_RIGHT 4L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_BOTTOM 5L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_BOTTOM_LEFT 6L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_LEFT 7L
#define LH_OS_SYSTEM_POSIX_MOVERESIZE_MOVE 8L
/** @} */

/* -- The connection --------------------------------------------------------
   One per process, opened by the first window and kept: the pump is one thread's,
   as it is on Windows, and reconnecting for every window that opens would buy
   nothing but round trips. */

/** libX11, once loaded. */
static const lh_os_system_posix_x11_t *lh_os_system_posix_x;
/** The connection to the X server; null until the first window opens. */
static lh_os_system_posix_display_t *lh_os_system_posix_display;
/** The default screen. */
static int lh_os_system_posix_screen;
/** Bits per pixel of the default visual. */
static int lh_os_system_posix_depth;
/** The root window of ::lh_os_system_posix_screen. */
static lh_os_system_posix_xid_t lh_os_system_posix_root;
/** Every open window of ours, linked through `next`. */
static lh_os_system_posix_window_t *lh_os_system_posix_windows;
/** Set by a quit request; ends the pump. */
static lh_bool_t lh_os_system_posix_quit;
/** Set by the error handler: the last request failed. */
static lh_bool_t lh_os_system_posix_failed;
/** -1 not asked yet, 0 the server has no SHAPE, 1 it has. */
static int lh_os_system_posix_shape = -1;
/** The keycode of the key release last skipped as an auto-repeat, `0` for none: the
   press that follows it is that key repeating (X keycodes start at 8). */
static lh_u32_t lh_os_system_posix_repeat_keycode;

/** @{ */
/** Atoms interned once by ::lh_os_system_posix_connect, named as the protocol names them. */
static lh_os_system_posix_xid_t lh_os_system_posix_atom_wm_protocols;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_wm_delete_window;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_net_wm_name;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_utf8_string;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_net_wm_state;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_net_wm_state_max_vert;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_net_wm_state_max_horz;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_net_workarea;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_net_wm_moveresize;
static lh_os_system_posix_xid_t lh_os_system_posix_atom_motif_wm_hints;
/** @} */

/** Xlib's default error handler prints and calls exit(). A window that is already
   gone when a request about it arrives -- the window manager closed it a moment
   before -- is not a reason to end the program, so errors are dropped here. */
static int
lh_os_system_posix_on_error(lh_os_system_posix_display_t *display,
                            lh_os_system_posix_xerror_event_t *error)
{
    (void)display;
    (void)error;
    lh_os_system_posix_failed = lh_bool_true;
    return 0;
}

lh_os_system_posix_display_t *
lh_os_system_posix_connection_get_display(void)
{
    return lh_os_system_posix_display;
}

lh_bool_t
lh_os_system_posix_connection_take_error(void)
{
    const lh_bool_t failed = lh_os_system_posix_failed;

    lh_os_system_posix_failed = lh_bool_false;
    return failed;
}

/** Open the X connection once per process, and intern the atoms; false without a server. */
static lh_bool_t
lh_os_system_posix_connect(void)
{
    const lh_os_system_posix_x11_t *x;

    lh_return_if(lh_null_ne(lh_os_system_posix_display), lh_bool_true);
    x = lh_os_system_posix_x11_load();
    lh_return_if(lh_null_eq(x), lh_bool_false);
    /* NULL is $DISPLAY. No server, no $DISPLAY, no permission: no window, and the
       caller is told so by an invalid handle. */
    lh_os_system_posix_display = x->XOpenDisplay(lh_null);
    lh_return_if(lh_null_eq(lh_os_system_posix_display), lh_bool_false);
    lh_os_system_posix_x = x;
    (void)x->XSetErrorHandler(lh_os_system_posix_on_error);
    lh_os_system_posix_screen = x->XDefaultScreen(lh_os_system_posix_display);
    lh_os_system_posix_depth =
        x->XDefaultDepth(lh_os_system_posix_display, lh_os_system_posix_screen);
    lh_os_system_posix_root = x->XRootWindow(lh_os_system_posix_display, lh_os_system_posix_screen);
    lh_os_system_posix_atom_wm_protocols =
        x->XInternAtom(lh_os_system_posix_display, "WM_PROTOCOLS", 0);
    lh_os_system_posix_atom_wm_delete_window =
        x->XInternAtom(lh_os_system_posix_display, "WM_DELETE_WINDOW", 0);
    lh_os_system_posix_atom_net_wm_name =
        x->XInternAtom(lh_os_system_posix_display, "_NET_WM_NAME", 0);
    lh_os_system_posix_atom_utf8_string =
        x->XInternAtom(lh_os_system_posix_display, "UTF8_STRING", 0);
    lh_os_system_posix_atom_net_wm_state =
        x->XInternAtom(lh_os_system_posix_display, "_NET_WM_STATE", 0);
    lh_os_system_posix_atom_net_wm_state_max_vert =
        x->XInternAtom(lh_os_system_posix_display, "_NET_WM_STATE_MAXIMIZED_VERT", 0);
    lh_os_system_posix_atom_net_wm_state_max_horz =
        x->XInternAtom(lh_os_system_posix_display, "_NET_WM_STATE_MAXIMIZED_HORZ", 0);
    lh_os_system_posix_atom_net_workarea =
        x->XInternAtom(lh_os_system_posix_display, "_NET_WORKAREA", 0);
    lh_os_system_posix_atom_net_wm_moveresize =
        x->XInternAtom(lh_os_system_posix_display, "_NET_WM_MOVERESIZE", 0);
    lh_os_system_posix_atom_motif_wm_hints =
        x->XInternAtom(lh_os_system_posix_display, "_MOTIF_WM_HINTS", 0);
    return lh_bool_true;
}

/* -- Windows of ours -------------------------------------------------------
   A handle is a pointer to one of these, and every call checks it is still in the
   list: an event or a call for a window that has gone finds nothing and does
   nothing, rather than reading freed memory. */

/** Our window behind @p handle, or ::lh_null when it is not (or no longer) one. */
static lh_os_system_posix_window_t *
lh_os_system_posix_window_find(lh_os_system_window_handle_t handle)
{
    lh_os_system_posix_window_t *window;

    for (window = lh_os_system_posix_windows; lh_null_ne(window); window = window->next)
    {
        lh_return_if(lh_ptr_rcast(lh_void, window) == handle, window);
    }
    return lh_null;
}

/** Our window with X id @p xid, or ::lh_null. */
static lh_os_system_posix_window_t *
lh_os_system_posix_window_find_xid(lh_os_system_posix_xid_t xid)
{
    lh_os_system_posix_window_t *window;

    for (window = lh_os_system_posix_windows; lh_null_ne(window); window = window->next)
    {
        lh_return_if(window->window == xid, window);
    }
    return lh_null;
}

/** Take @p self out of the list of open windows. */
static lh_void
lh_os_system_posix_window_unlist(lh_os_system_posix_window_t *self)
{
    lh_os_system_posix_window_t **at;

    for (at = lh_addr_of(lh_os_system_posix_windows); lh_null_ne(lh_ptr_deref(at));
         at = lh_addr_of((lh_ptr_deref(at))->next))
    {
        if (lh_ptr_deref(at) == self)
        {
            lh_ptr_deref(at) = self->next;
            return;
        }
    }
}

/** Add a rectangle to what waits to be painted, clipped to the window. Windows
   keeps the update region itself; here it is the bounding box of what was asked
   for, which is what a paint on Windows is handed too (`rcPaint`). */
static lh_void
lh_os_system_posix_window_damage(lh_os_system_posix_window_t *self, int left, int top, int right,
                                 int bottom)
{
    left = left < 0 ? 0 : left;
    top = top < 0 ? 0 : top;
    right = right > self->width ? self->width : right;
    bottom = bottom > self->height ? self->height : bottom;
    lh_return_if(left >= right || top >= bottom);
    if (!self->dirty)
    {
        self->dirty = lh_bool_true;
        self->dirty_left = left;
        self->dirty_top = top;
        self->dirty_right = right;
        self->dirty_bottom = bottom;
        return;
    }
    self->dirty_left = left < self->dirty_left ? left : self->dirty_left;
    self->dirty_top = top < self->dirty_top ? top : self->dirty_top;
    self->dirty_right = right > self->dirty_right ? right : self->dirty_right;
    self->dirty_bottom = bottom > self->dirty_bottom ? bottom : self->dirty_bottom;
}

/** How far in from the side row @p row of a corner of @p radius starts: the first
   pixel whose centre is inside the circle. In half pixels, so that it is all
   integers -- the same cut CreateRoundRectRgn makes on Windows, near enough that
   the two cannot be told apart at the radii a UI uses. */
static int
lh_os_system_posix_corner_inset(int radius, int row)
{
    const long dy = 2L * radius - 2L * row - 1L;
    int inset;

    for (inset = 0; inset < radius; ++inset)
    {
        const long dx = 2L * radius - 2L * inset - 1L;

        lh_return_if(dx * dx + dy * dy <= 4L * radius * radius, inset);
    }
    return radius;
}

/** The largest radius cut, which is far past any corner a UI draws and keeps the
   rectangles on the stack. */
#define LH_OS_SYSTEM_POSIX_CORNER_MAX 128

/** The rows of a round-rect of @p radius over @p width x @p height: one rectangle
   per row of the top corners and the bottom ones, and one for everything between.
   Returns how many. */
static int
lh_os_system_posix_corner_rows(lh_os_system_posix_xrectangle_t *rows, int width, int height,
                               int radius)
{
    int count = 0;
    int row;

    for (row = 0; row < radius; ++row)
    {
        const int inset = lh_os_system_posix_corner_inset(radius, row);

        rows[count].x = lh_cast_static(short, inset);
        rows[count].y = lh_cast_static(short, row);
        rows[count].width = lh_cast_static(unsigned short, (width - 2 * inset));
        rows[count].height = 1U;
        ++count;
        rows[count].x = lh_cast_static(short, inset);
        rows[count].y = lh_cast_static(short, (height - 1 - row));
        rows[count].width = lh_cast_static(unsigned short, (width - 2 * inset));
        rows[count].height = 1U;
        ++count;
    }
    rows[count].x = 0;
    rows[count].y = lh_cast_static(short, radius);
    rows[count].width = lh_cast_static(unsigned short, width);
    rows[count].height = lh_cast_static(unsigned short, (height - 2 * radius));
    return count + 1;
}

/** The radius the window is cut at now: none while maximized, and never more than
   half the window or the most this backend cuts. */
static int
lh_os_system_posix_window_radius(const lh_os_system_posix_window_t *self)
{
    int radius = self->maximized ? 0 : self->corner;

    radius = radius < self->width / 2 ? radius : self->width / 2;
    radius = radius < self->height / 2 ? radius : self->height / 2;
    return radius < LH_OS_SYSTEM_POSIX_CORNER_MAX ? radius : LH_OS_SYSTEM_POSIX_CORNER_MAX;
}

/** Cut the window's corners round, or give it back its rectangle. The shape is the
   window's, of the size it was cut at, so it is cut again on every resize -- and
   taken off while the window is maximized, as a maximized window has no corners
   on any desktop. Without the SHAPE extension the corners simply stay square.

   SHAPE is what takes the corners off on a server without a compositor, and what
   lets a click there fall through. Under a compositor -- every Wayland desktop,
   through XWayland -- a cut alone shows black, and the corners are cleared to
   transparent at every paint as well (::lh_os_system_posix_window_paint). A new cut
   repaints the window, because a corner that is smaller now has pixels to show
   that were transparent a moment ago. */
static lh_void
lh_os_system_posix_window_cut(lh_os_system_posix_window_t *self)
{
    const lh_os_system_posix_xext_t *ext = lh_os_system_posix_xext_load();
    lh_os_system_posix_xrectangle_t rows[2 * LH_OS_SYSTEM_POSIX_CORNER_MAX + 1];
    const int radius = lh_os_system_posix_window_radius(self);

    if (self->backing != LH_OS_SYSTEM_POSIX_X_NONE)
    {
        lh_os_system_posix_window_damage(self, 0, 0, self->width, self->height);
    }
    lh_return_if(lh_null_eq(ext));
    if (lh_os_system_posix_shape < 0)
    {
        int event_base = 0;
        int error_base = 0;

        lh_os_system_posix_shape =
            ext->XShapeQueryExtension(lh_os_system_posix_display, lh_addr_of(event_base),
                                      lh_addr_of(error_base)) != 0
                ? 1
                : 0;
    }
    lh_return_if(lh_os_system_posix_shape == 0);
    if (radius <= 0)
    {
        ext->XShapeCombineMask(lh_os_system_posix_display, self->window,
                               LH_OS_SYSTEM_POSIX_X_SHAPE_BOUNDING, 0, 0, LH_OS_SYSTEM_POSIX_X_NONE,
                               LH_OS_SYSTEM_POSIX_X_SHAPE_SET);
        return;
    }
    ext->XShapeCombineRectangles(
        lh_os_system_posix_display, self->window, LH_OS_SYSTEM_POSIX_X_SHAPE_BOUNDING, 0, 0, rows,
        lh_os_system_posix_corner_rows(rows, self->width, self->height, radius),
        LH_OS_SYSTEM_POSIX_X_SHAPE_SET, LH_OS_SYSTEM_POSIX_X_SHAPE_UNSORTED);
}

/** The rows of the corners that are outside a round-rect of @p radius: on the left
   and the right of every row of the cut. Returns how many. */
static int
lh_os_system_posix_corner_outside(lh_os_system_posix_xrectangle_t *rows, int width, int height,
                                  int radius)
{
    int count = 0;
    int row;

    for (row = 0; row < radius; ++row)
    {
        const int inset = lh_os_system_posix_corner_inset(radius, row);

        if (inset > 0)
        {
            rows[count].x = 0;
            rows[count].y = lh_cast_static(short, row);
            rows[count].width = lh_cast_static(unsigned short, inset);
            rows[count].height = 1U;
            rows[count + 1] = rows[count];
            rows[count + 1].x = lh_cast_static(short, (width - inset));
            rows[count + 2] = rows[count];
            rows[count + 2].y = lh_cast_static(short, (height - 1 - row));
            rows[count + 3] = rows[count + 1];
            rows[count + 3].y = lh_cast_static(short, (height - 1 - row));
            count += 4;
        }
    }
    return count;
}

/** The pixmap a paint goes into, the size of the window: made when the window is,
   and made again when its size changes. What was in the old one is not kept: a
   window of another size is painted again whole. */
static lh_void
lh_os_system_posix_window_back(lh_os_system_posix_window_t *self)
{
    const lh_os_system_posix_x11_t *x = lh_os_system_posix_x;
    const lh_os_system_posix_xrender_t *render = lh_os_system_posix_xrender_load();

    if (self->backing_picture != LH_OS_SYSTEM_POSIX_X_NONE)
    {
        render->XRenderFreePicture(lh_os_system_posix_display, self->backing_picture);
    }
    if (self->backing != LH_OS_SYSTEM_POSIX_X_NONE)
    {
        (void)x->XFreePixmap(lh_os_system_posix_display, self->backing);
    }
    self->backing = x->XCreatePixmap(lh_os_system_posix_display, self->window,
                                     lh_cast_static(unsigned int, self->width),
                                     lh_cast_static(unsigned int, self->height),
                                     lh_cast_static(unsigned int, lh_os_system_posix_depth));
    self->backing_picture = render->XRenderCreatePicture(
        lh_os_system_posix_display, self->backing,
        render->XRenderFindStandardFormat(lh_os_system_posix_display,
                                          LH_OS_SYSTEM_POSIX_X_PICT_STANDARD_RGB24),
        0UL, lh_null);
}

/** Paint the dirty rectangle of @p self through the app, then show it. */
static lh_void
lh_os_system_posix_window_paint(lh_os_system_posix_window_t *self)
{
    lh_os_system_posix_paint_t paint;
    const int left = self->dirty_left;
    const int top = self->dirty_top;
    const int right = self->dirty_right;
    const int bottom = self->dirty_bottom;

    self->dirty = lh_bool_false;
    paint.x11 = lh_os_system_posix_x;
    paint.display = lh_os_system_posix_display;
    paint.window = self->backing != LH_OS_SYSTEM_POSIX_X_NONE ? self->backing : self->window;
    paint.gc = self->gc;
    paint.depth = lh_os_system_posix_depth;
    paint.area =
        lh_cast_static(lh_u64_t, (right - left)) * lh_cast_static(lh_u64_t, (bottom - top));
    paint.rects = 1U;
    lh_os_window_on_native_paint(self->user, lh_addr_of(paint), left, top, right, bottom);
    /* What the paint put in the pixmap goes into the window, opaque, and the
       corners outside the cut go back to transparent -- every paint, because every
       paint can cover them. The window may have been closed by the paint. */
    if (lh_null_ne(lh_os_system_posix_window_find(self)) &&
        self->backing != LH_OS_SYSTEM_POSIX_X_NONE)
    {
        const lh_os_system_posix_xrender_t *render = lh_os_system_posix_xrender_load();
        const int radius = lh_os_system_posix_window_radius(self);

        render->XRenderComposite(lh_os_system_posix_display, LH_OS_SYSTEM_POSIX_X_PICT_OP_SRC,
                                 self->backing_picture, LH_OS_SYSTEM_POSIX_X_NONE,
                                 self->window_picture, left, top, 0, 0, left, top,
                                 lh_cast_static(unsigned int, (right - left)),
                                 lh_cast_static(unsigned int, (bottom - top)));
        if (radius > 0)
        {
            const lh_os_system_posix_xrender_color_t transparent = {0U, 0U, 0U, 0U};
            lh_os_system_posix_xrectangle_t rows[4 * LH_OS_SYSTEM_POSIX_CORNER_MAX];

            render->XRenderFillRectangles(
                lh_os_system_posix_display, LH_OS_SYSTEM_POSIX_X_PICT_OP_SRC, self->window_picture,
                lh_addr_of(transparent), rows,
                lh_os_system_posix_corner_outside(rows, self->width, self->height, radius));
        }
    }
}

/** Whether the window manager has the window maximized: both halves of
   `_NET_WM_STATE_MAXIMIZED` in its `_NET_WM_STATE`. */
static lh_bool_t
lh_os_system_posix_window_read_maximized(const lh_os_system_posix_window_t *self)
{
    const lh_os_system_posix_x11_t *x = lh_os_system_posix_x;
    lh_os_system_posix_xid_t type = LH_OS_SYSTEM_POSIX_X_NONE;
    int format = 0;
    unsigned long count = 0UL;
    unsigned long after = 0UL;
    lh_byte_ptr data = lh_null;
    lh_bool_t vertical = lh_bool_false;
    lh_bool_t horizontal = lh_bool_false;
    unsigned long i;

    if (x->XGetWindowProperty(
            lh_os_system_posix_display, self->window, lh_os_system_posix_atom_net_wm_state, 0L, 64L,
            0, LH_OS_SYSTEM_POSIX_X_XA_ATOM, lh_addr_of(type), lh_addr_of(format),
            lh_addr_of(count), lh_addr_of(after), lh_addr_of(data)) != LH_OS_SYSTEM_POSIX_X_SUCCESS)
    {
        return lh_bool_false;
    }
    if (lh_null_ne(data))
    {
        /* Format 32 comes back as `long`s whatever the width of a long is. */
        const lh_os_system_posix_xid_t *atoms = lh_ptr_rcast(const lh_os_system_posix_xid_t, data);

        for (i = 0UL; format == 32 && i < count; ++i)
        {
            vertical =
                atoms[i] == lh_os_system_posix_atom_net_wm_state_max_vert ? lh_bool_true : vertical;
            horizontal = atoms[i] == lh_os_system_posix_atom_net_wm_state_max_horz ? lh_bool_true
                                                                                   : horizontal;
        }
        (void)x->XFree(data);
    }
    return vertical && horizontal ? lh_bool_true : lh_bool_false;
}

/** The usable part of the screen -- without panels and docks -- from
   `_NET_WORKAREA`, or the whole screen when the window manager does not say. */
static lh_void
lh_os_system_posix_workarea(int *x, int *y, int *width, int *height)
{
    const lh_os_system_posix_x11_t *xlib = lh_os_system_posix_x;
    lh_os_system_posix_xid_t type = LH_OS_SYSTEM_POSIX_X_NONE;
    int format = 0;
    unsigned long count = 0UL;
    unsigned long after = 0UL;
    lh_byte_ptr data = lh_null;

    lh_ptr_deref(x) = 0;
    lh_ptr_deref(y) = 0;
    lh_ptr_deref(width) =
        xlib->XDisplayWidth(lh_os_system_posix_display, lh_os_system_posix_screen);
    lh_ptr_deref(height) =
        xlib->XDisplayHeight(lh_os_system_posix_display, lh_os_system_posix_screen);
    if (xlib->XGetWindowProperty(lh_os_system_posix_display, lh_os_system_posix_root,
                                 lh_os_system_posix_atom_net_workarea, 0L, 4L, 0,
                                 LH_OS_SYSTEM_POSIX_X_XA_CARDINAL, lh_addr_of(type),
                                 lh_addr_of(format), lh_addr_of(count), lh_addr_of(after),
                                 lh_addr_of(data)) != LH_OS_SYSTEM_POSIX_X_SUCCESS)
    {
        return;
    }
    if (lh_null_ne(data))
    {
        const long *area = lh_ptr_rcast(const long, data);

        /* The first desktop's area: x, y, width, height. */
        if (format == 32 && count >= 4UL && area[2] > 0 && area[3] > 0)
        {
            lh_ptr_deref(x) = lh_cast_static(int, area[0]);
            lh_ptr_deref(y) = lh_cast_static(int, area[1]);
            lh_ptr_deref(width) = lh_cast_static(int, area[2]);
            lh_ptr_deref(height) = lh_cast_static(int, area[3]);
        }
        (void)xlib->XFree(data);
    }
}

/** Hand a press on the caller's own chrome to the window manager, which moves or
   resizes the window as if its own frame had been pressed. The press already
   grabbed the pointer for us, and the window manager cannot take it while we
   hold it. */
static lh_bool_t
lh_os_system_posix_window_begin_moveresize(lh_os_system_posix_window_t *self,
                                           lh_os_window_zone_t zone, int x_root, int y_root)
{
    lh_os_system_posix_xevent_t event;
    long direction;

    switch (zone)
    {
    case lh_os_window_zone_caption:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_MOVE;
        break;
    case lh_os_window_zone_left:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_LEFT;
        break;
    case lh_os_window_zone_right:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_RIGHT;
        break;
    case lh_os_window_zone_top:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_TOP;
        break;
    case lh_os_window_zone_bottom:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_BOTTOM;
        break;
    case lh_os_window_zone_top_left:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_TOP_LEFT;
        break;
    case lh_os_window_zone_top_right:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_TOP_RIGHT;
        break;
    case lh_os_window_zone_bottom_left:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_BOTTOM_LEFT;
        break;
    case lh_os_window_zone_bottom_right:
        direction = LH_OS_SYSTEM_POSIX_MOVERESIZE_BOTTOM_RIGHT;
        break;
    case lh_os_window_zone_client:
    default:
        return lh_bool_false;
    }
    (void)lh_os_system_posix_x->XUngrabPointer(lh_os_system_posix_display,
                                               LH_OS_SYSTEM_POSIX_X_CURRENT_TIME);
    lh_memory_set(lh_addr_of(event), sizeof(event), 0);
    event.client.type = LH_OS_SYSTEM_POSIX_X_CLIENT_MESSAGE;
    event.client.window = self->window;
    event.client.message_type = lh_os_system_posix_atom_net_wm_moveresize;
    event.client.format = 32;
    event.client.data[0] = x_root;
    event.client.data[1] = y_root;
    event.client.data[2] = direction;
    event.client.data[3] = lh_cast_static(long, LH_OS_SYSTEM_POSIX_X_BUTTON_LEFT);
    event.client.data[4] = 1L; /* source: an application */
    (void)lh_os_system_posix_x->XSendEvent(lh_os_system_posix_display, lh_os_system_posix_root, 0,
                                           LH_OS_SYSTEM_POSIX_X_SUBSTRUCTURE_REDIRECT_MASK |
                                               LH_OS_SYSTEM_POSIX_X_SUBSTRUCTURE_NOTIFY_MASK,
                                           lh_addr_of(event));
    return lh_bool_true;
}

/** A key release that X sends only because the key is auto-repeating: the next
   event is a press of the same key at the same moment. Windows repeats a held key
   as presses with no releases between them, and an event stream does the same: the
   release is dropped and the press is marked ::LH_OS_INPUT_FLAG_REPEAT. */
static lh_bool_t
lh_os_system_posix_is_repeat(const lh_os_system_posix_xevent_t *release)
{
    lh_os_system_posix_xevent_t next;

    lh_return_if(lh_os_system_posix_x->XEventsQueued(
                     lh_os_system_posix_display, LH_OS_SYSTEM_POSIX_X_QUEUED_AFTER_READING) == 0,
                 lh_bool_false);
    (void)lh_os_system_posix_x->XPeekEvent(lh_os_system_posix_display, lh_addr_of(next));
    return next.type == LH_OS_SYSTEM_POSIX_X_KEY_PRESS && next.key.window == release->key.window &&
                   next.key.keycode == release->key.keycode && next.key.time == release->key.time
               ? lh_bool_true
               : lh_bool_false;
}

/** Whether the top-left corner of the window is anywhere a person could see it. */
static lh_bool_t
lh_os_system_posix_window_is_placed(const lh_os_system_posix_window_t *self)
{
    const lh_os_system_posix_x11_t *x = lh_os_system_posix_x;
    lh_os_system_posix_xid_t child = LH_OS_SYSTEM_POSIX_X_NONE;
    int at_x = 0;
    int at_y = 0;

    lh_return_if(x->XTranslateCoordinates(lh_os_system_posix_display, self->window,
                                          lh_os_system_posix_root, 0, 0, lh_addr_of(at_x),
                                          lh_addr_of(at_y), lh_addr_of(child)) == 0,
                 lh_bool_false);
    return at_x > lh_math_neg(self->width) && at_y > lh_math_neg(self->height) &&
                   at_x < x->XDisplayWidth(lh_os_system_posix_display, lh_os_system_posix_screen) &&
                   at_y < x->XDisplayHeight(lh_os_system_posix_display, lh_os_system_posix_screen)
               ? lh_bool_true
               : lh_bool_false;
}

/** Wait until the window is on the screen and where it is going to stay, or until
   a second has gone. Windows creates and places a window before CreateWindow
   returns; X maps it when the server gets round to it and the window manager
   places it after that -- some (Weston, under WSLg) map it far off the screen
   first and move it a few hundred milliseconds later. A caller that asks where its
   window is straight after opening it should get the answer, not the parking spot.

   Only the window's own MapNotify is taken off the queue; everything else stays
   there for the pump. */
static lh_void
lh_os_system_posix_window_wait_placed(lh_os_system_posix_window_t *self)
{
    const lh_u64_t until =
        lh_os_tick_us() + lh_cast_static(lh_u64_t, LH_OS_SYSTEM_POSIX_WINDOW_MAP_WAIT_MS) * 1000U;
    lh_os_system_posix_xevent_t event;
    lh_bool_t mapped = lh_bool_false;
    struct pollfd watch;

    watch.fd = lh_os_system_posix_x->XConnectionNumber(lh_os_system_posix_display);
    watch.events = POLLIN;
    for (;;)
    {
        const lh_u64_t now = lh_os_tick_us();

        if (!mapped && lh_os_system_posix_x->XCheckTypedWindowEvent(
                           lh_os_system_posix_display, self->window,
                           LH_OS_SYSTEM_POSIX_X_MAP_NOTIFY, lh_addr_of(event)))
        {
            mapped = lh_bool_true;
        }
        lh_return_if(mapped && lh_os_system_posix_window_is_placed(self));
        lh_return_if(now >= until);
        /* Asked again every few milliseconds once mapped: the move that places it is
           the window manager's, and it does not always tell the window. */
        watch.revents = 0;
        (void)poll(lh_addr_of(watch), 1,
                   mapped ? 5 : lh_cast_static(int, ((until - now) / 1000U)) + 1);
    }
}

lh_os_system_window_handle_t
lh_os_system_window_open(const lh_char_t *title, int width, int height, lh_ptr user,
                         lh_os_system_window_handle_t owner, lh_os_window_frame_t frame, int corner,
                         lh_os_window_placement_t placement)
{
    const lh_os_system_posix_x11_t *x;
    lh_os_system_posix_window_t *self;
    lh_os_system_posix_window_t *owner_window;
    lh_os_system_posix_xset_window_attributes_t attributes;
    lh_os_system_posix_xsize_hints_t hints;
    lh_os_system_posix_xid_t protocols[1];
    lh_os_system_posix_xid_t root;
    lh_os_system_posix_xvisual_info_t info;
    const lh_os_system_posix_xrender_t *render;
    const unsigned long mask = LH_OS_SYSTEM_POSIX_X_CW_BACK_PIXMAP |
                               LH_OS_SYSTEM_POSIX_X_CW_BIT_GRAVITY |
                               LH_OS_SYSTEM_POSIX_X_CW_EVENT_MASK;
    int event_base = 0;
    int error_base = 0;
    unsigned int got_width = 0U;
    unsigned int got_height = 0U;
    unsigned int border = 0U;
    unsigned int depth = 0U;
    int at_x = 0;
    int at_y = 0;
    lh_usize_t title_length = 0U;

    lh_assert_runtime_ref(title);
    lh_return_if(width <= 0 || height <= 0, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    lh_return_if(!lh_os_system_posix_connect(), LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    x = lh_os_system_posix_x;
    owner_window = lh_os_system_posix_window_find(owner);

    self = lh_ptr_rcast(lh_os_system_posix_window_t, lh_os_alloc(sizeof(lh_ptr_deref(self))));
    lh_return_if(lh_null_eq(self), LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    lh_memory_set(self, sizeof(lh_ptr_deref(self)), 0);
    self->user = lh_ptr_rcast(lh_os_window_t, user);
    self->owner = owner_window;
    self->width = width;
    self->height = height;
    self->enabled = lh_bool_true;
    self->corner = corner > 0 ? corner : 0;

    if (placement == lh_os_window_placement_center)
    {
        int area_x;
        int area_y;
        int area_width;
        int area_height;

        /* The middle of what the desktop leaves usable, and the top-left corner
           kept inside it, so that a window bigger than the screen still has its
           title where it can be grabbed -- the Windows backend's two rules. */
        lh_os_system_posix_workarea(lh_addr_of(area_x), lh_addr_of(area_y), lh_addr_of(area_width),
                                    lh_addr_of(area_height));
        at_x = area_x + (area_width - width) / 2;
        at_y = area_y + (area_height - height) / 2;
        at_x = at_x > area_x ? at_x : area_x;
        at_y = at_y > area_y ? at_y : area_y;
    }

    lh_memory_set(lh_addr_of(attributes), sizeof(attributes), 0);
    /* No background: the server would clear every exposed area to it before the
       paint, which is a flash of it on every resize. The app paints all of it. */
    attributes.background_pixmap = LH_OS_SYSTEM_POSIX_X_NONE;
    attributes.bit_gravity = LH_OS_SYSTEM_POSIX_X_NORTH_WEST_GRAVITY;
    attributes.event_mask =
        LH_OS_SYSTEM_POSIX_X_KEY_PRESS_MASK | LH_OS_SYSTEM_POSIX_X_KEY_RELEASE_MASK |
        LH_OS_SYSTEM_POSIX_X_BUTTON_PRESS_MASK | LH_OS_SYSTEM_POSIX_X_BUTTON_RELEASE_MASK |
        LH_OS_SYSTEM_POSIX_X_POINTER_MOTION_MASK | LH_OS_SYSTEM_POSIX_X_EXPOSURE_MASK |
        LH_OS_SYSTEM_POSIX_X_STRUCTURE_NOTIFY_MASK | LH_OS_SYSTEM_POSIX_X_PROPERTY_CHANGE_MASK |
        LH_OS_SYSTEM_POSIX_X_FOCUS_CHANGE_MASK;
    render = lh_os_system_posix_xrender_load();
    lh_memory_set(lh_addr_of(info), sizeof(info), 0);
    if (lh_null_ne(render) &&
        render->XRenderQueryExtension(lh_os_system_posix_display, lh_addr_of(event_base),
                                      lh_addr_of(error_base)) != 0 &&
        x->XMatchVisualInfo(lh_os_system_posix_display, lh_os_system_posix_screen, 32,
                            LH_OS_SYSTEM_POSIX_X_TRUE_COLOR, lh_addr_of(info)) != 0)
    {
        /* Every window is a 32-bit one where it can be, not only one that has round
           corners when it opens: the corners are the window's to change at any
           time, and the visual is not. A visual other than the root's needs a
           colormap and a border pixel of its own, or the server refuses the window. */
        self->colormap = x->XCreateColormap(lh_os_system_posix_display, lh_os_system_posix_root,
                                            info.visual, LH_OS_SYSTEM_POSIX_X_ALLOC_NONE);
        attributes.border_pixel = 0UL;
        attributes.colormap = self->colormap;
        self->window = x->XCreateWindow(
            lh_os_system_posix_display, lh_os_system_posix_root, at_x, at_y,
            lh_cast_static(unsigned int, width), lh_cast_static(unsigned int, height), 0U, 32,
            LH_OS_SYSTEM_POSIX_X_INPUT_OUTPUT, info.visual,
            mask | LH_OS_SYSTEM_POSIX_X_CW_BORDER_PIXEL | LH_OS_SYSTEM_POSIX_X_CW_COLORMAP,
            lh_addr_of(attributes));
        if (self->window != LH_OS_SYSTEM_POSIX_X_NONE)
        {
            self->window_picture = render->XRenderCreatePicture(
                lh_os_system_posix_display, self->window,
                render->XRenderFindVisualFormat(lh_os_system_posix_display, info.visual), 0UL,
                lh_null);
            lh_os_system_posix_window_back(self);
        }
        else
        {
            (void)x->XFreeColormap(lh_os_system_posix_display, self->colormap);
            self->colormap = LH_OS_SYSTEM_POSIX_X_NONE;
        }
    }
    if (self->window == LH_OS_SYSTEM_POSIX_X_NONE)
    {
        /* No 32-bit visual or no RENDER: the screen's own visual, drawn into as it
           is. Round corners are then cut by SHAPE alone, and a compositor shows
           them black. */
        self->window = x->XCreateWindow(
            lh_os_system_posix_display, lh_os_system_posix_root, at_x, at_y,
            lh_cast_static(unsigned int, width), lh_cast_static(unsigned int, height), 0U,
            LH_OS_SYSTEM_POSIX_X_COPY_FROM_PARENT, LH_OS_SYSTEM_POSIX_X_INPUT_OUTPUT, lh_null, mask,
            lh_addr_of(attributes));
    }
    if (self->window == LH_OS_SYSTEM_POSIX_X_NONE)
    {
        lh_runtime_allocator_free(self);
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }
    /* On whatever a paint draws into; a GC is bound to a depth, not to a drawable,
       so the one made on the first backing pixmap serves every one after it. */
    self->gc = x->XCreateGC(
        lh_os_system_posix_display,
        self->backing != LH_OS_SYSTEM_POSIX_X_NONE ? self->backing : self->window, 0UL, lh_null);

    /* The title twice: WM_NAME for the window managers that read only that, and
       _NET_WM_NAME in UTF-8 for every current one -- WM_NAME is Latin-1, and a
       Russian title in it is not a Russian title. */
    while (title[title_length] != '\0')
    {
        ++title_length;
    }
    (void)x->XStoreName(lh_os_system_posix_display, self->window, title);
    (void)x->XChangeProperty(
        lh_os_system_posix_display, self->window, lh_os_system_posix_atom_net_wm_name,
        lh_os_system_posix_atom_utf8_string, 8, LH_OS_SYSTEM_POSIX_X_PROP_MODE_REPLACE,
        lh_ptr_rcast(const lh_byte_t, title), lh_cast_static(int, title_length));

    /* The close button sends a message instead of killing the connection. */
    protocols[0] = lh_os_system_posix_atom_wm_delete_window;
    (void)x->XSetWMProtocols(lh_os_system_posix_display, self->window, protocols, 1);

    if (placement == lh_os_window_placement_center)
    {
        /* Without this the window manager places the window itself and the
           position given to XCreateWindow is ignored. */
        lh_memory_set(lh_addr_of(hints), sizeof(hints), 0);
        hints.flags = LH_OS_SYSTEM_POSIX_X_US_POSITION | LH_OS_SYSTEM_POSIX_X_P_POSITION;
        hints.x = at_x;
        hints.y = at_y;
        hints.width = width;
        hints.height = height;
        x->XSetWMNormalHints(lh_os_system_posix_display, self->window, lh_addr_of(hints));
    }
    if (lh_null_ne(owner_window))
    {
        (void)x->XSetTransientForHint(lh_os_system_posix_display, self->window,
                                      owner_window->window);
    }
    if (frame == lh_os_window_frame_own)
    {
        /* Motif's hints are the ones every window manager reads to leave the frame
           off: flags = decorations, decorations = none. */
        const long motif[5] = {2L, 0L, 0L, 0L, 0L};

        (void)x->XChangeProperty(
            lh_os_system_posix_display, self->window, lh_os_system_posix_atom_motif_wm_hints,
            lh_os_system_posix_atom_motif_wm_hints, 32, LH_OS_SYSTEM_POSIX_X_PROP_MODE_REPLACE,
            lh_ptr_rcast(const lh_byte_t, motif), 5);
    }

    /* Listed before it is mapped: the events of its first moments are for it. And
       the portable window knows its handle before any callback reaches it, as it
       does on Windows from WM_NCCREATE. */
    self->next = lh_os_system_posix_windows;
    lh_os_system_posix_windows = self;
    if (lh_null_ne(self->user))
    {
        self->user->handle = self;
    }
    (void)x->XMapWindow(lh_os_system_posix_display, self->window);
    (void)x->XFlush(lh_os_system_posix_display);
    lh_os_system_posix_window_wait_placed(self);

    /* The size it was given, which the window manager may not have honoured. */
    if (x->XGetGeometry(lh_os_system_posix_display, self->window, lh_addr_of(root),
                        lh_addr_of(at_x), lh_addr_of(at_y), lh_addr_of(got_width),
                        lh_addr_of(got_height), lh_addr_of(border), lh_addr_of(depth)) != 0 &&
        got_width > 0U && got_height > 0U)
    {
        self->width = lh_cast_static(int, got_width);
        self->height = lh_cast_static(int, got_height);
        if (self->backing != LH_OS_SYSTEM_POSIX_X_NONE)
        {
            lh_os_system_posix_window_back(self);
        }
    }
    lh_os_system_posix_window_cut(self);
    if (lh_null_ne(self->user))
    {
        lh_os_window_on_native_resize(self->user, self->width, self->height, lh_bool_false);
        /* And painted before the open returns, as UpdateWindow paints it there. */
        lh_os_system_posix_window_damage(self, 0, 0, self->width, self->height);
        if (lh_null_ne(lh_os_system_posix_window_find(self)) && self->dirty)
        {
            lh_os_system_posix_window_paint(self);
        }
    }
    (void)x->XFlush(lh_os_system_posix_display);
    return self;
}

lh_void
lh_os_system_window_set_corner_radius(lh_os_system_window_handle_t handle, int radius)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);

    lh_return_if(lh_null_eq(self));
    self->corner = radius > 0 ? radius : 0;
    lh_os_system_posix_window_cut(self);
    (void)lh_os_system_posix_x->XFlush(lh_os_system_posix_display);
}

lh_void
lh_os_system_window_minimize(lh_os_system_window_handle_t handle)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);

    lh_return_if(lh_null_eq(self));
    (void)lh_os_system_posix_x->XIconifyWindow(lh_os_system_posix_display, self->window,
                                               lh_os_system_posix_screen);
    (void)lh_os_system_posix_x->XFlush(lh_os_system_posix_display);
}

lh_void
lh_os_system_window_set_maximized(lh_os_system_window_handle_t handle, lh_bool_t maximized)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);
    lh_os_system_posix_xevent_t event;

    lh_return_if(lh_null_eq(self));
    /* Asked of the window manager, which owns the window's state: the answer comes
       back as a change of _NET_WM_STATE and is reported from there. */
    lh_memory_set(lh_addr_of(event), sizeof(event), 0);
    event.client.type = LH_OS_SYSTEM_POSIX_X_CLIENT_MESSAGE;
    event.client.window = self->window;
    event.client.message_type = lh_os_system_posix_atom_net_wm_state;
    event.client.format = 32;
    event.client.data[0] = maximized ? 1L : 0L; /* _NET_WM_STATE_ADD / _REMOVE */
    event.client.data[1] = lh_cast_static(long, lh_os_system_posix_atom_net_wm_state_max_vert);
    event.client.data[2] = lh_cast_static(long, lh_os_system_posix_atom_net_wm_state_max_horz);
    event.client.data[3] = 1L;
    (void)lh_os_system_posix_x->XSendEvent(lh_os_system_posix_display, lh_os_system_posix_root, 0,
                                           LH_OS_SYSTEM_POSIX_X_SUBSTRUCTURE_REDIRECT_MASK |
                                               LH_OS_SYSTEM_POSIX_X_SUBSTRUCTURE_NOTIFY_MASK,
                                           lh_addr_of(event));
    (void)lh_os_system_posix_x->XFlush(lh_os_system_posix_display);
}

lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t handle)
{
    return lh_null_ne(handle) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_os_system_window_set_enabled(lh_os_system_window_handle_t handle, lh_bool_t enabled)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);

    lh_return_if(lh_null_eq(self));
    self->enabled = enabled;
}

lh_void
lh_os_system_window_close(lh_os_system_window_handle_t handle)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);
    lh_os_system_posix_window_t *owned;
    lh_os_window_t *user;

    lh_return_if(lh_null_eq(self));
    /* The windows it owns go first, as Windows destroys owned windows before their
       owner: a modal child must not outlive the window it is modal to. */
    for (;;)
    {
        for (owned = lh_os_system_posix_windows; lh_null_ne(owned); owned = owned->next)
        {
            if (owned->owner == self)
            {
                break;
            }
        }
        if (lh_null_eq(owned))
        {
            break;
        }
        lh_os_system_window_close(owned);
    }
    lh_os_system_posix_window_unlist(self);
    (void)lh_os_system_posix_x->XFreeGC(lh_os_system_posix_display, self->gc);
    if (self->backing != LH_OS_SYSTEM_POSIX_X_NONE)
    {
        const lh_os_system_posix_xrender_t *render = lh_os_system_posix_xrender_load();

        render->XRenderFreePicture(lh_os_system_posix_display, self->backing_picture);
        render->XRenderFreePicture(lh_os_system_posix_display, self->window_picture);
        (void)lh_os_system_posix_x->XFreePixmap(lh_os_system_posix_display, self->backing);
    }
    (void)lh_os_system_posix_x->XDestroyWindow(lh_os_system_posix_display, self->window);
    if (self->colormap != LH_OS_SYSTEM_POSIX_X_NONE)
    {
        (void)lh_os_system_posix_x->XFreeColormap(lh_os_system_posix_display, self->colormap);
    }
    (void)lh_os_system_posix_x->XFlush(lh_os_system_posix_display);
    /* Destroying is synchronous on Windows and ends in WM_DESTROY; the portable
       layer is told here, the same way and at the same point. */
    user = self->user;
    lh_runtime_allocator_free(self);
    if (lh_null_ne(user) && user->handle == handle)
    {
        lh_os_window_on_native_destroy(user);
    }
}

/* -- Input ----------------------------------------------------------------- */

/** The buttons held, from an X @p state and the side buttons this window last heard
   go down: X keeps a bit for the first three and none for the back and forward. */
static lh_u32_t
lh_os_system_posix_window_buttons_held(const lh_os_system_posix_window_t *self, lh_u32_t state)
{
    const lh_u32_t side = lh_os_input_button_to_mask(lh_os_input_button_back) |
                          lh_os_input_button_to_mask(lh_os_input_button_forward);

    return lh_os_system_posix_input_buttons_of(state) |
           (lh_os_window_get_pressed_buttons(self->user) & side);
}

/** A wheel notch as a wheel event; false, and nothing sent, for any other button. */
static lh_bool_t
lh_os_system_posix_window_send_wheel(lh_os_system_posix_window_t *self,
                                     const lh_os_system_posix_xevent_t *event)
{
    lh_os_input_event_t input;
    lh_s32_t dx;
    lh_s32_t dy;

    lh_return_if(
        !lh_os_system_posix_input_wheel_of(event->button.button, lh_addr_of(dx), lh_addr_of(dy)),
        lh_bool_false);
    lh_os_input_event_init_wheel(lh_addr_of(input), lh_os_input_coord_from_int(event->button.x),
                                 lh_os_input_coord_from_int(event->button.y), dx, dy,
                                 lh_os_tick_ms());
    lh_os_input_event_set_buttons(
        lh_addr_of(input), lh_os_system_posix_window_buttons_held(self, event->button.state));
    lh_os_input_event_set_mods(lh_addr_of(input),
                               lh_os_system_posix_input_mods_of(event->button.state));
    lh_os_window_on_native_input(self->user, lh_addr_of(input));
    return lh_bool_true;
}

/** A button going @p down or up. X's `state` is what was held *before* the event, so
   the button itself is added or taken away to say what is held after it. */
static lh_void
lh_os_system_posix_window_send_button(lh_os_system_posix_window_t *self,
                                      const lh_os_system_posix_xevent_t *event, lh_bool_t down)
{
    lh_os_input_event_t input;
    const lh_os_input_button_t button = lh_os_system_posix_input_button_of(event->button.button);
    const lh_u32_t mask = lh_os_input_button_to_mask(button);
    const lh_u32_t held = lh_os_system_posix_window_buttons_held(self, event->button.state);

    lh_os_input_event_init_pointer(
        lh_addr_of(input), down ? lh_os_input_kind_pointer_down : lh_os_input_kind_pointer_up,
        lh_os_input_device_kind_mouse, 0U, button, lh_os_input_coord_from_int(event->button.x),
        lh_os_input_coord_from_int(event->button.y), lh_os_tick_ms());
    lh_os_input_event_set_buttons(lh_addr_of(input), down ? held | mask : held & ~mask);
    lh_os_input_event_set_mods(lh_addr_of(input),
                               lh_os_system_posix_input_mods_of(event->button.state));
    lh_os_window_on_native_input(self->user, lh_addr_of(input));
}

/** A key going down or up, and the text a press types. The keycode goes along as the
   scancode, so a key ::lh_os_key_t has no name for can still be told apart. */
static lh_void
lh_os_system_posix_window_send_key(lh_os_system_posix_window_t *self,
                                   lh_os_system_posix_xevent_t *event)
{
    char text[8];
    lh_os_input_event_t input;
    lh_os_system_posix_xid_t keysym = LH_OS_SYSTEM_POSIX_X_NONE;
    const lh_u32_t keycode = lh_type_cast(lh_u32_t, event->key.keycode);
    const lh_os_input_mods_t mods = lh_os_system_posix_input_mods_of(event->key.state);
    const lh_bool_t pressed =
        event->type == LH_OS_SYSTEM_POSIX_X_KEY_PRESS ? lh_bool_true : lh_bool_false;
    lh_bool_t repeat = lh_bool_false;

    if (!pressed && lh_os_system_posix_is_repeat(event))
    {
        lh_os_system_posix_repeat_keycode = keycode;
        return;
    }
    if (pressed)
    {
        repeat = lh_type_cast(lh_bool_t, lh_os_system_posix_repeat_keycode == keycode);
    }
    lh_os_system_posix_repeat_keycode = 0U;
    /* The keysym with the layout and Shift applied: what the key means now. */
    (void)lh_os_system_posix_x->XLookupString(
        lh_addr_of(event->key), text, lh_type_cast(int, sizeof(text)), lh_addr_of(keysym), lh_null);
    lh_os_input_event_init_key(lh_addr_of(input), lh_os_system_posix_key_from_keysym(keysym),
                               pressed, repeat, keycode, lh_os_tick_ms());
    lh_os_input_event_set_mods(lh_addr_of(input), mods);
    lh_os_window_on_native_input(self->user, lh_addr_of(input));
    /* Text only from a press, and not from a chord: Ctrl+C is a command, and Windows
       sends no WM_CHAR for Alt+key either. The window may have gone in the sink, so
       it is looked up again. */
    if (pressed &&
        (event->key.state & (LH_OS_SYSTEM_POSIX_X_CONTROL_MASK | LH_OS_SYSTEM_POSIX_X_MOD1_MASK)) ==
            0U &&
        lh_null_ne(lh_os_system_posix_window_find_xid(event->any.window)))
    {
        const lh_u32_t code = lh_os_system_posix_text_from_keysym(keysym);

        if (code != 0U)
        {
            lh_os_input_event_init_text(lh_addr_of(input), code, lh_os_tick_ms());
            lh_os_input_event_set_mods(lh_addr_of(input), mods);
            lh_os_window_on_native_input(self->user, lh_addr_of(input));
        }
    }
}

/** Hand one X event to the window it is for. */
static lh_void
lh_os_system_posix_dispatch(lh_os_system_posix_xevent_t *event)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find_xid(event->any.window);

    lh_return_if(lh_null_eq(self));
    switch (event->type)
    {
    case LH_OS_SYSTEM_POSIX_X_EXPOSE:
        lh_os_system_posix_window_damage(self, event->expose.x, event->expose.y,
                                         event->expose.x + event->expose.width,
                                         event->expose.y + event->expose.height);
        return;
    case LH_OS_SYSTEM_POSIX_X_CONFIGURE_NOTIFY:
        if (event->configure.configured == self->window &&
            (event->configure.width != self->width || event->configure.height != self->height))
        {
            self->width = event->configure.width;
            self->height = event->configure.height;
            if (self->backing != LH_OS_SYSTEM_POSIX_X_NONE)
            {
                lh_os_system_posix_window_back(self);
            }
            /* A window of another size is drawn again whole, as a Windows class
               with CS_HREDRAW | CS_VREDRAW is. */
            lh_os_system_posix_window_damage(self, 0, 0, self->width, self->height);
            lh_os_system_posix_window_cut(self);
            lh_os_window_on_native_resize(self->user, self->width, self->height, self->maximized);
        }
        return;
    case LH_OS_SYSTEM_POSIX_X_PROPERTY_NOTIFY:
        if (event->property.atom == lh_os_system_posix_atom_net_wm_state)
        {
            const lh_bool_t maximized = lh_os_system_posix_window_read_maximized(self);

            if (maximized != self->maximized)
            {
                self->maximized = maximized;
                lh_os_system_posix_window_cut(self);
                lh_os_window_on_native_resize(self->user, self->width, self->height, maximized);
            }
        }
        return;
    case LH_OS_SYSTEM_POSIX_X_CLIENT_MESSAGE:
        if (event->client.message_type == lh_os_system_posix_atom_wm_protocols &&
            lh_cast_static(lh_os_system_posix_xid_t, event->client.data[0]) ==
                lh_os_system_posix_atom_wm_delete_window)
        {
            /* The close button of the window manager's frame. On Windows this is
               WM_CLOSE, which with no handler destroys the window -- and the
               portable layer learns about it the same way, from the destroy. */
            lh_os_system_window_close(self);
        }
        return;
    default:
        break;
    }

    /* Input. A window disabled for a modal child of its own gets none, as a
       disabled window on Windows gets none. */
    lh_return_if(!self->enabled);
    switch (event->type)
    {
    case LH_OS_SYSTEM_POSIX_X_BUTTON_PRESS:
        if (lh_os_system_posix_window_send_wheel(self, event))
        {
            return;
        }
        if (event->button.button == LH_OS_SYSTEM_POSIX_X_BUTTON_LEFT &&
            lh_os_system_posix_window_begin_moveresize(
                self, lh_os_window_zone_at(self->user, event->button.x, event->button.y),
                event->button.x_root, event->button.y_root))
        {
            return;
        }
        lh_os_system_posix_window_send_button(self, event, lh_bool_true);
        return;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_RELEASE:
        /* The wheel's release half says nothing a press did not. */
        if (lh_os_system_posix_input_button_of(event->button.button) != lh_os_input_button_none)
        {
            lh_os_system_posix_window_send_button(self, event, lh_bool_false);
        }
        return;
    case LH_OS_SYSTEM_POSIX_X_MOTION_NOTIFY:
    {
        lh_os_input_event_t input;

        lh_os_input_event_init_pointer(
            lh_addr_of(input), lh_os_input_kind_pointer_move, lh_os_input_device_kind_mouse, 0U,
            lh_os_input_button_none, lh_os_input_coord_from_int(event->motion.x),
            lh_os_input_coord_from_int(event->motion.y), lh_os_tick_ms());
        lh_os_input_event_set_buttons(
            lh_addr_of(input), lh_os_system_posix_window_buttons_held(self, event->motion.state));
        lh_os_input_event_set_mods(lh_addr_of(input),
                                   lh_os_system_posix_input_mods_of(event->motion.state));
        lh_os_window_on_native_input(self->user, lh_addr_of(input));
        return;
    }
    case LH_OS_SYSTEM_POSIX_X_KEY_PRESS:
    case LH_OS_SYSTEM_POSIX_X_KEY_RELEASE:
        lh_os_system_posix_window_send_key(self, event);
        return;
    case LH_OS_SYSTEM_POSIX_X_FOCUS_IN:
    case LH_OS_SYSTEM_POSIX_X_FOCUS_OUT:
    {
        lh_os_input_event_t input;

        /* A grab moves the focus for a moment without the window losing it, and a
           pointer detail is about the window under the mouse, not this one. */
        lh_return_if(event->focus.mode == LH_OS_SYSTEM_POSIX_X_NOTIFY_GRAB ||
                     event->focus.mode == LH_OS_SYSTEM_POSIX_X_NOTIFY_UNGRAB ||
                     event->focus.detail == LH_OS_SYSTEM_POSIX_X_NOTIFY_POINTER);
        lh_os_input_event_init(lh_addr_of(input),
                               event->type == LH_OS_SYSTEM_POSIX_X_FOCUS_IN
                                   ? lh_os_input_kind_focus_in
                                   : lh_os_input_kind_focus_out,
                               lh_os_input_device_kind_keyboard, lh_os_tick_ms());
        lh_os_window_on_native_input(self->user, lh_addr_of(input));
        return;
    }
    default:
        return;
    }
}

/** The first window that waits to be painted, the first whose tick is due. Asked
   afresh after every callback, because a callback can close any window, its own
   included, and a list walked across that is a list of freed memory. */
static lh_os_system_posix_window_t *
lh_os_system_posix_first_dirty(void)
{
    lh_os_system_posix_window_t *window;

    for (window = lh_os_system_posix_windows; lh_null_ne(window); window = window->next)
    {
        lh_return_if(window->dirty, window);
    }
    return lh_null;
}

/** The first window whose tick is due at @p now, or ::lh_null. */
static lh_os_system_posix_window_t *
lh_os_system_posix_first_due(lh_u64_t now)
{
    lh_os_system_posix_window_t *window;

    for (window = lh_os_system_posix_windows; lh_null_ne(window); window = window->next)
    {
        lh_return_if(window->tick_ms != 0U && window->tick_due_us <= now, window);
    }
    return lh_null;
}

/** Milliseconds until the earliest tick, or -1 for none. */
static int
lh_os_system_posix_until_tick(lh_u64_t now)
{
    const lh_os_system_posix_window_t *window;
    lh_u64_t soonest = 0U;
    lh_bool_t any = lh_bool_false;

    for (window = lh_os_system_posix_windows; lh_null_ne(window); window = window->next)
    {
        if (window->tick_ms != 0U && (!any || window->tick_due_us < soonest))
        {
            soonest = window->tick_due_us;
            any = lh_bool_true;
        }
    }
    lh_return_if(!any, -1);
    lh_return_if(soonest <= now, 0);
    /* Rounded up: waking a millisecond early only to find nothing due is a spin. */
    return (int)((soonest - now + 999U) / 1000U);
}

lh_os_system_window_pump_result_t
lh_os_system_window_pump_wait(lh_tick_t timeout_ms)
{
    const lh_os_system_posix_x11_t *x = lh_os_system_posix_x;
    lh_bool_t handled = lh_bool_false;
    lh_os_system_posix_window_t *window;
    lh_os_system_posix_xevent_t event;
    struct pollfd watch;
    lh_u64_t now;
    int wait;
    int tick;

    if (lh_os_system_posix_quit)
    {
        lh_os_system_posix_quit = lh_bool_false;
        return lh_os_system_window_pump_quit;
    }
    wait = timeout_ms == LH_TICK_T_MAX ? -1 : lh_cast_static(int, timeout_ms);
    if (lh_null_eq(lh_os_system_posix_display))
    {
        /* Nothing to wait on but the time. */
        (void)poll(lh_null, 0, wait);
        return lh_os_system_window_pump_timeout;
    }

    now = lh_os_tick_us();
    tick = lh_os_system_posix_until_tick(now);
    if (tick >= 0 && (wait < 0 || tick < wait))
    {
        wait = tick;
    }
    /* XPending also sends what is buffered, so a request made in the last pass is
       on its way before this waits. Something already queued, or a paint still
       owed, is no time to sleep. */
    if (x->XPending(lh_os_system_posix_display) > 0 || lh_null_ne(lh_os_system_posix_first_dirty()))
    {
        wait = 0;
    }
    if (wait != 0)
    {
        watch.fd = x->XConnectionNumber(lh_os_system_posix_display);
        watch.events = POLLIN;
        watch.revents = 0;
        (void)poll(lh_addr_of(watch), 1, wait);
    }

    while (x->XPending(lh_os_system_posix_display) > 0)
    {
        (void)x->XNextEvent(lh_os_system_posix_display, lh_addr_of(event));
        lh_os_system_posix_dispatch(lh_addr_of(event));
        handled = lh_bool_true;
    }

    now = lh_os_tick_us();
    for (window = lh_os_system_posix_first_due(now); lh_null_ne(window);
         window = lh_os_system_posix_first_due(now))
    {
        /* From now, not from when it was due: a tick that is late does not come
           back twice to catch up, which a busy UI would only fall further behind on. */
        window->tick_due_us = now + lh_cast_static(lh_u64_t, window->tick_ms) * 1000U;
        lh_os_window_on_native_tick(window->user);
        handled = lh_bool_true;
    }
    for (window = lh_os_system_posix_first_dirty(); lh_null_ne(window);
         window = lh_os_system_posix_first_dirty())
    {
        lh_os_system_posix_window_paint(window);
        handled = lh_bool_true;
    }
    (void)x->XFlush(lh_os_system_posix_display);

    if (lh_os_system_posix_quit)
    {
        lh_os_system_posix_quit = lh_bool_false;
        return lh_os_system_window_pump_quit;
    }
    return handled ? lh_os_system_window_pump_message : lh_os_system_window_pump_timeout;
}

lh_void
lh_os_system_window_post_quit(void)
{
    /* Read by the pump on its way out, or at the start of the next one: the quit is
       always posted from inside a callback the pump is running, on its thread. */
    lh_os_system_posix_quit = lh_bool_true;
}

lh_void
lh_os_system_window_invalidate(lh_os_system_window_handle_t handle)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);

    lh_return_if(lh_null_eq(self));
    lh_os_system_posix_window_damage(self, 0, 0, self->width, self->height);
}

lh_void
lh_os_system_window_invalidate_rect(lh_os_system_window_handle_t handle, int left, int top,
                                    int right, int bottom)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);

    lh_return_if(lh_null_eq(self));
    lh_os_system_posix_window_damage(self, left, top, right, bottom);
}

lh_void
lh_os_system_window_set_timer(lh_os_system_window_handle_t handle, lh_u32_t ms)
{
    lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);

    lh_return_if(lh_null_eq(self));
    self->tick_ms = ms;
    self->tick_due_us = lh_os_tick_us() + lh_cast_static(lh_u64_t, ms) * 1000U;
}

lh_bool_t
lh_os_system_window_get_client_size(lh_os_system_window_handle_t handle, int *width, int *height)
{
    const lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);

    lh_return_if(lh_null_eq(self) || lh_null_eq(width) || lh_null_eq(height), lh_bool_false);
    lh_ptr_deref(width) = self->width;
    lh_ptr_deref(height) = self->height;
    return lh_bool_true;
}

lh_bool_t
lh_os_system_window_get_paint_region(lh_ptr paint_dc, lh_u64_t *area, lh_u32_t *count)
{
    const lh_os_system_posix_paint_t *paint =
        lh_ptr_rcast(const lh_os_system_posix_paint_t, paint_dc);

    lh_return_if(lh_null_eq(area) || lh_null_eq(count), lh_bool_false);
    lh_ptr_deref(area) = 0U;
    lh_ptr_deref(count) = 0U;
    lh_return_if(lh_null_eq(paint), lh_bool_false);
    /* One rectangle, the bounding box: that is all this backend keeps. */
    lh_ptr_deref(area) = paint->area;
    lh_ptr_deref(count) = paint->rects;
    return lh_bool_true;
}

lh_bool_t
lh_os_system_window_get_position(lh_os_system_window_handle_t handle, int *x, int *y)
{
    const lh_os_system_posix_window_t *self = lh_os_system_posix_window_find(handle);
    lh_os_system_posix_xid_t child = LH_OS_SYSTEM_POSIX_X_NONE;

    lh_return_if(lh_null_eq(self) || lh_null_eq(x) || lh_null_eq(y), lh_bool_false);
    /* Where its top-left pixel is on the screen. A window manager's frame puts the
       window inside a window of its own, so the position X keeps is relative to
       that and says nothing about the screen. */
    return lh_os_system_posix_x->XTranslateCoordinates(lh_os_system_posix_display, self->window,
                                                       lh_os_system_posix_root, 0, 0, x, y,
                                                       lh_addr_of(child)) != 0
               ? lh_bool_true
               : lh_bool_false;
}
