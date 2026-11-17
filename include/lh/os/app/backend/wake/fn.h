/**
 * @file fn.h
 * @brief Function type ::lh_os_app_backend_wake_fn.
 */

#ifndef LH_OS_APP_BACKEND_WAKE_FN_H
#define LH_OS_APP_BACKEND_WAKE_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_app_backend_wake_fn
 * @brief Wake a `wait` that is in progress.
 *
 * Called by ::lh_os_app_quit, so an application waiting with nothing to do
 * sees the quit at once.
 *
 * @param context The backend's context.
 */
typedef lh_void(lh_os_app_backend_wake_fn)(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_BACKEND_WAKE_FN_H */
