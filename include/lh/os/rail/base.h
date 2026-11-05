/**
 * @file base.h
 * @brief The OS rail base: rail's clock, lock, signal and thread over lh/os.
 *
 * One implementation of the rail base (lh/rail/base.h), for a platform with an
 * operating system: the clock is the monotonic tick (lh/os/tick.h), a lock is
 * an ::lh_os_mutex_t, a signal an ::lh_os_condition_t, a thread an
 * ::lh_os_thread_t. Each open or start takes its object from the runtime
 * allocator and each close or join gives it back, so a handle is a pointer to
 * that object. Every group is there.
 *
 * The host puts it in the slot once, before it starts any rail thread or loop:
 * `lh_rail_base_assign(lh_rail_runtime_base_get_mut(), lh_os_rail_base_get());`
 * The calls below are public bricks as well: each keeps the promise of its
 * type in lh/rail/base/<call>/fn.h.
 */

#ifndef LH_OS_RAIL_BASE_H
#define LH_OS_RAIL_BASE_H

#include <lh/rail/base.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The monotonic milliseconds of lh/os/tick.h (an ::lh_rail_base_now_fn).
 * @return The milliseconds now; 0 when the OS clock fails.
 */
lh_u64_t
lh_os_rail_base_now_ms(lh_void);

/**
 * @brief Make an open ::lh_os_mutex_t (an ::lh_rail_base_open_fn).
 * @return The mutex, from the runtime allocator; ::lh_null when there is no
 *         memory or the OS refuses the mutex.
 */
lh_ptr
lh_os_rail_base_lock_open(lh_void);

/**
 * @brief Close the mutex @p lock and free it (an ::lh_rail_base_handle_fn).
 * @param lock A mutex from ::lh_os_rail_base_lock_open, not held.
 * @return ::lh_bool_false when the OS refuses the close (nothing is freed).
 */
lh_bool_t
lh_os_rail_base_lock_close(lh_ptr lock);

/**
 * @brief Take the mutex @p lock (an ::lh_rail_base_handle_fn).
 * @param lock A mutex from ::lh_os_rail_base_lock_open.
 * @return ::lh_bool_false on an OS failure.
 */
lh_bool_t
lh_os_rail_base_lock(lh_ptr lock);

/**
 * @brief Give the mutex @p lock back (an ::lh_rail_base_handle_fn).
 * @param lock A mutex the caller holds.
 * @return ::lh_bool_false on an OS failure.
 */
lh_bool_t
lh_os_rail_base_unlock(lh_ptr lock);

/**
 * @brief Make an open ::lh_os_condition_t (an ::lh_rail_base_open_fn).
 * @return The condition, from the runtime allocator; ::lh_null when there is no
 *         memory or the OS refuses it.
 */
lh_ptr
lh_os_rail_base_signal_open(lh_void);

/**
 * @brief Close the condition @p signal and free it (an ::lh_rail_base_handle_fn).
 * @param signal A condition from ::lh_os_rail_base_signal_open, with no sleeper.
 * @return ::lh_bool_false when the OS refuses the close (nothing is freed).
 */
lh_bool_t
lh_os_rail_base_signal_close(lh_ptr signal);

/**
 * @brief Wait on the condition @p signal with the mutex @p lock (an
 *        ::lh_rail_base_wait_fn).
 *
 * ::lh_rail_base_wait_forever waits without a timeout; any other value is
 * ::lh_os_condition_wait_for.
 *
 * @param signal     A condition from ::lh_os_rail_base_signal_open.
 * @param lock       A mutex from ::lh_os_rail_base_lock_open, held once.
 * @param timeout_ms Longest wait, or ::lh_rail_base_wait_forever.
 * @return ::lh_bool_true with @p lock held again; ::lh_bool_false on an OS failure.
 */
lh_bool_t
lh_os_rail_base_signal_wait(lh_ptr signal, lh_ptr lock, lh_u32_t timeout_ms);

/**
 * @brief Wake every waiter on the condition @p signal (an ::lh_rail_base_handle_fn).
 * @param signal A condition from ::lh_os_rail_base_signal_open.
 * @return ::lh_bool_false on an OS failure.
 */
lh_bool_t
lh_os_rail_base_signal_wake(lh_ptr signal);

/**
 * @brief Start an ::lh_os_thread_t running @p run (an ::lh_rail_base_start_fn).
 * @param run   The body; must not be null.
 * @param user  The value @p run is given.
 * @param cpu   Logical CPU, or -1 to inherit (lh/os/thread.h).
 * @param flags Zero or ::lh_os_thread_block_sigpipe.
 * @return The thread, from the runtime allocator; ::lh_null when there is no
 *         memory or the OS does not start it (@p run is not called).
 */
lh_ptr
lh_os_rail_base_thread_start(lh_rail_base_run_cb run, lh_ptr user, lh_s32_t cpu,
                             lh_u32_t flags);

/**
 * @brief Join the thread @p thread and free it (an ::lh_rail_base_handle_fn).
 * @param thread A thread from ::lh_os_rail_base_thread_start; not the caller.
 * @return ::lh_bool_false when the join fails (the thread is kept, not freed).
 */
lh_bool_t
lh_os_rail_base_thread_join(lh_ptr thread);

/**
 * @brief ::lh_os_thread_sleep (an ::lh_rail_base_sleep_fn).
 * @param milliseconds The shortest pause.
 * @return ::lh_bool_false on an OS failure.
 */
lh_bool_t
lh_os_rail_base_sleep(lh_u32_t milliseconds);

/**
 * @brief The OS rail base: every call above in its place.
 * @return A constant base with every group; copy it into the slot.
 */
const lh_rail_base_t *
lh_os_rail_base_get(lh_void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_RAIL_BASE_H */
