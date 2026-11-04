/**
 * @file state.h
 * @brief Who is the last to hold a loop's thread: its owner, or the thread itself.
 *
 * A loop's thread runs until its stop; its owner usually waits for that and joins.
 * When the owner cannot wait (the step is stuck in a call that will return only
 * later), the two decide by one compare-exchange who frees the step's state:
 *
 * - running → ended: the thread finished first; the owner destroys as usual;
 * - running → abandoned: the owner gave up first; the thread, when it ends, frees
 *   the step's state itself and moves to released; the owner reaps the record.
 */

#ifndef LH_EXEC_LOOP_STATE_H
#define LH_EXEC_LOOP_STATE_H

#include <lh/core/numeric/types.h>

/**
 * @typedef lh_exec_loop_state_t
 * @brief Where a loop's thread is in its end.
 *
 * Alias for ::lh_uint_t (it lives in an ::lh_atomic_uint_t); canonical values are the
 * `lh_exec_loop_state_*` constants below.
 */
typedef lh_uint_t lh_exec_loop_state_t;

/**
 * @def lh_exec_loop_state_running
 * @brief The thread runs (or never started); nobody has given anything up.
 */
#define lh_exec_loop_state_running 0U

/**
 * @def lh_exec_loop_state_abandoned
 * @brief The owner gave up waiting; the thread still runs and will free the step's
 *        state itself.
 */
#define lh_exec_loop_state_abandoned 1U

/**
 * @def lh_exec_loop_state_ended
 * @brief The thread ran its last step while the owner still held it; the owner joins.
 */
#define lh_exec_loop_state_ended 2U

/**
 * @def lh_exec_loop_state_released
 * @brief An abandoned thread ran its last step and freed the step's state; the
 *        owner may reap the record.
 */
#define lh_exec_loop_state_released 3U

#endif /* LH_EXEC_LOOP_STATE_H */
