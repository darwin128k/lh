/**
 * @file condition.h
 * @brief OS condition primitive, independent of game hosts.
 *
 * Initialize before use. Do not copy live objects. Native failures are reported through
 * lh_os_system_last_error; contract failures through lh_os_last_error. Opening, closing
 * and joining require external lifetime coordination.
 */

#ifndef LH_OS_CONDITION_H
#define LH_OS_CONDITION_H

#include <lh/os/system/concurrency.h>
#include <lh/os/condition/fields.h>
#include <lh/os/mutex.h>

/**
 * @brief Native-resource owner for one OS condition.
 */
struct lh_os_condition
{
    /** Members: ::lh_os_condition_fields. */
    lh_os_condition_fields(lh_os_system_condition_handle_t);
};

/** @copybrief lh_os_condition */
typedef struct lh_os_condition lh_os_condition_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Initialize an empty condition-variable object.
 *
 * Initialization acquires no native resources. Do not reinitialize an open condition.
 *
 * @param self Object to initialize; null is ignored.
 */
lh_void
lh_os_condition_init(lh_os_condition_t *self);

/**
 * @brief Acquire a native condition variable.
 *
 * Windows requires the native condition-variable API available since Vista. Missing
 * runtime APIs are reported as a native failure.
 *
 * @param self Initialized object that does not own a condition variable.
 * @return ::lh_bool_true on success; ::lh_bool_false if invalid or unavailable.
 */
lh_bool_t
lh_os_condition_open(lh_os_condition_t *self);

/**
 * @brief Release an unused condition variable.
 *
 * All waiters must have returned before closing. Closing an empty object succeeds.
 *
 * @param self Initialized object with no waiters or concurrent users.
 * @return ::lh_bool_true when closed; ::lh_bool_false with ownership retained on failure.
 */
lh_bool_t
lh_os_condition_close(lh_os_condition_t *self);

/**
 * @brief Atomically release the mutex, wait and reacquire it.
 *
 * Always check the protected predicate in a loop: spurious wakeups are permitted.
 * Concurrent waiters on one condition must use the same mutex. Keep both objects alive
 * until every waiter has returned.
 *
 * @param self Open condition variable.
 * @param mutex Open mutex held exactly once by the caller.
 * @return ::lh_bool_true with the mutex reacquired; ::lh_bool_false on failure.
 */
lh_bool_t
lh_os_condition_wait(lh_os_condition_t *self, lh_os_mutex_t *mutex);

/**
 * @brief Wake one current waiter, if any.
 *
 * Protect predicate changes with the waiters' mutex. Notifications are not stored for
 * future waiters.
 *
 * @param self Open condition variable.
 * @return ::lh_bool_true on success; ::lh_bool_false on failure.
 */
lh_bool_t
lh_os_condition_signal(lh_os_condition_t *self);

/**
 * @brief Wake all current waiters.
 *
 * Every awakened waiter must reacquire the shared mutex and recheck its predicate.
 *
 * @param self Open condition variable.
 * @return ::lh_bool_true on success; ::lh_bool_false on failure.
 */
lh_bool_t
lh_os_condition_broadcast(lh_os_condition_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_CONDITION_H */
