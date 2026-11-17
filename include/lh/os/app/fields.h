/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_os_app_t.
 */

#ifndef LH_OS_APP_FIELDS_H
#define LH_OS_APP_FIELDS_H

#include <lh/core/timer/group.h>

/**
 * @def lh_os_app_fields(list_type, timers_type, step_fn_type, step_context_type, backend_type,
 *                       window_backend_type)
 * @brief The windows, the logical timers, the step that is the application, the
 *        two backends it runs on, and quit.
 *
 * The app owns none of the memory it points at: the windows are linked into
 * `windows`, the timers into the group head, and both backend tables are the
 * host's, installed with their contexts beside them. `backend` is the loop's
 * (::lh_os_app_backend_t), `window_backend` the windows' (::lh_os_window_backend_t);
 * either may be ::lh_null, and nothing is installed by default.
 *
 * @param list_type           Type of the window list (::lh_list_t).
 * @param timers_type         Type of the timer group (::lh_timer_group_t).
 * @param step_fn_type        Type of the step call (::lh_os_app_step_cb).
 * @param step_context_type   The step caller's own state.
 * @param backend_type        Type of the loop's table (::lh_os_app_backend_t).
 * @param window_backend_type Type of the windows' table (::lh_os_window_backend_t).
 */
#define lh_os_app_fields(list_type, timers_type, step_fn_type, step_context_type, backend_type,    \
                         window_backend_type)                                                      \
    list_type windows;                                                                             \
    timers_type timers;                                                                            \
    step_fn_type step_fn;                                                                          \
    step_context_type step_context;                                                                \
    const backend_type *backend;                                                                   \
    lh_ptr backend_context;                                                                        \
    const window_backend_type *window_backend;                                                     \
    lh_ptr window_backend_context;                                                                 \
    lh_bool_t quit

#endif /* LH_OS_APP_FIELDS_H */
