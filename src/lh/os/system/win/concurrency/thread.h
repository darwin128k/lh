/**
 * @file thread.h
 * @brief Backend-private: one Win32 thread of ours and its startup handshake.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_CONCURRENCY_THREAD_H
#define LH_SRC_OS_SYSTEM_WIN_CONCURRENCY_THREAD_H

#include <windows.h>
#include <lh/os/system/concurrency.h>

typedef struct lh_os_native_thread
{
    HANDLE handle;
    unsigned id;
    HANDLE gate;
    int proceed;
    lh_os_thread_fn run;
    lh_ptr user;
} lh_os_native_thread_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The thread handle (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
HANDLE
lh_os_native_thread_get_handle(const lh_os_native_thread_t *self);

/**
 * @brief Store the thread handle (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param handle The new value.
 */
lh_void
lh_os_native_thread_set_handle(lh_os_native_thread_t *self, HANDLE handle);

/**
 * @brief The thread id (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
unsigned
lh_os_native_thread_get_id(const lh_os_native_thread_t *self);

/**
 * @brief Store the thread id (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param id The new value.
 */
lh_void
lh_os_native_thread_set_id(lh_os_native_thread_t *self, unsigned id);

/**
 * @brief The event the new thread waits on before it runs (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
HANDLE
lh_os_native_thread_get_gate(const lh_os_native_thread_t *self);

/**
 * @brief Store the event the new thread waits on before it runs (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param gate The new value.
 */
lh_void
lh_os_native_thread_set_gate(lh_os_native_thread_t *self, HANDLE gate);

/**
 * @brief Nonzero once the thread may run its code (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_native_thread_get_proceed(const lh_os_native_thread_t *self);

/**
 * @brief Store nonzero once the thread may run its code (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @param proceed The new value.
 */
lh_void
lh_os_native_thread_set_proceed(lh_os_native_thread_t *self, int proceed);

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
 * @brief The thread id, to change in place (level 1).
 *
 * @param self The thread; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
unsigned *
lh_os_native_thread_get_id_mut(lh_os_native_thread_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_WIN_CONCURRENCY_THREAD_H */
