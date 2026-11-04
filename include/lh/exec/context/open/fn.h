/**
 * @file fn.h
 * @brief What an executor thread opens when it starts.
 */

#ifndef LH_EXEC_CONTEXT_OPEN_FN_H
#define LH_EXEC_CONTEXT_OPEN_FN_H

#include <lh/core/ptr.h>

/**
 * @typedef lh_exec_context_open_fn
 * @brief Function type `lh_ptr (lh_ptr user)` — open the thread's state.
 *
 * Runs once on the executor's thread, before its first task. @p user is the
 * context's own pointer. The result is the `local` every `work` on that thread
 * receives, and what `close` gets back: a database connection, a scratch
 * buffer. ::lh_null is a valid result (nothing opened, or opening failed — the
 * work then sees ::lh_null and decides). Not a pointer type by itself; use
 * ::lh_exec_context_open_cb.
 */
typedef lh_ptr(lh_exec_context_open_fn)(lh_ptr user);

#endif /* LH_EXEC_CONTEXT_OPEN_FN_H */
