/**
 * @file fn.h
 * @brief ::lh_exec_loop_release_fn — what an abandoned loop's thread frees when it ends.
 */

#ifndef LH_EXEC_LOOP_RELEASE_FN_H
#define LH_EXEC_LOOP_RELEASE_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Free the state an abandoned loop's step worked on.
 *
 * Runs once, on the loop's own thread, after its last step, and only when the owner
 * gave up waiting (::lh_exec_loop_try_abandon): the owner no longer touches that state,
 * so the thread is the last to use it and frees it. The loop record itself is not
 * freed here; the owner reaps it later (::lh_exec_loop_try_reap).
 *
 * @param user The value given to ::lh_exec_loop_try_abandon.
 */
typedef lh_void(lh_exec_loop_release_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_LOOP_RELEASE_FN_H */
