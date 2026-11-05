/**
 * @file base.h
 * @brief ::lh_rail_base_t — what rail needs from a platform, as one table of calls.
 *
 * rail itself is portable: tasks, queues, mailboxes, executors and loops are
 * plain code. Where it has to sleep, wake, take a lock, start a thread or read
 * the time it calls the base, and only the base. Each platform fills the table
 * its own way — the OS one over threads, mutexes and conditions
 * (lh/os/rail/base.h), a board without an OS over its tick, interrupt masking
 * and wait-for-event — and rail does not know which one it runs on. What every
 * call promises is written on its type (lh/rail/base/<call>/fn.h); a platform
 * keeps those promises however it can.
 *
 * The calls come in groups: the clock, the lock, the signal a thread sleeps on,
 * the thread. A group is there only when every call of it is
 * (::lh_rail_base_has_clock and its siblings); a platform without threads
 * leaves that group empty, and rail's `try_` forms that need it refuse.
 *
 * The record is a plain value: a platform keeps one constant, the host copies
 * it into the slot rail reads (lh/rail/runtime/base.h) before it starts any
 * thread. Objects the base makes are named by handles (::lh_ptr); the base owns
 * what is behind them from the open or start to the close or join.
 *
 * The calls below (::lh_rail_base_now_ms and on) are the fast forms: they call
 * the member as it is and check nothing. Check the group once, where a whole
 * mechanism starts, not on every call.
 */

#ifndef LH_RAIL_BASE_H
#define LH_RAIL_BASE_H

#include <lh/rail/base/fields.h>
#include <lh/rail/base/handle/cb.h>
#include <lh/rail/base/now/cb.h>
#include <lh/rail/base/open/cb.h>
#include <lh/rail/base/sleep/cb.h>
#include <lh/rail/base/start/cb.h>
#include <lh/rail/base/wait/cb.h>

/**
 * @brief The calls a platform gives rail: clock, lock, signal, thread.
 */
struct lh_rail_base
{
    /** Members: ::lh_rail_base_fields. */
    lh_rail_base_fields(lh_rail_base_now_fn, lh_rail_base_open_fn, lh_rail_base_handle_fn,
                        lh_rail_base_wait_fn, lh_rail_base_start_fn, lh_rail_base_sleep_fn);
};

/** @copybrief lh_rail_base */
typedef struct lh_rail_base lh_rail_base_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The clock (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null when the platform has no clock.
 */
lh_rail_base_now_cb
lh_rail_base_get_now_cb(const lh_rail_base_t *self);

/**
 * @brief Store the clock (level 1).
 * @param self   Base; must not be null.
 * @param now_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_now_cb(lh_rail_base_t *self, lh_rail_base_now_cb now_cb);

/**
 * @brief The call that makes a lock (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_open_cb
lh_rail_base_get_lock_open_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that makes a lock (level 1).
 * @param self         Base; must not be null.
 * @param lock_open_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_lock_open_cb(lh_rail_base_t *self, lh_rail_base_open_cb lock_open_cb);

/**
 * @brief The call that closes and frees a lock (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_handle_cb
lh_rail_base_get_lock_close_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that closes and frees a lock (level 1).
 * @param self          Base; must not be null.
 * @param lock_close_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_lock_close_cb(lh_rail_base_t *self, lh_rail_base_handle_cb lock_close_cb);

/**
 * @brief The call that takes a lock, waiting while another thread holds it (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_handle_cb
lh_rail_base_get_lock_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that takes a lock (level 1).
 * @param self    Base; must not be null.
 * @param lock_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_lock_cb(lh_rail_base_t *self, lh_rail_base_handle_cb lock_cb);

/**
 * @brief The call that gives a held lock back (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_handle_cb
lh_rail_base_get_unlock_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that gives a held lock back (level 1).
 * @param self      Base; must not be null.
 * @param unlock_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_unlock_cb(lh_rail_base_t *self, lh_rail_base_handle_cb unlock_cb);

/**
 * @brief The call that makes a signal (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_open_cb
lh_rail_base_get_signal_open_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that makes a signal (level 1).
 * @param self           Base; must not be null.
 * @param signal_open_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_signal_open_cb(lh_rail_base_t *self, lh_rail_base_open_cb signal_open_cb);

/**
 * @brief The call that closes and frees a signal (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_handle_cb
lh_rail_base_get_signal_close_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that closes and frees a signal (level 1).
 * @param self            Base; must not be null.
 * @param signal_close_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_signal_close_cb(lh_rail_base_t *self, lh_rail_base_handle_cb signal_close_cb);

/**
 * @brief The call that sleeps on a signal (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_wait_cb
lh_rail_base_get_signal_wait_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that sleeps on a signal (level 1).
 * @param self           Base; must not be null.
 * @param signal_wait_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_signal_wait_cb(lh_rail_base_t *self, lh_rail_base_wait_cb signal_wait_cb);

/**
 * @brief The call that wakes every thread sleeping on a signal (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_handle_cb
lh_rail_base_get_signal_wake_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that wakes a signal's sleepers (level 1).
 * @param self           Base; must not be null.
 * @param signal_wake_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_signal_wake_cb(lh_rail_base_t *self, lh_rail_base_handle_cb signal_wake_cb);

/**
 * @brief The call that starts a thread (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null when the platform has no threads.
 */
lh_rail_base_start_cb
lh_rail_base_get_thread_start_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that starts a thread (level 1).
 * @param self            Base; must not be null.
 * @param thread_start_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_thread_start_cb(lh_rail_base_t *self, lh_rail_base_start_cb thread_start_cb);

/**
 * @brief The call that waits for a thread to end and frees it (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_handle_cb
lh_rail_base_get_thread_join_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that joins a thread (level 1).
 * @param self           Base; must not be null.
 * @param thread_join_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_thread_join_cb(lh_rail_base_t *self, lh_rail_base_handle_cb thread_join_cb);

/**
 * @brief The call that pauses the calling thread (level 1).
 * @param self Base; must not be null.
 * @return The call, or ::lh_null.
 */
lh_rail_base_sleep_cb
lh_rail_base_get_sleep_cb(const lh_rail_base_t *self);

/**
 * @brief Store the call that pauses the calling thread (level 1).
 * @param self     Base; must not be null.
 * @param sleep_cb The call (may be ::lh_null).
 */
lh_void
lh_rail_base_set_sleep_cb(lh_rail_base_t *self, lh_rail_base_sleep_cb sleep_cb);

/**
 * @brief Whether @p self has a clock.
 * @param self Base; must not be null.
 * @return ::lh_bool_true when ::lh_rail_base_now_ms may be called.
 */
lh_bool_t
lh_rail_base_has_clock(const lh_rail_base_t *self);

/**
 * @brief Whether @p self has the whole lock group: open, close, lock, unlock.
 * @param self Base; must not be null.
 * @return ::lh_bool_true when every lock call is there.
 */
lh_bool_t
lh_rail_base_has_lock(const lh_rail_base_t *self);

/**
 * @brief Whether @p self has the whole signal group: open, close, wait, wake.
 * @param self Base; must not be null.
 * @return ::lh_bool_true when every signal call is there.
 */
lh_bool_t
lh_rail_base_has_signal(const lh_rail_base_t *self);

/**
 * @brief Whether @p self has the whole thread group: start, join, sleep.
 * @param self Base; must not be null.
 * @return ::lh_bool_true when every thread call is there.
 */
lh_bool_t
lh_rail_base_has_thread(const lh_rail_base_t *self);

/**
 * @brief Copy every call of @p other into @p self.
 * @param self  Destination; must not be null.
 * @param other Source; must not be null. May be @p self.
 */
lh_void
lh_rail_base_assign(lh_rail_base_t *self, const lh_rail_base_t *other);

/**
 * @brief Read the clock of @p self (fast form: the clock must be there).
 * @param self Base with a clock; must not be null.
 * @return Milliseconds of its monotonic clock (::lh_rail_base_now_fn).
 */
lh_u64_t
lh_rail_base_now_ms(const lh_rail_base_t *self);

/**
 * @brief Make a lock (fast form: the lock group must be there).
 * @param self Base with the lock group; must not be null.
 * @return The lock's handle, or ::lh_null when it cannot be made.
 */
lh_ptr
lh_rail_base_lock_open(const lh_rail_base_t *self);

/**
 * @brief Close and free a lock nobody holds.
 * @param self Base with the lock group; must not be null.
 * @param lock The lock's handle; the handle names nothing afterwards.
 * @return ::lh_bool_false on a failure.
 */
lh_bool_t
lh_rail_base_lock_close(const lh_rail_base_t *self, lh_ptr lock);

/**
 * @brief Take a lock, waiting while another thread holds it.
 * @param self Base with the lock group; must not be null.
 * @param lock The lock's handle, not held by the caller.
 * @return ::lh_bool_false on a failure (the lock is not taken).
 */
lh_bool_t
lh_rail_base_lock(const lh_rail_base_t *self, lh_ptr lock);

/**
 * @brief Give a held lock back.
 * @param self Base with the lock group; must not be null.
 * @param lock The lock's handle, held by the caller.
 * @return ::lh_bool_false on a failure.
 */
lh_bool_t
lh_rail_base_unlock(const lh_rail_base_t *self, lh_ptr lock);

/**
 * @brief Make a signal (fast form: the signal group must be there).
 * @param self Base with the signal group; must not be null.
 * @return The signal's handle, or ::lh_null when it cannot be made.
 */
lh_ptr
lh_rail_base_signal_open(const lh_rail_base_t *self);

/**
 * @brief Close and free a signal nobody sleeps on.
 * @param self   Base with the signal group; must not be null.
 * @param signal The signal's handle; the handle names nothing afterwards.
 * @return ::lh_bool_false on a failure.
 */
lh_bool_t
lh_rail_base_signal_close(const lh_rail_base_t *self, lh_ptr signal);

/**
 * @brief Sleep on @p signal, @p lock given back meanwhile (::lh_rail_base_wait_fn).
 * @param self       Base with the signal group; must not be null.
 * @param signal     The signal's handle.
 * @param lock       The lock guarding what the caller waits for, held once.
 * @param timeout_ms Longest sleep, or ::lh_rail_base_wait_forever.
 * @return ::lh_bool_true with @p lock held again after a wake, the timeout or an
 *         early return; ::lh_bool_false on a failure.
 */
lh_bool_t
lh_rail_base_signal_wait(const lh_rail_base_t *self, lh_ptr signal, lh_ptr lock,
                         lh_u32_t timeout_ms);

/**
 * @brief Wake every thread sleeping on @p signal now; a wake with no sleeper is
 *        not kept.
 * @param self   Base with the signal group; must not be null.
 * @param signal The signal's handle.
 * @return ::lh_bool_false on a failure.
 */
lh_bool_t
lh_rail_base_signal_wake(const lh_rail_base_t *self, lh_ptr signal);

/**
 * @brief Start a thread running @p run (fast form: the thread group must be there).
 * @param self  Base with the thread group; must not be null.
 * @param run   The body; must not be null.
 * @param user  The value @p run is given.
 * @param cpu   Logical CPU, or -1 for any (a hint).
 * @param flags The base's own options, 0 for none (a hint).
 * @return The thread's handle for the join, or ::lh_null when none started.
 */
lh_ptr
lh_rail_base_thread_start(const lh_rail_base_t *self, lh_rail_base_run_cb run, lh_ptr user,
                          lh_s32_t cpu, lh_u32_t flags);

/**
 * @brief Wait for a thread to end and free it; never from that thread itself.
 * @param self   Base with the thread group; must not be null.
 * @param thread The thread's handle; on success it names nothing afterwards.
 * @return ::lh_bool_false on a failure (the thread is still the caller's).
 */
lh_bool_t
lh_rail_base_thread_join(const lh_rail_base_t *self, lh_ptr thread);

/**
 * @brief Pause the calling thread for at least @p milliseconds.
 * @param self         Base with the thread group; must not be null.
 * @param milliseconds The shortest pause.
 * @return ::lh_bool_false on a failure.
 */
lh_bool_t
lh_rail_base_sleep(const lh_rail_base_t *self, lh_u32_t milliseconds);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_BASE_H */
