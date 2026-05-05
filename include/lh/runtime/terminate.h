/**
 * @file terminate.h
 * @brief Runtime terminate handler: setter and invocation.
 *
 * Provides ::lh_runtime_terminate_set() to install a custom terminate
 * callback and ::lh_runtime_terminate() to invoke the current handler
 * (typically in unrecoverable error paths).
 *
 * @see lh_runtime_terminate_fn
 * @see lh_runtime_terminate_cb
 */

#ifndef LH_RUNTIME_TERMINATE_H
#define LH_RUNTIME_TERMINATE_H

#include <lh/attribute/noreturn.h>
#include <lh/attribute/thread_local.h>
#include <lh/library/fallback.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/unreachable.h>
#include <lh/runtime/terminate/cb.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Install a custom runtime terminate callback.
 * @param fn New callback (or null to restore default). Type: ::lh_runtime_terminate_cb.
 * @return Previous callback (may be null).
 */
lh_runtime_terminate_cb lh_runtime_terminate_set(lh_runtime_terminate_cb fn);

/**
 * @brief Invoke the current runtime terminate handler (does not return).
 *
 * Calls the callback set via ::lh_runtime_terminate_set(), or the default
 * handler if none was set.
 * The function is declared noreturn.
 */
LH_ATTRIBUTE_NORETURN
void lh_runtime_terminate(void);

LH_COMPILER_EXTERN_C_END

#endif // LH_RUNTIME_TERMINATE_H