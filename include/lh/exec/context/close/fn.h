/**
 * @file fn.h
 * @brief What an executor thread closes when it stops.
 */

#ifndef LH_EXEC_CONTEXT_CLOSE_FN_H
#define LH_EXEC_CONTEXT_CLOSE_FN_H

#include <lh/core/ptr.h>

/**
 * @typedef lh_exec_context_close_fn
 * @brief Function type `void (lh_ptr user, lh_ptr local)` — close the thread's
 *        state.
 *
 * Runs once on the executor's thread, after its last task, on the same thread
 * that opened it (`mysql_thread_end` must). @p user is the context's own
 * pointer; @p local is what open returned. Not a pointer type by itself; use
 * ::lh_exec_context_close_cb.
 */
typedef lh_void(lh_exec_context_close_fn)(lh_ptr user, lh_ptr local);

#endif /* LH_EXEC_CONTEXT_CLOSE_FN_H */
