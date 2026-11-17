/**
 * @file fn.h
 * @brief Callable signature for the step of an application.
 */

#ifndef LH_OS_APP_STEP_FN_H
#define LH_OS_APP_STEP_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_app_step_fn
 * @brief Function type `lh_void(lh_ptr)` for the step of an application.
 *
 * Do what there is to do now, then return: take the events, draw what
 * changed, and let the caller sleep until it is time again.
 *
 * The shape is the one of a rail loop's step (::lh_rail_loop_step_fn), so the
 * same call can serve as the step of a loop and as the step of an
 * application.
 *
 * @param user The value the application was given.
 */
typedef lh_void(lh_os_app_step_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_STEP_FN_H */
