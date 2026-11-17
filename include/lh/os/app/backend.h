/**
 * @file backend.h
 * @brief The contract under an application's loop: ::lh_os_app_backend_t.
 *
 * What ::lh_os_app_run needs from below, as a table of calls: the time, a
 * wait for the next thing to do, and a way to cut that wait short. On an
 * operating system the system backend (`lh/os/app/backend/system.h`) fills it
 * with the message pump; on a board it is rail's clock and queue, or a sleep
 * until the next interrupt; in a test it is a fake.
 *
 * The window side is a separate table (::lh_os_window_backend_t): a board can
 * have a loop without windows, and a host can have windows on a loop it runs
 * itself and never call ::lh_os_app_run.
 *
 * An application holds the table and its context (::lh_os_app_install_backend).
 * Nothing is installed by default.
 */

#ifndef LH_OS_APP_BACKEND_H
#define LH_OS_APP_BACKEND_H

#include <lh/core/timer/clock/fn.h>
#include <lh/os/app/backend/fields.h>
#include <lh/os/app/backend/wait/fn.h>
#include <lh/os/app/backend/wake/fn.h>

/**
 * @struct lh_os_app_backend
 * @brief The slots a loop backend fills.
 */
struct lh_os_app_backend
{
    /** Members: ::lh_os_app_backend_fields. */
    lh_os_app_backend_fields(lh_timer_clock_fn, lh_os_app_backend_wait_fn,
                             lh_os_app_backend_wake_fn);
};
/** @copybrief lh_os_app_backend */
typedef struct lh_os_app_backend lh_os_app_backend_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The `now` slot of @p self (level 1): the clock the application's timers run by.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_timer_clock_fn *
lh_os_app_backend_get_now(const lh_os_app_backend_t *self);

/**
 * @brief Store the `now` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_app_backend_set_now(lh_os_app_backend_t *self, lh_timer_clock_fn *fn);

/**
 * @brief The `wait` slot of @p self (level 1): wait for the next thing to do.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_app_backend_wait_fn *
lh_os_app_backend_get_wait(const lh_os_app_backend_t *self);

/**
 * @brief Store the `wait` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_app_backend_set_wait(lh_os_app_backend_t *self, lh_os_app_backend_wait_fn *fn);

/**
 * @brief The `wake` slot of @p self (level 1): cut a wait short.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_app_backend_wake_fn *
lh_os_app_backend_get_wake(const lh_os_app_backend_t *self);

/**
 * @brief Store the `wake` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_app_backend_set_wake(lh_os_app_backend_t *self, lh_os_app_backend_wake_fn *fn);

/**
 * @brief Empty table: every slot ::lh_null.
 *
 * A backend then sets the slots it has. The table is the backend's and is
 * usually a constant it hands out; nothing here owns it.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_app_backend_init(lh_os_app_backend_t *self);

/**
 * @brief True when every required slot of @p self is set: `now`, `wait`, `wake`.
 *
 * All three are required: a loop cannot run without the time, a wait or a
 * way to cut the wait short.
 *
 * @param self The table; must not be null.
 * @return True when the table can be used.
 */
lh_bool_t
lh_os_app_backend_is_complete(const lh_os_app_backend_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_BACKEND_H */
