/**
 * @file thread.h
 * @brief OS thread primitive, independent of game hosts.
 *
 * Initialize before use. Do not copy live objects. Native failures are reported through
 * lh_os_system_last_error; contract failures through lh_os_last_error. Opening, closing
 * and joining require external lifetime coordination.
 */

#ifndef LH_OS_THREAD_H
#define LH_OS_THREAD_H

#include <lh/os/system/concurrency.h>
#include <lh/os/thread/fields.h>

/**
 * @brief Native-resource owner for one OS thread.
 */
struct lh_os_thread
{
    /** Members: ::lh_os_thread_fields. */
    lh_os_thread_fields(lh_os_system_thread_handle_t);
};

/** @copybrief lh_os_thread */
typedef struct lh_os_thread lh_os_thread_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Initialize an empty thread object.
 *
 * Do not reinitialize a live thread. Initialization acquires no native resources.
 *
 * @param self Object to initialize; null is ignored.
 */
lh_void
lh_os_thread_init(lh_os_thread_t *self);

/**
 * @brief Start a native worker in an initialized, empty object.
 *
 * On success the callback may run before this function returns. Affinity is supported on
 * Linux and Windows; other POSIX backends reject an explicit CPU. SIGPIPE blocking affects
 * only the new POSIX worker and is ignored on Windows. Coordinate cancellation with the
 * callback; this API never forcibly cancels a running worker.
 *
 * @param self Initialized object that does not own a thread.
 * @param run Worker callback; must not be null.
 * @param user Borrowed callback state, retained until the worker exits.
 * @param cpu Logical CPU index, or -1 to inherit affinity.
 * @param flags Zero or ::lh_os_thread_block_sigpipe.
 * @return ::lh_bool_true on success; ::lh_bool_false without invoking run on failure.
 */
lh_bool_t
lh_os_thread_start(lh_os_thread_t *self, lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu,
                   lh_u32_t flags);

/**
 * @brief Wait for the worker and release its native resources.
 *
 * Joining an empty object succeeds. Join from another thread; joining oneself fails. No
 * other caller may concurrently join or modify this object.
 *
 * @param self Initialized thread object, exclusively owned by the joining caller.
 * @return ::lh_bool_true when closed; ::lh_bool_false with ownership retained on failure.
 */
lh_bool_t
lh_os_thread_join(lh_os_thread_t *self);

/**
 * @brief Suspend the calling thread for a finite interval.
 *
 * POSIX interruptions are retried using the remaining interval. Actual wakeup may be later
 * due to scheduling. This call does not pump game events or invoke callbacks.
 *
 * @param milliseconds Minimum sleep interval in milliseconds.
 * @return ::lh_bool_true on completion; ::lh_bool_false on a native failure.
 */
lh_bool_t
lh_os_thread_sleep(lh_u32_t milliseconds);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_THREAD_H */
