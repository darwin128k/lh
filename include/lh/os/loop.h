/**
 * @file loop.h
 * @brief ::lh_os_loop_t — a thread that sleeps until there is work, then does one step.
 *
 * The base of every background thread: a log writer, a job worker, a timer thread.
 * The thread runs the step, fires the loop's due timers, and sleeps until one of:
 * a wake, the nearest timer of its group, the end of its period, or a stop. It
 * never spins.
 *
 * Two ways to wake it, from any thread. ::lh_os_loop_wake is never lost: it takes
 * the loop's mutex for a moment. ::lh_os_loop_hint takes no lock at all, for a
 * producer that must not wait even that long (a game's main thread); a hint that
 * lands just as the thread goes to sleep is missed, and the step then runs at the
 * end of the period.
 *
 * By default the thread sleeps on a condition. A loop made with a waiter
 * (::lh_os_loop_try_create_waiting) sleeps in the waiter's `wait` instead — on a
 * `poll` of its sockets and a `lh/os/wake.h` signal, say — and every wake, hint and
 * stop calls the waiter's `wake`. Then no wake is lost: the signal stays pending
 * until the waiter consumes it.
 *
 * The loop's timers (::lh_os_loop_get_timers_mut) are touched by the loop's thread
 * only: add them before ::lh_os_loop_try_start or from inside a step.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOOP_H
#define LH_OS_LOOP_H

#include <lh/config.h>
#include <lh/os/loop/step/cb.h>
#include <lh/os/loop/wait/cb.h>
#include <lh/os/loop/wake/cb.h>
#include <lh/timer/group.h>
#include <lh/memory/allocator.h>
#include <lh/os/thread.h>
#include <lh/os/condition.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/loop.h requires LH_LIBRARY_OPTION_OS"
#endif

/** @brief A worker loop; opaque. */
typedef struct lh_os_loop lh_os_loop_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief ::lh_os_loop_try_create for a loop that sleeps in @p wait, woken by @p wake.
 *
 * @param period_ms Longest sleep, in milliseconds; not 0.
 * @param step      The step; not ::lh_null.
 * @param user      The value the step is given.
 * @param wait      How the thread sleeps; not ::lh_null.
 * @param wake      How a sleep is cut short, from any thread; not ::lh_null.
 * @param waiter    The value @p wait and @p wake are given.
 * @return The loop, or ::lh_null as ::lh_os_loop_try_create.
 */
lh_os_loop_t *
lh_os_loop_try_create_waiting(lh_u32_t period_ms, lh_os_loop_step_cb step, lh_ptr user,
                              lh_os_loop_wait_cb wait, lh_os_loop_wake_cb wake, lh_ptr waiter);

/**
 * @brief The loop's timers, fired on its thread before each step.
 *
 * @param self The loop.
 * @return The group; the loop's thread's alone once it runs.
 */
lh_timer_group_t *
lh_os_loop_get_timers_mut(lh_os_loop_t *self);

/**
 * @brief Whether the step running now is the last one: a stop was asked before it.
 *
 * For the step itself; another thread reads nothing meaningful.
 *
 * @param self The loop.
 */
lh_bool_t
lh_os_loop_is_last_step(const lh_os_loop_t *self);

/**
 * @brief Start the thread; the first step runs at once.
 *
 * @param self The loop, created and not started.
 * @return ::lh_bool_false when the thread could not start.
 */
lh_bool_t
lh_os_loop_try_start(lh_os_loop_t *self);

/**
 * @brief ::lh_os_loop_try_start with the thread's CPU and flags (`lh/os/thread.h`).
 *
 * @param self  The loop, created and not started.
 * @param cpu   Logical CPU index, or -1 to inherit affinity.
 * @param flags Zero or ::lh_os_thread_block_sigpipe.
 * @return ::lh_bool_false when the thread could not start.
 */
lh_bool_t
lh_os_loop_try_start_on(lh_os_loop_t *self, lh_s32_t cpu, lh_u32_t flags);

/**
 * @brief Have the thread run its step soon; never lost.
 *
 * Takes the loop's mutex for a moment. Any thread may call it, many times: wakes
 * that come together run one step.
 *
 * @param self The loop.
 */
lh_void
lh_os_loop_wake(lh_os_loop_t *self);

/**
 * @brief ::lh_os_loop_wake without any lock; may be missed for one period.
 *
 * For a producer that must never wait. The step runs soon, or at the latest at the
 * end of the period.
 *
 * @param self The loop.
 */
lh_void
lh_os_loop_hint(lh_os_loop_t *self);

/**
 * @brief Stop the thread after one last step, join it and free the loop.
 *
 * Call it from another thread than the loop's. A loop never started is just freed.
 *
 * @param self The loop, or ::lh_null.
 */
lh_void
lh_os_loop_destroy(lh_os_loop_t *self);

/**
 * @brief Create a loop, without its thread yet.
 *
 * @param period_ms Longest sleep, in milliseconds; not 0.
 * @param step      The step; not ::lh_null.
 * @param user      The value the step is given.
 * @return The loop, or ::lh_null when an argument is wrong or the system is short of
 *         memory or synchronization objects.
 */
lh_os_loop_t *
lh_os_loop_try_create(lh_u32_t period_ms, lh_os_loop_step_cb step, lh_ptr user);

/**
 * @brief Allocator mut (internal).
 */
lh_memory_allocator_t *
lh_os_loop_get_allocator_mut(lh_os_loop_t *self);

/**
 * @brief Step fn (internal).
 */
lh_os_loop_step_cb
lh_os_loop_get_step_fn(const lh_os_loop_t *self);

/**
 * @brief Lh os loop set step fn (internal).
 */
lh_void
lh_os_loop_set_step_fn(lh_os_loop_t *self, lh_os_loop_step_cb step);

/**
 * @brief User (internal).
 */
lh_ptr
lh_os_loop_get_user(const lh_os_loop_t *self);

/**
 * @brief Lh os loop set user (internal).
 */
lh_void
lh_os_loop_set_user(lh_os_loop_t *self, lh_ptr user);

/**
 * @brief Period ms (internal).
 */
lh_u32_t
lh_os_loop_get_period_ms(const lh_os_loop_t *self);

/**
 * @brief Lh os loop set period ms (internal).
 */
lh_void
lh_os_loop_set_period_ms(lh_os_loop_t *self, lh_u32_t period_ms);

/**
 * @brief Thread mut (internal).
 */
lh_os_thread_t *
lh_os_loop_get_thread_mut(lh_os_loop_t *self);

/**
 * @brief Mutex mut (internal).
 */
lh_os_mutex_t *
lh_os_loop_get_mutex_mut(lh_os_loop_t *self);

/**
 * @brief Condition mut (internal).
 */
lh_os_condition_t *
lh_os_loop_get_condition_mut(lh_os_loop_t *self);

/**
 * @brief Lh os loop is started (internal).
 */
lh_bool_t
lh_os_loop_is_started(const lh_os_loop_t *self);

/**
 * @brief Lh os loop set started (internal).
 */
lh_void
lh_os_loop_set_started(lh_os_loop_t *self, lh_bool_t started);

/**
 * @brief Clear the pending wake and say whether there was one.
 */
lh_bool_t
lh_os_loop_take_pending(lh_os_loop_t *self);

/**
 * @brief Lh os loop is pending (internal).
 */
lh_bool_t
lh_os_loop_is_pending(lh_os_loop_t *self);

/**
 * @brief Lh os loop set pending (internal).
 */
lh_void
lh_os_loop_set_pending(lh_os_loop_t *self);

/**
 * @brief Lh os loop set last step (internal).
 */
lh_void
lh_os_loop_set_last_step(lh_os_loop_t *self, lh_bool_t last_step);

/**
 * @brief Lh os loop is stopping (internal).
 */
lh_bool_t
lh_os_loop_is_stopping(const lh_os_loop_t *self);

/**
 * @brief Lh os loop set stopping (internal).
 */
lh_void
lh_os_loop_set_stopping(lh_os_loop_t *self);

/**
 * @brief Lh os loop init flags (internal).
 */
lh_void
lh_os_loop_init_flags(lh_os_loop_t *self);

/**
 * @brief Sleep at most @p timeout_ms, unless a wake or a stop is already there.
 */
lh_void
lh_os_loop_sleep(lh_os_loop_t *self, lh_u32_t timeout_ms);

/**
 * @brief Lh os loop run (internal).
 */
lh_void
lh_os_loop_run(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOOP_H */
