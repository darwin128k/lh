/**
 * @file fn.h
 * @brief What an executor thread opens when it starts.
 */

#ifndef LH_RAIL_CONTEXT_OPEN_FN_H
#define LH_RAIL_CONTEXT_OPEN_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_rail_context_open_fn
 * @brief Function type `lh_ptr (lh_ptr user)` — open the thread's state.
 *
 * Runs once on the executor's thread, before its first task. @p user is the
 * context's own pointer. The result is the `local` every `work` on that thread
 * receives, and what `close` gets back: a database connection, a scratch
 * buffer. ::lh_null is a valid result (nothing opened, or opening failed — the
 * work then sees ::lh_null and decides). Not a pointer type by itself; use
 * ::lh_rail_context_open_cb.
 */
typedef lh_ptr(lh_rail_context_open_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_CONTEXT_OPEN_FN_H */
