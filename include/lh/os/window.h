/**
 * @file window.h
 * @brief A native OS window — ::lh_os_window_t.
 *
 * Top-level windows are linked into an ::lh_os_app_t. A window may own modal
 * children (linked into `children`); those disable their parent until closed.
 * The app / parent does not own window memory — only the links.
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
#include <lh/os/window/fields.h>
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
 * Links @p self into @p app. Fails when already open or already linked.
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
 * Clears the handle, re-enables a modal parent, and unlinks from app or parent.
 */
lh_void
lh_os_window_on_native_destroy(lh_os_window_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_H */
