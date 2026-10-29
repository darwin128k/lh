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
 * The loop's timers (::lh_os_loop_get_timers_mut) are touched by the loop's thread
 * only: add them before ::lh_os_loop_try_start or from inside a step.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOOP_H
#define LH_OS_LOOP_H

#include <lh/config.h>
#include <lh/os/loop/step/cb.h>
#include <lh/timer/group.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/loop.h requires LH_LIBRARY_OPTION_OS"
#endif

/** @brief A worker loop; opaque. */
typedef struct lh_os_loop lh_os_loop_t;

LH_COMPILER_EXTERN_C_BEGIN

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOOP_H */
