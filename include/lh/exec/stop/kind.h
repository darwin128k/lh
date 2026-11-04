/**
 * @file kind.h
 * @brief How an executor stops: run what it accepted, or drop what has not
 *        started.
 *
 * One rule for every executor. In both kinds the executor refuses new tasks at
 * once, the task that is running finishes, and every accepted task reaches its
 * mailbox, so its `destroy` runs on the owner's thread at dispatch:
 *
 * - drain: every accepted task runs; dispatch delivers `complete`, then
 *   `destroy`;
 * - cancel: tasks not yet started are marked cancelled; dispatch delivers
 *   `destroy` only.
 *
 * A stop never waits for the owner to dispatch — the owner is usually the
 * thread that stops — so the owner dispatches after the stop returns.
 */

#ifndef LH_EXEC_STOP_KIND_H
#define LH_EXEC_STOP_KIND_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_exec_stop_kind_t
 * @brief How an executor stops.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_exec_stop_kind_*`
 * constants below.
 */
typedef lh_u8_t lh_exec_stop_kind_t;

/**
 * @def lh_exec_stop_kind_none
 * @brief Not stopping: the executor accepts and runs tasks. The state of a
 *        running executor, never an argument to a stop.
 */
#define lh_exec_stop_kind_none 0U

/**
 * @def lh_exec_stop_kind_drain
 * @brief Run every accepted task, then end.
 */
#define lh_exec_stop_kind_drain 1U

/**
 * @def lh_exec_stop_kind_cancel
 * @brief Finish the running task, cancel the ones not started, then end.
 */
#define lh_exec_stop_kind_cancel 2U

#endif /* LH_EXEC_STOP_KIND_H */
