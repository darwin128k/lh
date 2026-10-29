/**
 * @file fn.h
 * @brief Subscription context disposal function type.
 *
 * Called once per subscription, after removal and after the outermost publication
 * has returned.
 */

#ifndef LH_OS_MODULE_EVENT_DISPOSE_FN_H
#define LH_OS_MODULE_EVENT_DISPOSE_FN_H

#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Release context after subscription removal and the outermost publish.
 * @param context Transferred on successful subscription, including null.
 * The callback must not reenter or deinitialize the bus.
 */
typedef lh_void(lh_os_module_event_dispose_fn)(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_EVENT_DISPOSE_FN_H */
