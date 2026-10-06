/**
 * @file app.h
 * @brief Process shell for native windows: ::lh_os_app_t.
 *
 * Owns no window memory. Holds the list of top-level ::lh_os_window_t and
 * runs one OS message pump until quit or every top-level window is gone.
 * Product state (log, render, scene) stays in the host — e.g. `pa_app_t`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW.
 */

#ifndef LH_OS_APP_H
#define LH_OS_APP_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/app/fields.h>
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
 * @brief Top-level window list and quit flag for one process shell.
 */
struct lh_os_app
{
    lh_os_app_fields(lh_list_t);
};
typedef struct lh_os_app lh_os_app_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty app: no windows, not quitting.
 */
lh_void
lh_os_app_init(lh_os_app_t *self);

/**
 * @brief Close every top-level window still linked, then clear @p self.
 */
lh_void
lh_os_app_deinit(lh_os_app_t *self);

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
 * @brief First top-level window, or ::lh_null.
 */
struct lh_os_window *
lh_os_app_get_first_window(const lh_os_app_t *self);

/**
 * @brief Top-level window after @p window, or ::lh_null.
 */
struct lh_os_window *
lh_os_app_get_next_window(const lh_os_app_t *self, const struct lh_os_window *window);

/**
 * @brief Pump OS messages until quit or no top-level windows remain.
 */
lh_void
lh_os_app_run(lh_os_app_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_H */
