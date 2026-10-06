/**
 * @file plus.h
 * @brief GDI+ part of the GDI canvas backend.
 *
 * Round fills and mask fills. Native calls live in ::lh_os_system_gdiplus_*;
 * this header wraps ::lh_ui_color_t / ::lh_ui_mask_t. Mask drawing needs a
 * frame opened for the canvas begin/end (not one Graphics per glyph).
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
 */
lh_void
lh_os_render_backend_gdi_plus_fill_mask(lh_os_system_gdiplus_frame_t frame, const lh_ui_point_t *origin,
                                        const lh_ui_mask_t *mask, const lh_ui_color_t *color);

/**
 * @brief Anti-aliased rounded fill of @p rect on @p hdc; nothing on failure.
 *
 * @p hdc is the opaque draw target from ::lh_ui_surface_get_draw_target.
 */
lh_void
lh_os_render_backend_gdi_plus_fill_round_rect(lh_ptr hdc, const lh_ui_rect_t *rect,
                                              lh_ui_scalar_t radius, const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_RENDER_BACKEND_GDI_PLUS_H */
