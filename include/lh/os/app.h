/**
 * @file app.h
 * @brief Process shell for native windows: ::lh_os_app_t.
 *
 * Holds links to top-level ::lh_os_window_t added from outside. Index 0 is
 * the main window; closing it quits the app. Owns a ::lh_timer_group_t driven
 * each pump turn from ::lh_os_tick_ms. Product state stays in the host.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW.
 */

#ifndef LH_OS_APP_H
#define LH_OS_APP_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/app/fields.h>
#include <lh/timer/group.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/app.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/app.h requires LH_LIBRARY_OPTION_OS_WINDOW (CMake: -DLH_LIBRARY_OPTION_OS_WINDOW=ON)"
#endif

struct lh_os_window;

/**
 * @struct lh_os_app
 * @typedef lh_os_app_t
 * @brief Top-level windows, logical timers, and quit flag.
 */
struct lh_os_app
{
    lh_os_app_fields(lh_list_t, lh_timer_group_t);
};
typedef struct lh_os_app lh_os_app_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty app: no windows, empty timer group, not quitting.
 */
lh_void
lh_os_app_init(lh_os_app_t *self);

/**
 * @brief Close every top-level window still linked, then clear @p self.
 */
lh_void
lh_os_app_deinit(lh_os_app_t *self);

/**
 * @brief Main window (index 0), or ::lh_null when none are linked.
 */
struct lh_os_window *
lh_os_app_get_window(const lh_os_app_t *self);

/**
 * @brief Logical timer group of @p self (add timers from outside).
 */
lh_timer_group_t *
lh_os_app_get_timers(lh_os_app_t *self);

/**
 * @brief True when @p self has asked the pump to stop.
 */
lh_bool_t
lh_os_app_is_quit(const lh_os_app_t *self);

/**
 * @brief Ask the pump to stop and wake it if it is waiting.
 */
lh_void
lh_os_app_quit(lh_os_app_t *self);

/**
 * @brief First top-level window (same as ::lh_os_app_get_window), or ::lh_null.
 */
struct lh_os_window *
lh_os_app_get_first_window(const lh_os_app_t *self);

/**
 * @brief Top-level window after @p window, or ::lh_null.
 */
struct lh_os_window *
lh_os_app_get_next_window(const lh_os_app_t *self, const struct lh_os_window *window);

/**
 * @brief Pump OS messages and fire due timers until quit or no windows.
 */
lh_void
lh_os_app_run(lh_os_app_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_H */
