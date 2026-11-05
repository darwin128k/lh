/**
 * @file loop.h
 * @brief ::lh_rail_loop_t — a thread that sleeps until there is work, then does one step.
 *
 * The base of every background thread: a log writer, a job worker, a timer thread.
 * The thread runs the step, fires the loop's due timers, and sleeps until one of:
 * a wake, the nearest timer of its group, the end of its period, or a stop. It
 * never spins.
 *
 * Two ways to wake it, from any thread. ::lh_rail_loop_wake is never lost: it takes
 * the loop's lock for a moment. ::lh_rail_loop_hint takes no lock at all, for a
 * producer that must not wait even that long (a game's main thread); a hint that
 * lands just as the thread goes to sleep is missed, and the step then runs at the
 * end of the period.
 *
 * By default the thread sleeps on a signal of the rail base. A loop made with a waiter
 * (::lh_rail_loop_try_create_waiting) sleeps in the waiter's `wait` instead — on a
 * `poll` of its sockets and a `lh/os/wake.h` signal, say — and every wake, hint and
 * stop calls the waiter's `wake`. Then no wake is lost: the signal stays pending
 * until the waiter consumes it.
 *
 * The loop's timers (::lh_rail_loop_get_timers_mut) are touched by the loop's thread
 * only: add them before ::lh_rail_loop_try_start or from inside a step.
 *
 * Ending. ::lh_rail_loop_destroy stops, joins and frees, waiting as long as the last
 * step takes. An owner that must not wait that long (a game thread unloading a module
 * while the step is stuck in a call on a network share) stops with
 * ::lh_rail_loop_try_stop and a timeout. When the time runs out it gives the thread up
 * with ::lh_rail_loop_try_abandon: from then on the owner touches neither the loop nor
 * the step's state; the thread finishes its step, runs one last step, frees that state
 * through the release call, and the owner later frees the record with
 * ::lh_rail_loop_try_reap, which never waits for work (lh/rail/loop/state.h).
 *
 * The thread, the lock and the signal come from the rail base slot
 * (lh/rail/runtime/base.h): creating a loop needs its clock, lock and signal
 * groups, starting it the thread group; without them the `try_` forms refuse.
 */

#ifndef LH_RAIL_LOOP_H
#define LH_RAIL_LOOP_H

#include <lh/core/atomic/uint.h>
#include <lh/core/memory/allocator.h>
#include <lh/rail/loop/release/cb.h>
#include <lh/rail/loop/state.h>
#include <lh/rail/loop/step/cb.h>
#include <lh/rail/loop/wait/cb.h>
#include <lh/rail/loop/wake/cb.h>
#include <lh/core/timer/group.h>

/** @brief A worker loop; opaque. */
typedef struct lh_rail_loop lh_rail_loop_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The loop's timers, fired on its thread before each step.
 *
 * @param self The loop.
 * @return The group; the loop's thread's alone once it runs.
 */
lh_timer_group_t *
lh_rail_loop_get_timers_mut(lh_rail_loop_t *self);

/**
 * @brief Whether the step running now is the last one: a stop was asked before it.
 *
 * For the step itself; another thread reads nothing meaningful.
 *
 * @param self The loop.
 */
lh_bool_t
lh_rail_loop_is_last_step(const lh_rail_loop_t *self);

/**
 * @brief Allocator mut (internal).
 */
lh_memory_allocator_t *
lh_rail_loop_get_allocator_mut(lh_rail_loop_t *self);

/**
 * @brief Step fn (internal).
 */
lh_rail_loop_step_cb
lh_rail_loop_get_step_fn(const lh_rail_loop_t *self);

/**
 * @brief Lh os loop set step fn (internal).
 */
lh_void
lh_rail_loop_set_step_fn(lh_rail_loop_t *self, lh_rail_loop_step_cb step);

/**
 * @brief User (internal).
 */
lh_ptr
lh_rail_loop_get_user(const lh_rail_loop_t *self);

/**
 * @brief Lh os loop set user (internal).
 */
lh_void
lh_rail_loop_set_user(lh_rail_loop_t *self, lh_ptr user);

/**
 * @brief Period ms (internal).
 */
lh_u32_t
lh_rail_loop_get_period_ms(const lh_rail_loop_t *self);

/**
 * @brief Lh os loop set period ms (internal).
 */
lh_void
lh_rail_loop_set_period_ms(lh_rail_loop_t *self, lh_u32_t period_ms);

/**
 * @brief The loop's thread (level 1).
 * @param self The loop.
 * @return The thread's rail base handle, or ::lh_null while not started.
 */
lh_ptr
lh_rail_loop_get_thread(const lh_rail_loop_t *self);

/**
 * @brief Store the loop's thread (level 1).
 * @param self   The loop.
 * @param thread A thread's rail base handle, or ::lh_null.
 */
lh_void
lh_rail_loop_set_thread(lh_rail_loop_t *self, lh_ptr thread);

/**
 * @brief The loop's lock, guarding the pending wake and the stop (level 1).
 * @param self The loop.
 * @return The lock's rail base handle, or ::lh_null while not made.
 */
lh_ptr
lh_rail_loop_get_lock(const lh_rail_loop_t *self);

/**
 * @brief Store the loop's lock (level 1).
 * @param self The loop.
 * @param lock A lock's rail base handle, or ::lh_null.
 */
lh_void
lh_rail_loop_set_lock(lh_rail_loop_t *self, lh_ptr lock);

/**
 * @brief The signal the loop's thread sleeps on (level 1).
 * @param self The loop.
 * @return The signal's rail base handle, or ::lh_null while not made.
 */
lh_ptr
lh_rail_loop_get_signal(const lh_rail_loop_t *self);

/**
 * @brief Store the signal the loop's thread sleeps on (level 1).
 * @param self   The loop.
 * @param signal A signal's rail base handle, or ::lh_null.
 */
lh_void
lh_rail_loop_set_signal(lh_rail_loop_t *self, lh_ptr signal);

/**
 * @brief Lh os loop is started (internal).
 */
lh_bool_t
lh_rail_loop_is_started(const lh_rail_loop_t *self);

/**
 * @brief Lh os loop set started (internal).
 */
lh_void
lh_rail_loop_set_started(lh_rail_loop_t *self, lh_bool_t started);

/**
 * @brief Clear the pending wake and say whether there was one.
 */
lh_bool_t
lh_rail_loop_take_pending(lh_rail_loop_t *self);

/**
 * @brief Lh os loop is pending (internal).
 */
lh_bool_t
lh_rail_loop_is_pending(lh_rail_loop_t *self);

/**
 * @brief Lh os loop set pending (internal).
 */
lh_void
lh_rail_loop_set_pending(lh_rail_loop_t *self);

/**
 * @brief Lh os loop set last step (internal).
 */
lh_void
lh_rail_loop_set_last_step(lh_rail_loop_t *self, lh_bool_t last_step);

/**
 * @brief Lh os loop is stopping (internal).
 */
lh_bool_t
lh_rail_loop_is_stopping(const lh_rail_loop_t *self);

/**
 * @brief Lh os loop set stopping (internal).
 */
lh_void
lh_rail_loop_set_stopping(lh_rail_loop_t *self);

/**
 * @brief Lh os loop init flags (internal).
 */
lh_void
lh_rail_loop_init_flags(lh_rail_loop_t *self);

/**
 * @brief The loop's end state (level 1).
 *
 * @param self The loop.
 * @return The counter that holds an ::lh_rail_loop_state_t; change it with the atomic
 *         brick.
 */
lh_atomic_uint_t *
lh_rail_loop_get_state_mut(lh_rail_loop_t *self);

/**
 * @brief The call an abandoned thread frees the step's state with (level 1).
 *
 * @param self The loop.
 * @return The release call, or ::lh_null for none.
 */
lh_rail_loop_release_cb
lh_rail_loop_get_release_fn(const lh_rail_loop_t *self);

/**
 * @brief Store the call an abandoned thread frees the step's state with (level 1).
 *
 * @param self    The loop.
 * @param release The release call (may be ::lh_null).
 */
lh_void
lh_rail_loop_set_release_fn(lh_rail_loop_t *self, lh_rail_loop_release_cb release);

/**
 * @brief The value the release call is given (level 1).
 *
 * @param self The loop.
 * @return The value.
 */
lh_ptr
lh_rail_loop_get_release_user(const lh_rail_loop_t *self);

/**
 * @brief Store the value the release call is given (level 1).
 *
 * @param self         The loop.
 * @param release_user The value.
 */
lh_void
lh_rail_loop_set_release_user(lh_rail_loop_t *self, lh_ptr release_user);

/**
 * @brief ::lh_rail_loop_try_start with the thread's CPU and flags, hints the rail
 *        base may ignore (lh/rail/base/start/fn.h).
 *
 * @param self  The loop, created and not started.
 * @param cpu   Logical CPU index, or -1 to inherit affinity.
 * @param flags The base's own options (the OS base: zero or
 *              ::lh_os_thread_block_sigpipe).
 * @return ::lh_bool_false when the rail base has no thread group or the thread
 *         could not start.
 */
lh_bool_t
lh_rail_loop_try_start_on(lh_rail_loop_t *self, lh_s32_t cpu, lh_u32_t flags);

/**
 * @brief Take the loop's lock; a failure is ignored.
 * @param self The loop, created.
 */
lh_void
lh_rail_loop_lock(lh_rail_loop_t *self);

/**
 * @brief Give the loop's lock back; a failure is ignored.
 * @param self The loop, its lock held by the caller.
 */
lh_void
lh_rail_loop_unlock(lh_rail_loop_t *self);

/**
 * @brief Wake the loop's thread if it sleeps on its signal; a failure is ignored.
 * @param self The loop, created.
 */
lh_void
lh_rail_loop_signal(lh_rail_loop_t *self);

/**
 * @brief The loop's end state now, read with acquire order.
 *
 * @param self The loop.
 * @return An ::lh_rail_loop_state_t.
 */
lh_rail_loop_state_t
lh_rail_loop_load_state(lh_rail_loop_t *self);

/**
 * @brief The end of the loop's thread: settle who frees the step's state.
 *
 * Runs on the loop's thread after its last step. When the owner still holds the
 * loop, marks it ended; when the owner abandoned it, runs the release call and marks
 * it released. Nothing touches the loop afterwards but the owner's reap.
 *
 * @param self The loop.
 */
lh_void
lh_rail_loop_end(lh_rail_loop_t *self);

/**
 * @brief Free the loop's timers, its lock and signal and its record.
 *
 * The thread must be joined or never started.
 *
 * @param self The loop.
 */
lh_void
lh_rail_loop_free(lh_rail_loop_t *self);

/**
 * @brief Give up a stopping loop whose step does not return.
 *
 * After a stop was asked (::lh_rail_loop_try_stop). On success the owner must touch
 * neither the loop nor the step's state again, except to reap the loop: the thread
 * calls @p release with @p release_user when it ends, then the loop can be reaped
 * (::lh_rail_loop_try_reap). On failure the thread ended just now: destroy as usual.
 *
 * @param self         The loop, stopping.
 * @param release      Frees the step's state, on the loop's thread (may be ::lh_null).
 * @param release_user The value @p release is given.
 * @return ::lh_bool_true when the thread now frees the state itself;
 *         ::lh_bool_false when it had ended already.
 */
lh_bool_t
lh_rail_loop_try_abandon(lh_rail_loop_t *self, lh_rail_loop_release_cb release,
                         lh_ptr release_user);

/**
 * @brief Have the thread run its step soon; never lost.
 *
 * Takes the loop's mutex for a moment. Any thread may call it, many times: wakes
 * that come together run one step.
 *
 * @param self The loop.
 */
lh_void
lh_rail_loop_wake(lh_rail_loop_t *self);

/**
 * @brief ::lh_rail_loop_wake without any lock; may be missed for one period.
 *
 * For a producer that must never wait. The step runs soon, or at the latest at the
 * end of the period.
 *
 * @param self The loop.
 */
lh_void
lh_rail_loop_hint(lh_rail_loop_t *self);

/**
 * @brief Ask the thread to run one last step and end; does not wait.
 *
 * @param self The loop, started.
 */
lh_void
lh_rail_loop_ask_stop(lh_rail_loop_t *self);

/**
 * @brief Sleep at most @p timeout_ms, unless a wake or a stop is already there.
 */
lh_void
lh_rail_loop_sleep(lh_rail_loop_t *self, lh_u32_t timeout_ms);

/**
 * @brief Whether the thread has run its last step while the owner held it.
 *
 * @param self The loop.
 * @return ::lh_bool_true once the state is ::lh_rail_loop_state_ended.
 */
lh_bool_t
lh_rail_loop_is_ended(lh_rail_loop_t *self);

/**
 * @brief Free an abandoned loop once its thread has released its state; never waits
 *        for work.
 *
 * Call it from time to time (a game frame) for every abandoned loop. On
 * ::lh_bool_true the loop is gone.
 *
 * @param self An abandoned loop.
 * @return ::lh_bool_true when the thread had ended and the loop was freed;
 *         ::lh_bool_false when the step still runs (call again later).
 */
lh_bool_t
lh_rail_loop_try_reap(lh_rail_loop_t *self);

/**
 * @brief Start the thread; the first step runs at once.
 *
 * @param self The loop, created and not started.
 * @return ::lh_bool_false when the rail base has no thread group or the thread
 *         could not start.
 */
lh_bool_t
lh_rail_loop_try_start(lh_rail_loop_t *self);

/**
 * @brief Stop the thread after one last step, join it and free the loop.
 *
 * Call it from another thread than the loop's. A loop never started is just freed.
 *
 * @param self The loop, or ::lh_null.
 */
lh_void
lh_rail_loop_destroy(lh_rail_loop_t *self);

/**
 * @brief The body of the loop's thread (an ::lh_rail_base_run_fn).
 * @param user The ::lh_rail_loop_t.
 */
lh_void
lh_rail_loop_run(lh_ptr user);

/**
 * @brief Ask for a stop and wait at most @p timeout_ms for the last step to end.
 *
 * Call it from another thread than the loop's. On ::lh_bool_true call
 * ::lh_rail_loop_destroy, which joins at once. On ::lh_bool_false the step is still
 * running: wait again, destroy (and wait without a bound), or give the thread up with
 * ::lh_rail_loop_try_abandon.
 *
 * @param self       The loop.
 * @param timeout_ms Longest wait, in milliseconds (0: do not wait).
 * @return ::lh_bool_true when the thread has ended (or never started).
 */
lh_bool_t
lh_rail_loop_try_stop(lh_rail_loop_t *self, lh_u32_t timeout_ms);

/**
 * @brief Create a loop, without its thread yet.
 *
 * @param period_ms Longest sleep, in milliseconds; not 0.
 * @param step      The step; not ::lh_null.
 * @param user      The value the step is given.
 * @return The loop, or ::lh_null when an argument is wrong, the rail base lacks the
 *         clock, lock or signal group, or memory or a base object is short.
 */
lh_rail_loop_t *
lh_rail_loop_try_create(lh_u32_t period_ms, lh_rail_loop_step_cb step, lh_ptr user);

/**
 * @brief ::lh_rail_loop_try_create for a loop that sleeps in @p wait, woken by @p wake.
 *
 * @param period_ms Longest sleep, in milliseconds; not 0.
 * @param step      The step; not ::lh_null.
 * @param user      The value the step is given.
 * @param wait      How the thread sleeps; not ::lh_null.
 * @param wake      How a sleep is cut short, from any thread; not ::lh_null.
 * @param waiter    The value @p wait and @p wake are given.
 * @return The loop, or ::lh_null as ::lh_rail_loop_try_create.
 */
lh_rail_loop_t *
lh_rail_loop_try_create_waiting(lh_u32_t period_ms, lh_rail_loop_step_cb step, lh_ptr user,
                                lh_rail_loop_wait_cb wait, lh_rail_loop_wake_cb wake,
                                lh_ptr waiter);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_LOOP_H */
