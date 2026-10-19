/**
 * @file corner.h
 * @brief Round-rect corner cut shared by window backends.
 *
 * The inset of a corner row is pure geometry: the same half-pixel circle test
 * Windows' `CreateRoundRectRgn` uses, so POSIX SHAPE and Win GDI stay visually
 * matched. No display types here.
 */

#ifndef LH_OS_WINDOW_CORNER_H
#define LH_OS_WINDOW_CORNER_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

/**
 * @def LH_OS_WINDOW_CORNER_MAX
 * @brief Largest corner radius a backend cuts (stack rows stay bounded).
 */
#define LH_OS_WINDOW_CORNER_MAX 128

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How far in from the side row @p row of a corner of @p radius starts.
 *
 * The first pixel whose centre is inside the circle, in whole pixels. Measured
 * in half-pixels so the test stays integer — near enough to `CreateRoundRectRgn`
 * that the two cannot be told apart at UI radii.
 */
lh_s32_t
lh_os_window_corner_inset(lh_s32_t radius, lh_s32_t row);

/**
 * @brief Effective cut radius for a window of @p width x @p height.
 *
 * Zero while @p maximized; never more than half the window or
 * ::LH_OS_WINDOW_CORNER_MAX.
 */
lh_s32_t
lh_os_window_corner_clamp(lh_s32_t corner, lh_s32_t width, lh_s32_t height, lh_bool_t maximized);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_CORNER_H */
