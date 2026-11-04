/**
 * @file mutex.h
 * @brief OS mutex primitive, independent of game hosts.
 *
 * Initialize before use. Do not copy live objects. Native failures are reported through
 * lh_os_system_last_error; contract failures through lh_os_last_error. Opening, closing
 * and joining require external lifetime coordination.
 */

#ifndef LH_OS_MUTEX_H
#define LH_OS_MUTEX_H

#include <lh/os/system/concurrency.h>
#include <lh/core/memory/allocator.h>
#include <lh/os/mutex/fields.h>

/**
 * @brief Native-resource owner for one OS mutex.
 */
struct lh_os_mutex
{
    /** Members: ::lh_os_mutex_fields. */
    lh_os_mutex_fields(lh_os_system_mutex_handle_t, lh_memory_allocator_t);
};

/** @copybrief lh_os_mutex */
typedef struct lh_os_mutex lh_os_mutex_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The allocator the native storage of @p self came from (level 1).
 * @param self Object; must not be null.
 * @return A copy of the runtime allocator taken when the native object was
 *         created; meaningless while @p self owns none.
 */
const lh_memory_allocator_t *
lh_os_mutex_get_allocator(const lh_os_mutex_t *self);

/**
 * @brief The allocator the native storage of @p self came from, for change (level 1).
 * @param self Object; must not be null.
 * @return The object's own copy of the allocator.
 */
lh_memory_allocator_t *
lh_os_mutex_get_allocator_mut(lh_os_mutex_t *self);

/**
 * @brief Initialize an empty mutex object.
 *
 * Initialization acquires no native resources. Do not reinitialize an open mutex.
 *
 * @param self Object to initialize; null is ignored.
 */
lh_void
lh_os_mutex_init(lh_os_mutex_t *self);

/**
 * @brief Acquire a native mutex for an initialized, empty object.
 *
 * @param self Initialized object that does not own a mutex.
 * @return ::lh_bool_true on success; ::lh_bool_false if invalid, already open or unavailable.
 */
lh_bool_t
lh_os_mutex_open(lh_os_mutex_t *self);

/**
 * @brief Release an unused native mutex.
 *
 * The mutex must be unlocked and no other thread may access it. Closing an empty object
 * succeeds.
 *
 * @param self Initialized mutex object with no concurrent users.
 * @return ::lh_bool_true when closed; ::lh_bool_false with ownership retained on failure.
 */
lh_bool_t
lh_os_mutex_close(lh_os_mutex_t *self);

/**
 * @brief Acquire exclusive ownership of an open mutex.
 *
 * This call may block. Do not recursively lock the mutex; unlock it on the same thread.
 *
 * @param self Open mutex to lock.
 * @return ::lh_bool_true with the lock held; ::lh_bool_false on failure.
 */
lh_bool_t
lh_os_mutex_lock(lh_os_mutex_t *self);

/**
 * @brief Release a mutex held by the calling thread.
 *
 * The mutex and its shared state must remain alive until all users have finished.
 *
 * @param self Open mutex locked exactly once by the caller.
 * @return ::lh_bool_true on success; ::lh_bool_false on failure.
 */
lh_bool_t
lh_os_mutex_unlock(lh_os_mutex_t *self);

/**
 * @brief The native mutex handle (level 1).
 *
 * ::lh_null while the mutex is not open.
 *
 * @param self The mutex; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_mutex_handle_t
lh_os_mutex_get_handle(const lh_os_mutex_t *self);

/**
 * @brief Store the native mutex handle (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The mutex; checked by ::lh_assert_runtime_ref.
 * @param handle The new value.
 */
lh_void
lh_os_mutex_set_handle(lh_os_mutex_t *self, lh_os_system_mutex_handle_t handle);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MUTEX_H */
