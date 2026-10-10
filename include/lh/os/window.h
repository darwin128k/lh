/**
 * @file window.h
 * @brief A native OS window — ::lh_os_window_t.
 *
 * Top-level windows are opened into an ::lh_os_app_t from outside; the first
 * linked window is main (index 0). A window may own modal children. Set
 * ::lh_os_window_set_on_close to learn when the native window dies — including
 * when the user closes it (reason ::lh_os_window_close_reason_os).
 *
 * The message pump lives on ::lh_os_app_run, not on the window.
 *
 * A caller can style the window instead of taking the OS frame, and none of it is
 * hardcoded here: ::lh_os_window_set_frame says whose frame it is,
 * ::lh_os_window_set_corner_radius cuts its corners, and
 * ::lh_os_window_set_on_zone answers point by point what the window system should do
 * — move it, resize it, or leave the press to the app. What that looks like is the
 * caller's, so a project draws and hit-tests its own title bar and its own buttons.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW (itself requires ::LH_LIBRARY_OPTION_OS).
 */

#ifndef LH_OS_WINDOW_H
#define LH_OS_WINDOW_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/window/handle.h>
#include <lh/os/window/close/reason.h>
#include <lh/os/window/fields.h>
#include <lh/os/window/frame.h>
#include <lh/os/window/on/click/cb.h>
#include <lh/os/window/on/close/cb.h>
#include <lh/os/window/on/move/cb.h>
#include <lh/os/window/on/paint/cb.h>
#include <lh/os/window/on/press/cb.h>
#include <lh/os/window/on/release/cb.h>
#include <lh/os/window/on/resize/cb.h>
#include <lh/os/window/on/zone/cb.h>
#include <lh/os/window/on/wheel/cb.h>
#include <lh/os/window/zone.h>
#include <lh/ptr.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/window.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/window.h requires LH_LIBRARY_OPTION_OS_WINDOW (CMake: -DLH_LIBRARY_OPTION_OS_WINDOW=ON)"
#endif

struct lh_os_app;

/**
 * @struct lh_os_window
 * @typedef lh_os_window_t
 * @brief One native window: handle, app or parent, and modal children.
 */
struct lh_os_window
{
    lh_os_window_fields(lh_os_system_window_handle_t, struct lh_os_app, struct lh_os_window);
};
typedef struct lh_os_window lh_os_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set @p self to the empty (not-yet-open) state.
 *
 * Does not touch the OS.
 */
lh_void
lh_os_window_init(lh_os_window_t *self);

/**
 * @brief Draw @p self's frame and chrome instead of the OS's.
 *
 * Call before ::lh_os_window_open — what it says is read at creation time.
 * ::lh_os_window_frame_own creates the window with no OS frame at all: its client is
 * the whole window, so the width and height given to open are the client size with
 * nothing subtracted for a frame. Everything that was drawn on that frame is now the
 * app's to paint and the app's to hit test; tell the window system which of it moves
 * and resizes the window with ::lh_os_window_set_on_zone.
 *
 * The default, ::lh_os_window_frame_system, is the ordinary OS frame with its
 * caption, menu and resize borders, and needs nothing else set.
 */
lh_void
lh_os_window_set_frame(lh_os_window_t *self, lh_os_window_frame_t frame);

/**
 * @brief Cut @p self's corners to a rounded region of @p radius pixels.
 *
 * The corners are clipped away, not painted over: the window is cut to a rounded
 * region, which is the only way to round a window on a system with no compositor
 * behind it. Works with either frame and may be set before or after opening; `0` or
 * less is square.
 *
 * @p radius is in pixels and is clamped by the window system to what fits.
 */
lh_void
lh_os_window_set_corner_radius(lh_os_window_t *self, int radius);

/**
 * @brief Say where @p self opens for the first time.
 *
 * Call before ::lh_os_window_open, like ::lh_os_window_set_frame: what it says is
 * read at creation. The default, ::lh_os_window_placement_default, is the window
 * system's own choice, which on Win32 is a cascade from the top-left corner and not
 * where a user expects a window to be.
 *
 * ::lh_os_window_placement_center puts it in the middle of the work area of the
 * monitor it opens on — a modal child on the monitor its owner is on — and keeps
 * the title reachable when the window is larger than that area.
 *
 * This is where a window *opens*. Moving one that is already up is the window
 * system's drag, and ::lh_os_window_set_maximized restores it to the rectangle it
 * was opened at.
 */
lh_void
lh_os_window_set_placement(lh_os_window_t *self, lh_os_window_placement_t placement);

/**
 * @brief Where @p self opens, or ::lh_os_window_placement_default.
 */
lh_os_window_placement_t
lh_os_window_get_placement(const lh_os_window_t *self);

/**
 * @brief Where @p self is on the screen, into @p x and @p y: the top-left corner in
 *        screen coordinates, which is what ::lh_os_window_set_placement counts in.
 *
 * The window system may answer differently from what placement asked for — a
 * window dragged somewhere, or restored from a maximize — so this is the position,
 * not the request.
 *
 * @return True when the position was read.
 */
lh_bool_t
lh_os_window_get_position(const lh_os_window_t *self, int *x, int *y);

/**
 * @brief Name what the window system should do with each point of @p self.
 *
 * The one hook for styling a window's behaviour around its content, and the reason
 * none of it is hardcoded here: only the caller knows where its own chrome is. Answer
 * ::lh_os_window_zone_caption over a strip it drew itself and a press there moves the
 * window, with the OS's own move loop, its rules about the screen edges and the right
 * cursor; answer one of the edge and corner zones over a border and the OS resizes
 * the window; answer ::lh_os_window_zone_client — the default, and the answer when no
 * callback is set — and the press reaches the app as an ordinary one, so a button the
 * app drew on its own title bar keeps working.
 *
 * Set at any time: it is consulted per press and per pointer move, not read once.
 */
lh_void
lh_os_window_set_on_zone(lh_os_window_t *self, lh_os_window_on_zone_cb on_zone, lh_ptr context);

/**
 * @brief Minimize, maximize or restore @p self, the way its own frame's buttons do.
 *
 * These are here because a window with a frame of its own has no system buttons to
 * press: `minimize` sends it to the taskbar in its minimized state, `set_maximized`
 * fills the screen and puts it back, and the restore is the same call with the other
 * argument. A maximized window answers ::lh_os_window_is_maximized so a button knows
 * which way round to draw itself.
 *
 * A no-op when the window is closed. A maximized window is resized by the window
 * system, which is why ::lh_os_window_set_on_resize exists.
 */
lh_void
lh_os_window_minimize(lh_os_window_t *self);
lh_void
lh_os_window_set_maximized(lh_os_window_t *self, lh_bool_t maximized);

/**
 * @brief True while @p self is maximized.
 */
lh_bool_t
lh_os_window_is_maximized(const lh_os_window_t *self);

/**
 * @brief Get @p self's client area size, or false when it is closed.
 *
 * The same two numbers ::lh_os_window_set_on_resize hands over, for a caller that
 * wants them once instead of waiting to be told.
 */
lh_bool_t
lh_os_window_get_client_size(const lh_os_window_t *self, int *width, int *height);

/**
 * @brief Set what ::lh_os_window_set_on_resize tells @p self about its size.
 *
 * Without it a caller cannot tell a window apart from one whose size simply never
 * changes, which matters as soon as the window can be resized: a drag of one of the
 * zones named through ::lh_os_window_set_on_zone, or the window system's own
 * maximize, and neither announces itself through a paint.
 */
lh_void
lh_os_window_set_on_resize(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize, lh_ptr context);

/**
 * @brief Open @p self as a top-level window under @p app and show it.
 *
 * Links @p self at the end of @p app's list. The first window opened is main
 * (index 0). Fails when already open or already linked.
 */
lh_bool_t
lh_os_window_open(struct lh_os_app *app, lh_os_window_t *self, const lh_char_t *title, int width,
                  int height);

/**
 * @brief Open @p self as a modal child of @p parent and show it.
 *
 * Disables @p parent until @p self closes. Links into @p parent's children.
 */
lh_bool_t
lh_os_window_open_modal(lh_os_window_t *parent, lh_os_window_t *self, const lh_char_t *title,
                        int width, int height);

/**
 * @brief True while @p self still has a live native window.
 */
lh_bool_t
lh_os_window_is_open(const lh_os_window_t *self);

/**
 * @brief Raw handle, or ::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID when closed.
 */
lh_os_system_window_handle_t
lh_os_window_get_handle(const lh_os_window_t *self);

/**
 * @brief Owning app when top-level, or ::lh_null.
 */
struct lh_os_app *
lh_os_window_get_app(const lh_os_window_t *self);

/**
 * @brief Parent when a modal child, or ::lh_null.
 */
lh_os_window_t *
lh_os_window_get_parent(const lh_os_window_t *self);

/**
 * @brief First modal child, or ::lh_null.
 */
lh_os_window_t *
lh_os_window_get_first_child(const lh_os_window_t *self);

/**
 * @brief Modal child after @p child, or ::lh_null.
 */
lh_os_window_t *
lh_os_window_get_next_child(const lh_os_window_t *self, const lh_os_window_t *child);

/**
 * @brief Notify @p on_close when the native window is destroyed.
 *
 * @p context is passed through. ::lh_null clears the slot. Fires for both
 * ::lh_os_window_close_reason_api and ::lh_os_window_close_reason_os.
 */
lh_void
lh_os_window_set_on_close(lh_os_window_t *self, lh_os_window_on_close_cb on_close, lh_ptr context);

/**
 * @brief Notify @p on_paint when the native window needs a redraw.
 *
 * @p context is passed through. ::lh_null clears the slot. While the
 * callback runs, ::lh_os_window_get_paint_dc returns the platform paint DC.
 */
lh_void
lh_os_window_set_on_paint(lh_os_window_t *self, lh_os_window_on_paint_cb on_paint, lh_ptr context);

/**
 * @brief Notify @p on_press on a primary-button press in client coordinates.
 *
 * @p context is passed through. ::lh_null clears the slot.
 */
lh_void
lh_os_window_set_on_press(lh_os_window_t *self, lh_os_window_on_press_cb on_press, lh_ptr context);

/**
 * @brief Notify @p on_move on pointer move in client coordinates.
 *
 * @p context is passed through. ::lh_null clears the slot.
 */
lh_void
lh_os_window_set_on_move(lh_os_window_t *self, lh_os_window_on_move_cb on_move, lh_ptr context);

/**
 * @brief Notify @p on_release on a primary-button release in client coordinates.
 *
 * @p context is passed through. ::lh_null clears the slot. Click synthesis
 * (press then release without a drag) is the caller's job — the Win32
 * backend does not also fire ::lh_os_window_set_on_click.
 */
lh_void
lh_os_window_set_on_release(lh_os_window_t *self, lh_os_window_on_release_cb on_release,
                            lh_ptr context);

/**
 * @brief Notify @p on_wheel on a mouse-wheel tick in client coordinates.
 *
 * @p context is passed through. ::lh_null clears the slot. The callback gets
 * notches per axis (::lh_os_window_on_wheel_fn).
 */
lh_void
lh_os_window_set_on_wheel(lh_os_window_t *self, lh_os_window_on_wheel_cb on_wheel, lh_ptr context);

/**
 * @brief Notify @p on_key when a key goes down or up (::lh_os_window_on_key_fn).
 *        ::lh_null clears the slot.
 */
lh_void
lh_os_window_set_on_key(lh_os_window_t *self, lh_os_window_on_key_cb on_key, lh_ptr context);

/**
 * @brief Notify @p on_text with each typed character (::lh_os_window_on_text_fn).
 *        ::lh_null clears the slot.
 */
lh_void
lh_os_window_set_on_text(lh_os_window_t *self, lh_os_window_on_text_cb on_text, lh_ptr context);

/**
 * @brief Notify @p on_click on a primary-button click in client coordinates.
 *
 * Kept for callers that still wire a click; the Win32 backend does not fire
 * it (use press/release and synthesize above). @p context is passed through.
 * ::lh_null clears the slot.
 */
lh_void
lh_os_window_set_on_click(lh_os_window_t *self, lh_os_window_on_click_cb on_click, lh_ptr context);

/**
 * @brief Notify @p on_tick every @p ms (::lh_os_window_on_tick_fn), or @p ms `0` to
 *        stop.
 *
 * This exists because of a way an app that polls something goes wrong without it. The
 * only thing a window gets back on its own is a **paint**, so the poll was run inside
 * the paint and its answer -- "I still have requests due" -- was thrown away. The
 * device was asked three questions and then went quiet, with a status line saying it had
 * answered everything. The obvious repair is to invalidate when the poll is not done,
 * and that is a **spin**: the next paint comes back at once, the gap between two
 * questions to the same device has not elapsed, the poll sends nothing and says it is
 * still busy, and 219 requests turn into 219 requests times the gap in wasted frames at
 * full CPU. A timer is what turns "ask for the next frame" into "come back at 20 ms".
 *
 * @p ms below the window system's minimum is raised to it rather than refused: a caller
 * that asks for 1 ms on a system whose timer resolution is 15.6 ms asked for as fast as
 * it can, and refusing turns a too-fast request into no timer at all.
 *
 * Works before or after opening; the timer is armed when the window is. @p on_tick
 * ::lh_null stops the timer.
 */
lh_void
lh_os_window_set_on_tick(lh_os_window_t *self, lh_os_window_on_tick_cb on_tick, lh_ptr context,
                         lh_u32_t ms);

/**
 * @brief Platform paint DC for the current ::lh_os_window_on_paint_fn, or
 *        ::lh_null outside a paint cycle.
 *
 * On Win32 this is an `HDC` from `BeginPaint`.
 */
lh_ptr
lh_os_window_get_paint_dc(const lh_os_window_t *self);

/**
 * @brief Client paint rectangle for the current paint cycle (right/bottom
 *        exclusive), or zeros outside a paint.
 */
lh_void
lh_os_window_get_paint_rect(const lh_os_window_t *self, int *left, int *top, int *right, int *bottom);

/**
 * @brief Ask the OS to redraw @p self (posts a paint of the whole client).
 */
lh_void
lh_os_window_invalidate(lh_os_window_t *self);

/**
 * @brief Ask the OS to redraw a client rectangle of @p self (right/bottom
 *        exclusive).
 */
lh_void
lh_os_window_invalidate_rect(lh_os_window_t *self, int left, int top, int right, int bottom);

/**
 * @brief Close children, then destroy the native window and unlink @p self.
 */
lh_void
lh_os_window_close(lh_os_window_t *self);

/**
 * @brief Close if needed and clear @p self.
 */
lh_void
lh_os_window_deinit(lh_os_window_t *self);

/**
 * @brief Called from the native backend when the OS destroys the window.
 *
 * Invokes the on-close callback, clears the handle, re-enables a modal parent,
 * and unlinks from app or parent.
 */
lh_void
lh_os_window_on_native_destroy(lh_os_window_t *self);

/**
 * @brief Called from the native backend when the window's timer goes off.
 *
 * Invokes the on-tick callback if there is one. The native side owns the schedule; this
 * is only the delivery, and it is a no-op for a window with no callback rather than a
 * crash -- a window whose timer was stopped by the window system, or one that was never
 * given a period, still gets messages.
 */
lh_void
lh_os_window_on_native_tick(lh_os_window_t *self);

/**
 * @brief Called from the native backend inside a paint cycle with @p paint_dc
 *        and the update rectangle set.
 */
lh_void
lh_os_window_on_native_paint(lh_os_window_t *self, lh_ptr paint_dc, int left, int top, int right,
                             int bottom);

/**
 * @brief What the window system should do with the client point (@p x, @p y).
 *
 * The backend's way of asking ::lh_os_window_set_on_zone, so the answer is made in
 * one place whatever the system asks for. ::lh_os_window_zone_client when no callback
 * is set. @p x and @p y are client-area coordinates.
 */
lh_os_window_zone_t
lh_os_window_zone_at(lh_os_window_t *self, int x, int y);

/**
 * @brief Called from the native backend after the client area changed.
 *
 * Records the maximized state and then fires ::lh_os_window_set_on_resize, so both
 * facts a caller needs about a size change arrive the same way.
 *
 * An empty client (either dimension zero) records the maximized state but fires
 * nothing: a window that has just been created sends its size before creation has
 * applied the one it was asked for, and a minimized window has no client at all.
 * Neither is a size to lay out for; the next message carrying a real one is.
 */
lh_void
lh_os_window_on_native_resize(lh_os_window_t *self, int width, int height, lh_bool_t maximized);

/**
 * @brief Called from the native backend on a primary-button press.
 *
 * @p x and @p y are client-area coordinates.
 */
lh_void
lh_os_window_on_native_press(lh_os_window_t *self, int x, int y);

/**
 * @brief Called from the native backend on pointer move.
 *
 * @p x and @p y are client-area coordinates.
 */
lh_void
lh_os_window_on_native_move(lh_os_window_t *self, int x, int y);

/**
 * @brief Called from the native backend on a primary-button release.
 *
 * @p x and @p y are client-area coordinates.
 */
lh_void
lh_os_window_on_native_release(lh_os_window_t *self, int x, int y);

/**
 * @brief Called from the native backend on a mouse-wheel tick.
 *
 * @p x and @p y are client-area coordinates. @p dx / @p dy are notches.
 */
lh_void
lh_os_window_on_native_wheel(lh_os_window_t *self, int x, int y, int dx, int dy);

/**
 * @brief Called from the native backend when @p key goes down or up.
 */
lh_void
lh_os_window_on_native_key(lh_os_window_t *self, lh_key_t key, lh_bool_t pressed);

/**
 * @brief Called from the native backend with one typed code point.
 */
lh_void
lh_os_window_on_native_text(lh_os_window_t *self, lh_u32_t code);

/**
 * @brief Called from the native backend on a primary-button click.
 *
 * @p x and @p y are client-area coordinates. Kept for tests and callers that
 * fire a click without going through press/release.
 */
lh_void
lh_os_window_on_native_click(lh_os_window_t *self, int x, int y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_H */
