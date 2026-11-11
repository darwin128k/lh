/**
 * @file fn.h
 * @brief ::lh_os_module_hook_fn — one handler of a hook chain.
 */

#ifndef LH_OS_MODULE_HOOK_FN_H
#define LH_OS_MODULE_HOOK_FN_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the call, change it in place if need be, and say whether the chain
 *        goes on.
 *
 * @param context The value given when the handler was added.
 * @param call    The call the chain runs for; its shape is the caller's.
 * @return ::lh_bool_true to go on with the next handler, ::lh_bool_false to stop.
 */
typedef lh_bool_t(lh_os_module_hook_fn)(lh_ptr context, lh_ptr call);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_HOOK_FN_H */
