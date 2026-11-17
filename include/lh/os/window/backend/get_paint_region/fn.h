/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_get_paint_region_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_GET_PAINT_REGION_FN_H
#define LH_OS_WINDOW_BACKEND_GET_PAINT_REGION_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/bool.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_get_paint_region_fn
 * @brief Read what the backend was asked to paint, off the paint @p target.
 *
 * Optional, and read only by a measurement (::lh_gl_frame_meter_t): without it
 * a frame is recorded with its request unknown.
 *
 * @param context The backend's context.
 * @param target  The paint target ::lh_os_window_on_native_paint was given.
 * @param area    Receives the area of every invalidated rectangle together.
 * @param count   Receives how many rectangles there were.
 * @return True when the region was read.
 */
typedef lh_bool_t(lh_os_window_backend_get_paint_region_fn)(lh_ptr context, lh_ptr target,
                                                            lh_u64_t *area, lh_u32_t *count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_GET_PAINT_REGION_FN_H */
