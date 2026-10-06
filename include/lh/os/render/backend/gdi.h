/**
 * @file gdi.h
 * @brief Win32 GDI canvas backend: ::lh_os_render_backend_gdi.
 *
 * Context holds a window handle, the paint-time destination DC (::lh_ptr),
 * and an owned ::lh_ui_surface_t. Set the DC from ::lh_os_window_get_paint_dc
 * inside `on_paint`, then paint; clear it when the paint ends. `begin` sizes
 * the surface to the client and draws into it; `end` presents the surface to
 * the paint DC in one blit. Win32 calls go through ::lh_os_system_*.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW and Windows.
 */

#ifndef LH_OS_RENDER_BACKEND_GDI_H
#define LH_OS_RENDER_BACKEND_GDI_H

#include <lh/compiler/extern/c.h>
#include <lh/compiler/os.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/system/window/handle.h>
#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/surface.h>
#include <lh/void.h>

#include <lh/os/render/backend/gdi/fields.h>

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/render/backend/gdi.h requires LH_LIBRARY_OPTION_OS_WINDOW"
#endif

#if LH_COMPILER_OS != LH_COMPILER_OS_WINDOWS
#    error "lh/os/render/backend/gdi.h requires Windows"
#endif

/**
 * @struct lh_os_render_backend_gdi_context
 * @typedef lh_os_render_backend_gdi_context_t
 * @brief GDI target: window, paint DC, off-screen surface.
 */
struct lh_os_render_backend_gdi_context
{
    lh_os_render_backend_gdi_fields(lh_os_system_window_handle_t, lh_ptr, lh_ui_surface_t,
                                    lh_os_system_gdiplus_frame_t, lh_ptr, lh_u32_t, lh_u64_t);
};
typedef struct lh_os_render_backend_gdi_context lh_os_render_backend_gdi_context_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Clear @p self: no window, no DC, empty surface. Starts GDI+ for the
 *        first context; pair with ::lh_os_render_backend_gdi_context_deinit.
 */
lh_void
lh_os_render_backend_gdi_context_init(lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Drop the surface of @p self and release GDI+ when it was the last
 *        GDI context.
 */
lh_void
lh_os_render_backend_gdi_context_deinit(lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Bind @p self to @p hwnd. Does not own the window.
 */
lh_void
lh_os_render_backend_gdi_context_set_hwnd(lh_os_render_backend_gdi_context_t *self,
                                          lh_os_system_window_handle_t hwnd);

/**
 * @brief Window of @p self, or ::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID.
 */
lh_os_system_window_handle_t
lh_os_render_backend_gdi_context_get_hwnd(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Set the paint-time destination DC (from ::lh_os_window_get_paint_dc),
 *        or ::lh_null.
 */
lh_void
lh_os_render_backend_gdi_context_set_hdc(lh_os_render_backend_gdi_context_t *self, lh_ptr hdc);

/**
 * @brief Current paint destination DC, or ::lh_null outside a paint cycle.
 */
lh_ptr
lh_os_render_backend_gdi_context_get_hdc(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Backend `begin`: size the surface to the client, draw into it.
 */
lh_void
lh_os_render_backend_gdi_begin(lh_ptr context);

/**
 * @brief Backend `end`: present the surface into the paint DC.
 */
lh_void
lh_os_render_backend_gdi_end(lh_ptr context);

/**
 * @brief Backend `clear`: fill the surface.
 */
lh_void
lh_os_render_backend_gdi_clear(lh_ptr context, const lh_ui_color_t *color);

/**
 * @brief Backend `fill_rect`: solid fill on the surface.
 */
lh_void
lh_os_render_backend_gdi_fill_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                   const lh_ui_color_t *color);

/**
 * @brief Backend `fill_round_rect`: anti-aliased through GDI+ on the surface;
 *        a square fill when GDI+ did not start.
 */
lh_void
lh_os_render_backend_gdi_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                         lh_ui_scalar_t radius, const lh_ui_color_t *color);

/**
 * @brief Backend `set_clip`: the surface DC clip region (::lh_null removes it).
 */
lh_void
lh_os_render_backend_gdi_set_clip(lh_ptr context, const lh_ui_rect_t *clip);

/**
 * @brief Backend `fill_mask`: paint @p mask in @p color through the frame GDI+.
 */
lh_void
lh_os_render_backend_gdi_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                                   const lh_ui_color_t *color);

/**
 * @brief `fill_mask` calls since the last ::lh_os_render_backend_gdi_begin.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_mask_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief `fill_rect` calls since the last ::lh_os_render_backend_gdi_begin.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_rect_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief `fill_round_rect` calls since the last ::lh_os_render_backend_gdi_begin.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_round_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief `set_clip` calls since the last ::lh_os_render_backend_gdi_begin.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_clip_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Microseconds since ::lh_os_render_backend_gdi_begin for the current frame.
 */
lh_u64_t
lh_os_render_backend_gdi_context_get_frame_us(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief GDI backend table for ::lh_ui_canvas_t (context: this GDI context).
 *
 * Rounded fills go through GDI+ (::lh_os_render_backend_gdi_fill_round_rect).
 */
extern const lh_ui_canvas_backend_t lh_os_render_backend_gdi;

/**
 * @brief Same GDI table as ::lh_os_render_backend_gdi, but `fill_round_rect`
 *        is ::lh_null — the canvas falls back to ::lh_ui_canvas_fill_round_rect_by_rects.
 */
extern const lh_ui_canvas_backend_t lh_os_render_backend_gdi_soft;

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_RENDER_BACKEND_GDI_H */
