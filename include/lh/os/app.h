/**
 * @file app.h
 * @brief The application: ::lh_os_app_t.
 *
 * An application is a **step**, the timers it fires and its top-level
 * ::lh_os_window_t: the first window linked is the main one, and closing it
 * quits the app.
 *
 * The step is the whole application in one call — the timers that are due,
 * and then the work (::lh_os_app_step). ::lh_os_app_run is the loop that calls
 * it; a host that owns its loop — rail's (::lh_rail_loop_pass), a browser's,
 * a bare `main` on a board — calls ::lh_os_app_step itself instead. The step
 * call has the same shape as a rail loop's step (::lh_os_app_step_fn), so one
 * can stand in for the other.
 *
 * **What is below the application is two contracts**, and the host installs
 * both: the loop's (::lh_os_app_backend_t, ::lh_os_app_install_backend) — the
 * time, the wait, the wake — and the windows' (::lh_os_window_backend_t,
 * ::lh_os_app_install_window_backend). On an operating system both are the system
 * backend (`lh/os/app/backend/system.h`, `lh/os/window/backend/system.h`); on a
 * board they are the board's; in a test they are fakes. Nothing is installed by
 * default, and the application is the same code with an operating system and
 * without one.
 *
 * Product state stays in the host: the app owns no product state, and neither
 * the timers', the windows' nor the backends' memory is its own.
 */

#ifndef LH_OS_APP_H
#define LH_OS_APP_H

#include <lh/os/app/backend.h>
#include <lh/os/app/fields.h>
#include <lh/os/app/step/cb.h>
#include <lh/os/window/backend.h>

struct lh_os_window;

/**
 * @struct lh_os_app
 * @brief The windows, the step, the logical timers, the backends, and the quit flag.
 */
struct lh_os_app
{
    /** Members: ::lh_os_app_fields. */
    lh_os_app_fields(lh_list_t, lh_timer_group_t, lh_os_app_step_cb, lh_ptr, lh_os_app_backend_t,
                     lh_os_window_backend_t);
};
/** @copybrief lh_os_app */
typedef struct lh_os_app lh_os_app_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Logical timer group of @p self (add timers from outside).
 *
 * @param self The application; must not be null.
 * @return The group.
 */
lh_timer_group_t *
lh_os_app_get_timers(lh_os_app_t *self);

/**
 * @brief True when @p self has asked to stop.
 *
 * @param self The application; must not be null.
 * @return True when the app is quitting.
 */
lh_bool_t
lh_os_app_is_quit(const lh_os_app_t *self);

/**
 * @brief Store whether @p self stops (level 1).
 *
 * A level-1 setter: the flag is stored as given. ::lh_os_app_quit sets it.
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @param quit The new flag.
 */
lh_void
lh_os_app_set_quit(lh_os_app_t *self, lh_bool_t quit);

/**
 * @brief The step of @p self.
 *
 * @param self The application; must not be null.
 * @return The call, or ::lh_null when the app has no step.
 */
lh_os_app_step_cb
lh_os_app_get_step_fn(const lh_os_app_t *self);

/**
 * @brief Store the step of @p self (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; may be ::lh_null.
 */
lh_void
lh_os_app_set_step_fn(lh_os_app_t *self, lh_os_app_step_cb fn);

/**
 * @brief The state the step of @p self gets.
 *
 * @param self The application; must not be null.
 * @return The step caller's own state.
 */
lh_ptr
lh_os_app_get_step_context(const lh_os_app_t *self);

/**
 * @brief Store the state the step of @p self gets (level 1).
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param context The step caller's own state.
 */
lh_void
lh_os_app_set_step_context(lh_os_app_t *self, lh_ptr context);

/**
 * @brief The loop's table of @p self, or ::lh_null when none is installed (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
const lh_os_app_backend_t *
lh_os_app_get_backend(const lh_os_app_t *self);

/**
 * @brief Store the loop's table of @p self (level 1).
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param backend The table; the host's, and it must outlive the application.
 */
lh_void
lh_os_app_set_backend(lh_os_app_t *self, const lh_os_app_backend_t *backend);

/**
 * @brief The context passed to every call of the loop's table (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_app_get_backend_context(const lh_os_app_t *self);

/**
 * @brief Store the context passed to every call of the loop's table (level 1).
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param context The backend's context.
 */
lh_void
lh_os_app_set_backend_context(lh_os_app_t *self, lh_ptr context);

/**
 * @brief The windows' table of @p self, or ::lh_null when none is installed
 *        (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
const lh_os_window_backend_t *
lh_os_app_get_window_backend(const lh_os_app_t *self);

/**
 * @brief Store the windows' table of @p self (level 1).
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param backend The table; the host's, and it must outlive every window.
 */
lh_void
lh_os_app_set_window_backend(lh_os_app_t *self, const lh_os_window_backend_t *backend);

/**
 * @brief The context passed to every call of the windows' table (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_app_get_window_backend_context(const lh_os_app_t *self);

/**
 * @brief Store the context passed to every call of the windows' table (level 1).
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param context The backend's context.
 */
lh_void
lh_os_app_set_window_backend_context(lh_os_app_t *self, lh_ptr context);

/** @brief The @c windows of @p self. */
const lh_list_t *
lh_os_app_get_windows(const lh_os_app_t *self);

/** @brief The @c windows of @p self, to change in place. */
lh_list_t *
lh_os_app_get_windows_mut(lh_os_app_t *self);

/**
 * @brief Ask to stop, and wake the loop if it is waiting.
 *
 * The wake is the loop backend's `wake`, when one is installed.
 *
 * @param self The application; must not be null.
 */
lh_void
lh_os_app_quit(lh_os_app_t *self);

/**
 * @brief Install the loop's table of @p self and its context.
 *
 * @param self    The application; must not be null.
 * @param backend The table; the host's, and it must outlive the application.
 *                ::lh_null uninstalls it.
 * @param context Passed to every call of the table.
 */
lh_void
lh_os_app_install_backend(lh_os_app_t *self, const lh_os_app_backend_t *backend, lh_ptr context);

/**
 * @brief Install the windows' table of @p self and its context.
 *
 * A window opened into @p self (::lh_os_window_open) takes both; a window that is
 * already open keeps the ones it took.
 *
 * @param self    The application; must not be null.
 * @param backend The table; the host's, and it must outlive every window.
 *                ::lh_null uninstalls it.
 * @param context Passed to every call of the table.
 */
lh_void
lh_os_app_install_window_backend(lh_os_app_t *self, const lh_os_window_backend_t *backend,
                                 lh_ptr context);

/**
 * @brief The whole application in one call: the timers that are due, then the
 *        step.
 *
 * The caller owns the clock and says what time it is: ::lh_os_app_run passes
 * the loop backend's `now`, a host that runs its own loop passes its own.
 *
 * @param self The application; must not be null.
 * @param now  The current time.
 */
lh_void
lh_os_app_step(lh_os_app_t *self, lh_tick_t now);

/**
 * @brief First top-level window (same as ::lh_os_app_get_window), or ::lh_null.
 *
 * @param self The application; must not be null.
 * @return The first window.
 */
struct lh_os_window *
lh_os_app_get_first_window(const lh_os_app_t *self);

/**
 * @brief Top-level window after @p window, or ::lh_null.
 *
 * @param self   The application; must not be null.
 * @param window The window to start after.
 * @return The next window.
 */
struct lh_os_window *
lh_os_app_get_next_window(const lh_os_app_t *self, const struct lh_os_window *window);

/**
 * @brief True while ::lh_os_app_run has something to run for.
 *
 * Not quitting, and — for an application that has windows to live in, that is,
 * one with a window backend installed — at least one window still linked. An
 * application without a window backend (a remote that only reads its buttons)
 * runs until it quits.
 *
 * @param self The application; must not be null.
 * @return True while the loop goes on.
 */
lh_bool_t
lh_os_app_is_running(const lh_os_app_t *self);

/**
 * @brief Empty app: no windows, empty timer group, no step, no backends, not
 *        quitting.
 *
 * @param self The application; must not be null.
 */
lh_void
lh_os_app_init(lh_os_app_t *self);

/**
 * @brief Close every top-level window still linked, then clear @p self.
 *
 * @param self The application; must not be null.
 */
lh_void
lh_os_app_deinit(lh_os_app_t *self);

/**
 * @brief Main window (index 0), or ::lh_null when none are linked.
 *
 * @param self The application; must not be null.
 * @return The main window.
 */
struct lh_os_window *
lh_os_app_get_window(const lh_os_app_t *self);

/**
 * @brief Run the loop: the step, then a wait until the next timer, while
 *        ::lh_os_app_is_running.
 *
 * Every turn reads the time from the loop backend's `now`, calls
 * ::lh_os_app_step, and waits through the backend's `wait` for at most what
 * ::lh_timer_group_until_next says. A `wait` that reports a quit quits.
 *
 * An application with no loop backend, or with one that lacks a slot, is a
 * checked failure (::lh_runtime_check_if): nothing is installed by default.
 *
 * @param self The application; must not be null.
 */
lh_void
lh_os_app_run(lh_os_app_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_H */
