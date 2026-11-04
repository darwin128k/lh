/**
 * @file concurrency.h
 * @brief Native thread and synchronization operations selected by CMake.
 *
 * The backend allocates nothing. Each native object (a thread's startup
 * record, a mutex, a condition) lives in storage the caller hands in: the
 * caller asks for its size (`_get_size`), allocates it zero-filled, passes it
 * to `_start` / `_open`, and frees it after a successful `_join` / `_close`.
 * The portable layer (`lh/os/thread.h`, `mutex.h`, `condition.h`) does this
 * through the runtime allocator, so the backend calls only the platform.
 * A handle is the storage's address.
 */

#ifndef LH_OS_SYSTEM_CONCURRENCY_H
#define LH_OS_SYSTEM_CONCURRENCY_H

#include <lh/config.h>
#include <lh/core/bool.h>
#include <lh/core/ptr.h>
#include <lh/core/size.h>
#include <lh/core/compiler/extern/c.h>

#if !LH_LIBRARY_OPTION_OS
#    error "OS concurrency requires LH_LIBRARY_OPTION_OS"
#endif

/**
 * @brief Opaque native thread handle; ::lh_null means no owned resource.
 */
typedef lh_ptr lh_os_system_thread_handle_t;

/**
 * @brief Opaque native mutex handle; ::lh_null means no owned resource.
 */
typedef lh_ptr lh_os_system_mutex_handle_t;

/**
 * @brief Opaque native condition handle; ::lh_null means no owned resource.
 */
typedef lh_ptr lh_os_system_condition_handle_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Callback executed on a successfully created worker thread.
 *
 * @param user Borrowed state that remains alive until the callback returns.
 */
typedef lh_void (*lh_os_thread_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

/**
 * @def lh_os_thread_block_sigpipe
 * @brief Block SIGPIPE in the new POSIX worker; ignored on Windows.
 */
#define lh_os_thread_block_sigpipe 1U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Bytes of storage one native thread needs (::lh_os_system_thread_start).
 */
lh_usize_t
lh_os_system_thread_get_size(void);

/**
 * @brief Create a native worker and validate its startup options before user code runs.
 *
 * The caller must join every successful handle. A successful callback may run before start
 * returns. Native failures are captured in lh_os_system_last_error.
 *
 * @param storage Zero-filled storage of ::lh_os_system_thread_get_size bytes, owned by
 *        the caller and left in place until the join succeeds; the backend never frees
 *        it, on failure either.
 * @param run Worker callback; must not be null.
 * @param user Borrowed callback state, retained until the worker exits.
 * @param cpu Logical CPU index, or -1 to inherit affinity.
 * @param flags Zero or ::lh_os_thread_block_sigpipe.
 * @return Owned handle, or ::lh_null on failure without invoking the callback.
 */
lh_os_system_thread_handle_t
lh_os_system_thread_start(lh_ptr storage, lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu,
                          lh_u32_t flags);

/**
 * @brief Wait for a native worker and release its native resources.
 *
 * Success consumes the handle (the caller then frees its storage); failure retains
 * ownership. The caller must prevent concurrent use during release.
 *
 * @param handle Live native thread handle that remains valid during the operation.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_thread_join(lh_os_system_thread_handle_t handle);

/**
 * @brief Bytes of storage one native mutex needs (::lh_os_system_mutex_open).
 */
lh_usize_t
lh_os_system_mutex_get_size(void);

/**
 * @brief Initialize one native mutex in @p storage.
 *
 * The caller releases the handle with the matching close function.
 *
 * @param storage Zero-filled storage of ::lh_os_system_mutex_get_size bytes, owned by
 *        the caller, which frees it after a successful close (or after a failed open).
 * @return The handle (@p storage), or ::lh_null with a captured native error.
 */
lh_os_system_mutex_handle_t
lh_os_system_mutex_open(lh_ptr storage);

/**
 * @brief Destroy a native mutex after its last use.
 *
 * Success consumes the handle (the caller then frees its storage); failure retains
 * ownership. The caller must prevent
 * concurrent use during release. The mutex must be unlocked.
 *
 * @param handle Live native mutex handle that remains valid during the operation.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_mutex_close(lh_os_system_mutex_handle_t handle);

/**
 * @brief Lock a native mutex on the calling thread.
 *
 * Do not concurrently close or modify the handle.
 *
 * @param handle Live native mutex handle that remains valid during the operation.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_mutex_lock(lh_os_system_mutex_handle_t handle);

/**
 * @brief Unlock the native mutex held by the calling thread.
 *
 * Do not concurrently close or modify the handle.
 *
 * @param handle Live native mutex handle that remains valid during the operation.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_mutex_unlock(lh_os_system_mutex_handle_t handle);

/**
 * @brief Bytes of storage one native condition needs (::lh_os_system_condition_open).
 */
lh_usize_t
lh_os_system_condition_get_size(void);

/**
 * @brief Initialize one native condition in @p storage.
 *
 * The caller releases the handle with the matching close function.
 *
 * @param storage Zero-filled storage of ::lh_os_system_condition_get_size bytes, owned by
 *        the caller, which frees it after a successful close (or after a failed open).
 * @return The handle (@p storage), or ::lh_null with a captured native error.
 */
lh_os_system_condition_handle_t
lh_os_system_condition_open(lh_ptr storage);

/**
 * @brief Destroy a native condition after its last use.
 *
 * Success consumes the handle (the caller then frees its storage); failure retains
 * ownership. The caller must prevent
 * concurrent use during release. All condition waiters must have returned.
 *
 * @param handle Live native condition handle that remains valid during the operation.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_condition_close(lh_os_system_condition_handle_t handle);

/**
 * @brief ::lh_os_system_condition_wait that also returns after @p timeout_ms.
 *
 * A timeout is not a failure: the caller rechecks its predicate either way.
 *
 * @param handle     Live native condition handle that remains valid during the operation.
 * @param mutex      Live native mutex held exactly once by the caller.
 * @param timeout_ms Longest wait, in milliseconds.
 * @return ::lh_bool_true after a signal, a spurious wakeup or the timeout, with the
 *         mutex held; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_condition_wait_for(lh_os_system_condition_handle_t handle,
                                lh_os_system_mutex_handle_t mutex, lh_u32_t timeout_ms);

/**
 * @brief Wait on a native condition, atomically releasing and reacquiring the mutex.
 *
 * Do not concurrently close or modify the handle. Loop on a protected predicate and use
 * the same mutex for all concurrent waiters.
 *
 * @param handle Live native condition handle that remains valid during the operation.
 * @param mutex Live native mutex held exactly once by the caller.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_condition_wait(lh_os_system_condition_handle_t handle,
                            lh_os_system_mutex_handle_t mutex);

/**
 * @brief Notify one native condition waiter, if any.
 *
 * Do not concurrently close or modify the handle.
 *
 * @param handle Live native condition handle that remains valid during the operation.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_condition_signal(lh_os_system_condition_handle_t handle);

/**
 * @brief Notify all current native condition waiters.
 *
 * Do not concurrently close or modify the handle.
 *
 * @param handle Live native condition handle that remains valid during the operation.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_condition_broadcast(lh_os_system_condition_handle_t handle);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_CONCURRENCY_H */
