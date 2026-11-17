/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_now_us_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_NOW_US_FN_H
#define LH_OS_WINDOW_BACKEND_NOW_US_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_now_us_fn
 * @brief A monotonic clock in microseconds.
 *
 * Optional, and read only by a measurement (::lh_gl_frame_meter_t): without it
 * a frame is recorded with its time `0`.
 *
 * @param context The backend's context.
 * @return Microseconds from an arbitrary origin.
 */
typedef lh_u64_t(lh_os_window_backend_now_us_fn)(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_NOW_US_FN_H */
