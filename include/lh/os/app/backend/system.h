/**
 * @file system.h
 * @brief The system backend of an application's loop: ::lh_os_app_backend_t over
 *        the operating system's message pump.
 *
 * `now` is the system's monotonic milliseconds (::lh_os_tick_ms), `wait` is the
 * message pump (::lh_os_system_window_pump_wait), and `wake` posts the system's
 * quit message (::lh_os_system_window_post_quit) — the only thing that cuts a
 * system's wait short, and the only reason the loop is woken (::lh_os_app_quit).
 *
 * The table is the host's, filled by ::lh_os_app_backend_system_init, and
 * installed with ::lh_os_app_install_backend; nothing installs it by default.
 * The context is not used.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW: the pump is the window system's.
 */

#ifndef LH_OS_APP_BACKEND_SYSTEM_H
#define LH_OS_APP_BACKEND_SYSTEM_H

#include <lh/config.h>
#include <lh/os/app/backend.h>

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/app/backend/system.h requires LH_LIBRARY_OPTION_OS_WINDOW"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** @brief `now`: ::lh_os_tick_ms. */
lh_tick_t
lh_os_app_backend_system_now(lh_ptr context);

/**
 * @brief `wait`: ::lh_os_system_window_pump_wait for at most @p timeout.
 *
 * @return True when the pump saw the system's quit message.
 */
lh_bool_t
lh_os_app_backend_system_wait(lh_ptr context, lh_tick_t timeout);

/** @brief `wake`: ::lh_os_system_window_post_quit. */
lh_void
lh_os_app_backend_system_wake(lh_ptr context);

/**
 * @brief Fill @p table with every slot of the system backend.
 *
 * @param table The host's table; must not be null, and must outlive the
 *              application it is installed on.
 */
lh_void
lh_os_app_backend_system_init(lh_os_app_backend_t *table);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_BACKEND_SYSTEM_H */
