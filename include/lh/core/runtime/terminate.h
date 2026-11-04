/**
 * @file terminate.h
 * @brief Runtime terminate slot: the handler unrecoverable paths stop with.
 *
 * ::lh_runtime_terminate runs the runtime ::lh_runtime_terminate_handler_t,
 * typically on an unrecoverable error path
 * (a failed check, a broken invariant).
 *
 * The slot is one object for the whole program, empty until the host fills it
 * through ::lh_runtime_terminate_get_mut — before the first use and before it starts
 * other threads (a thread sees everything written before it was created).
 * Every thread, the ones LH starts and the host's own, uses the same object.
 *
 * The handler starts empty: the core has no C library. An empty
 * handler, or one that returns, ends in a compiler trap (`lh_compiler_trap`),
 * which stops a debugger right there; a desktop host installs a handler that
 * calls `abort`.
 *
 * @code{.c}
 * static void board_reset(lh_self_ptr self) { (void)self; NVIC_SystemReset(); }
 * lh_runtime_terminate_handler_init(lh_runtime_terminate_get_mut(), board_reset, lh_null); // at
 * start-up
 * @endcode
 */

#ifndef LH_RUNTIME_TERMINATE_H
#define LH_RUNTIME_TERMINATE_H

#include <lh/core/library/fallback.h>
#include <lh/core/attribute/noreturn.h>
#include <lh/core/bool.h>
#include <lh/core/runtime/terminate/handler.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the runtime terminate handler, for change.
 *
 * One object for every thread. The host fills it before the first use and
 * before it starts other threads; a change while other threads use it is a
 * data race.
 *
 * @return The runtime ::lh_runtime_terminate_handler_t (never null).
 */
lh_runtime_terminate_handler_t *
lh_runtime_terminate_get_mut(void);

/**
 * @brief Return the runtime terminate handler.
 *
 * Same object as ::lh_runtime_terminate_get_mut, read-only.
 *
 * @return The runtime ::lh_runtime_terminate_handler_t (never null).
 */
const lh_runtime_terminate_handler_t *
lh_runtime_terminate_get(void);

/**
 * @brief Invoke the calling thread's terminate handler (does not return).
 *
 * Runs ::lh_runtime_terminate_get(); an empty handler, or one that returns,
 * ends in a compiler trap.
 *
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
