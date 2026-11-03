/**
 * @file terminate.h
 * @brief Runtime terminate handler: setter and invocation.
 *
 * Provides ::lh_runtime_terminate_set() to install a custom terminate
 * callback and ::lh_runtime_terminate() to invoke the current handler
 * (typically in unrecoverable error paths).
 *
 * The handler has two levels, as ::lh_runtime_allocator does. The process
 * handler (::lh_runtime_terminate_get_process) is one for the whole program;
 * every thread's own handler starts as a copy of it, taken on that thread's
 * first use. A host installs its handler once, at start-up, with
 * ::lh_runtime_terminate_set_process, and every thread uses it: the threads LH
 * starts and the host's own alike. The process handler is published
 * atomically (::lh_atomic_fn_ptr_t), so installing it while other threads run
 * is no data race. With
 * ::LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB on the process handler
 * starts as abort(); off, null, and a null handler spins instead of returning.
 *
 * @see lh_runtime_terminate_fn
 * @see lh_runtime_terminate_cb
 */

#ifndef LH_RUNTIME_TERMINATE_H
#define LH_RUNTIME_TERMINATE_H

#include <lh/library/fallback.h>
#include <lh/compiler/extern/c.h>
#include <lh/attribute/noreturn.h>
#include <lh/bool.h>
#include <lh/runtime/terminate/cb.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Get the process terminate callback.
 *
 * The value each thread's handler is copied from on that thread's first use;
 * it is not what an already running thread calls. Read with acquire order.
 *
 * @return The process callback, or null if none is installed.
 */
lh_runtime_terminate_cb
lh_runtime_terminate_get_process(void);

/**
 * @brief Install the process terminate callback.
 *
 * Published with release order. Reaches only threads that have not used their
 * handler yet, so call it at start-up, before other threads use LH, for every
 * thread to get it. Safe while other threads run: each sees the old handler or
 * the new one.
 *
 * @param fn New process callback (null: threads seeded from it spin on
 *           ::lh_runtime_terminate).
 */
lh_void
lh_runtime_terminate_set_process(lh_runtime_terminate_cb fn);

/**
 * @brief Install the calling thread's terminate callback.
 *
 * Changes this thread only; other threads keep theirs.
 *
 * @param fn New callback (null: ::lh_runtime_terminate spins on this thread).
 *           Type: ::lh_runtime_terminate_cb.
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_terminate_set(my_handler);
 * @endcode
 */
lh_void
lh_runtime_terminate_set(lh_runtime_terminate_cb fn);

/**
 * @brief Get the calling thread's terminate callback.
 *
 * On the thread's first use it is a copy of ::lh_runtime_terminate_get_process.
 *
 * @return The thread's callback, or null if none is installed
 *         (::lh_runtime_terminate then spins).
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_terminate_cb old = lh_runtime_terminate_get();
 * // install new handler
 * @endcode
 */
lh_runtime_terminate_cb
lh_runtime_terminate_get(void);

/**
 * @brief Invoke the calling thread's terminate handler (does not return).
 *
 * Calls ::lh_runtime_terminate_get(); a null handler, or one that returns,
 * leaves the thread spinning so the host can still inspect it.
 * The function is declared noreturn.
 *
 * Example usage:
 * @code{.c}
 * if (unrecoverable_error) {
 *     lh_runtime_terminate();
 * }
 * @endcode
 */
LH_ATTRIBUTE_NORETURN
lh_void
lh_runtime_terminate(void);

/**
 * @brief Invoke the terminate handler unless @p ok holds.
 *
 * For an invariant whose breach leaves no safe way on: an unlock or a join that
 * failed, after which carrying on could free state another thread still uses.
 *
 * @param ok The invariant; false does not return.
 */
lh_void
lh_runtime_terminate_unless(lh_bool_t ok);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_TERMINATE_H */
