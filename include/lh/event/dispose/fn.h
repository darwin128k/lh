/**
 * @file fn.h
 * @brief Subscription context disposal function type.
 *
 * Subscription context disposal function type.
 */

#ifndef LH_EVENT_DISPOSE_FN_H
#define LH_EVENT_DISPOSE_FN_H

#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Release context after subscription removal and the outermost publish.
 * @param context Transferred on successful subscription, including null.
 * The callback must not reenter or deinitialize the bus.
 */
typedef lh_void(lh_event_dispose_fn)(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EVENT_DISPOSE_FN_H */
