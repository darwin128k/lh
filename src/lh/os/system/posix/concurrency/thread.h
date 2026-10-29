/**
 * @file thread.h
 * @brief Backend-private: one POSIX thread of ours and its startup handshake.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_CONCURRENCY_THREAD_H
#define LH_SRC_OS_SYSTEM_POSIX_CONCURRENCY_THREAD_H

#include <pthread.h>
#include <lh/os/system/concurrency.h>

typedef struct lh_os_native_thread
{
    pthread_t thread;
    pthread_mutex_t gate;
    pthread_cond_t ready;
    lh_os_thread_fn run;
    lh_ptr user;
    lh_u32_t flags;
    int prepared, preparation_error, proceed;
} lh_os_native_thread_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The thread (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
pthread_t
lh_os_native_thread_get_thread(const lh_os_native_thread_t *self);

/**
 * @brief Store the thread (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param thread The new value.
 */
lh_void
lh_os_native_thread_set_thread(lh_os_native_thread_t *self, pthread_t thread);

/**
 * @brief The lock over the startup handshake (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
const pthread_mutex_t *
lh_os_native_thread_get_gate(const lh_os_native_thread_t *self);

/**
 * @brief The lock over the startup handshake, to change in place (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
pthread_mutex_t *
lh_os_native_thread_get_gate_mut(lh_os_native_thread_t *self);

/**
 * @brief What the two sides of the handshake wait on (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
const pthread_cond_t *
lh_os_native_thread_get_ready(const lh_os_native_thread_t *self);

/**
 * @brief What the two sides of the handshake wait on, to change in place (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
pthread_cond_t *
lh_os_native_thread_get_ready_mut(lh_os_native_thread_t *self);

/**
 * @brief The code the thread runs (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
lh_os_thread_fn
lh_os_native_thread_get_run_fn(const lh_os_native_thread_t *self);

/**
 * @brief Store the code the thread runs (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param run The new value.
 */
lh_void
lh_os_native_thread_set_run_fn(lh_os_native_thread_t *self, lh_os_thread_fn run);

/**
 * @brief The value the code is given (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_native_thread_get_user(const lh_os_native_thread_t *self);

/**
 * @brief Store the value the code is given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param user The new value.
 */
lh_void
lh_os_native_thread_set_user(lh_os_native_thread_t *self, lh_ptr user);

/**
 * @brief The start flags of the thread (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_os_native_thread_get_flags(const lh_os_native_thread_t *self);

/**
 * @brief Store the start flags of the thread (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param flags The new value.
 */
lh_void
lh_os_native_thread_set_flags(lh_os_native_thread_t *self, lh_u32_t flags);

/**
 * @brief Nonzero once the new thread has prepared itself (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_native_thread_get_prepared(const lh_os_native_thread_t *self);

/**
 * @brief Store nonzero once the new thread has prepared itself (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param prepared The new value.
 */
lh_void
lh_os_native_thread_set_prepared(lh_os_native_thread_t *self, int prepared);

/**
 * @brief The error the preparation ended with; 0 for none (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_native_thread_get_preparation_error(const lh_os_native_thread_t *self);

/**
 * @brief Store the error the preparation ended with; 0 for none (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param preparation_error The new value.
 */
lh_void
lh_os_native_thread_set_preparation_error(lh_os_native_thread_t *self, int preparation_error);

/**
 * @brief 1 once the thread may run its code, -1 when it must not (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_native_thread_get_proceed(const lh_os_native_thread_t *self);

/**
 * @brief Store 1 once the thread may run its code, -1 when it must not (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param proceed The new value.
 */
lh_void
lh_os_native_thread_set_proceed(lh_os_native_thread_t *self, int proceed);

/**
 * @brief The thread, to change in place (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
pthread_t *
lh_os_native_thread_get_thread_mut(lh_os_native_thread_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_CONCURRENCY_THREAD_H */
