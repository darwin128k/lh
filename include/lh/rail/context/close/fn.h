/**
 * @file fn.h
 * @brief What an executor thread closes when it stops.
 */

#ifndef LH_RAIL_CONTEXT_CLOSE_FN_H
#define LH_RAIL_CONTEXT_CLOSE_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_rail_context_close_fn
 * @brief Function type `void (lh_ptr user, lh_ptr local)` — close the thread's
 *        state.
 *
 * Runs once on the executor's thread, after its last task, on the same thread
 * that opened it (`mysql_thread_end` must). @p user is the context's own
 * pointer; @p local is what open returned. Not a pointer type by itself; use
 * ::lh_rail_context_close_cb.
 */
typedef lh_void(lh_rail_context_close_fn)(lh_ptr user, lh_ptr local);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_CONTEXT_CLOSE_FN_H */
