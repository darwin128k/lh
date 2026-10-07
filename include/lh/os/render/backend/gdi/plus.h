/**
 * @file plus.h
 * @brief GDI+ part of the GDI canvas backend.
 *
 * Round fills and mask fills. Native calls live in ::lh_os_system_gdiplus_*;
 * this header wraps ::lh_ui_color_t / ::lh_ui_mask_t. Both need a frame opened
 * for the canvas begin/end (not one Graphics / path / solid per call).
 */

#ifndef LH_OS_RENDER_BACKEND_GDI_PLUS_H
#define LH_OS_RENDER_BACKEND_GDI_PLUS_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/os/system/gdiplus.h>
#include <lh/ptr.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start GDI+ for one more user (only the first call starts it).
 *
 * @return ::lh_bool_true when GDI+ is running afterwards.
 */
lh_bool_t
lh_os_render_backend_gdi_plus_acquire(lh_void);

/**
 * @brief Drop one user; the last one stops GDI+.
 */
lh_void
lh_os_render_backend_gdi_plus_release(lh_void);

/**
 * @brief True while GDI+ is running.
 */
lh_bool_t
lh_os_render_backend_gdi_plus_is_ready(lh_void);

/**
 * @brief Open a GDI+ frame on @p hdc for the canvas begin/end.
 */
lh_os_system_gdiplus_frame_t
lh_os_render_backend_gdi_plus_frame_begin(lh_ptr hdc);

/**
 * @brief Close @p frame from ::lh_os_render_backend_gdi_plus_frame_begin.
 */
lh_void
lh_os_render_backend_gdi_plus_frame_end(lh_os_system_gdiplus_frame_t frame);

/**
 * @brief Paint @p mask in @p color at @p origin through @p frame.
 *        ::lh_bool_false when nothing was drawn.
 */
lh_bool_t
lh_os_render_backend_gdi_plus_fill_mask(lh_os_system_gdiplus_frame_t frame, const lh_ui_point_t *origin,
                                        const lh_ui_mask_t *mask, const lh_ui_color_t *color);

/**
 * @brief Anti-aliased rounded fill of @p rect through @p frame.
 *        ::lh_bool_false when nothing was drawn.
 */
lh_bool_t
lh_os_render_backend_gdi_plus_fill_round_rect(lh_os_system_gdiplus_frame_t frame,
                                              const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                              const lh_ui_color_t *color);

/**
 * @brief Match the frame Graphics clip to @p left/top/right/bottom.
 */
lh_void
lh_os_render_backend_gdi_plus_set_clip(lh_os_system_gdiplus_frame_t frame, int left, int top,
                                       int right, int bottom);

/**
 * @brief Clear the frame Graphics clip.
 */
lh_void
lh_os_render_backend_gdi_plus_clear_clip(lh_os_system_gdiplus_frame_t frame);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_RENDER_BACKEND_GDI_PLUS_H */
