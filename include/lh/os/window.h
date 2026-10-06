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
#include <lh/os/window/on/click/cb.h>
#include <lh/os/window/on/close/cb.h>
#include <lh/os/window/on/move/cb.h>
#include <lh/os/window/on/paint/cb.h>
#include <lh/os/window/on/press/cb.h>
#include <lh/os/window/on/release/cb.h>
#include <lh/os/window/on/wheel/cb.h>
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
 * @p context is passed through. ::lh_null clears the slot. @p delta is
 * notches (positive = away from the user).
 */
lh_void
lh_os_window_set_on_wheel(lh_os_window_t *self, lh_os_window_on_wheel_cb on_wheel, lh_ptr context);

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
 * @brief Called from the native backend inside a paint cycle with @p paint_dc
 *        and the update rectangle set.
 */
lh_void
lh_os_window_on_native_paint(lh_os_window_t *self, lh_ptr paint_dc, int left, int top, int right,
                             int bottom);

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
 * @p x and @p y are client-area coordinates. @p delta is notches.
 */
lh_void
lh_os_window_on_native_wheel(lh_os_window_t *self, int x, int y, int delta);

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
