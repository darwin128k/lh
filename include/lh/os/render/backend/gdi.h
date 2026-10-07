/**
 * @file gdi.h
 * @brief Win32 GDI as a screen: ::lh_os_render_backend_gdi.
 *
 * Every primitive is drawn by the software backend (::lh_ui_canvas_backend_sw)
 * into the pixels of an owned ::lh_ui_surface_t (a 32-bit DIB section). GDI
 * only shows the result: `begin` sizes the surface to the client and points
 * the software context at its pixels, `end` blits the surface into the paint
 * DC in one `BitBlt`. Set the DC from ::lh_os_window_get_paint_dc inside
 * `on_paint`, then paint; clear it when the paint ends. Win32 calls go
 * through ::lh_os_system_*.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW and Windows.
 */

#ifndef LH_OS_RENDER_BACKEND_GDI_H
#define LH_OS_RENDER_BACKEND_GDI_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/os.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/system/window/handle.h>
#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
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
 * @brief GDI target: window, paint DC, off-screen surface, software context.
 */
struct lh_os_render_backend_gdi_context
{
    lh_os_render_backend_gdi_fields(lh_os_system_window_handle_t, lh_ptr, lh_ui_surface_t, lh_ui_canvas_sw_t,
                                    lh_u32_t, lh_u64_t);
};
typedef struct lh_os_render_backend_gdi_context lh_os_render_backend_gdi_context_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Context ─────────────────────────────────────────────────────────────── */

/**
 * @brief Clear @p self: no window, no DC, empty surface, no pixels to draw.
 *        Pair with ::lh_os_render_backend_gdi_context_deinit.
 */
lh_void
lh_os_render_backend_gdi_context_init(lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Drop the surface of @p self and clear it.
 */
lh_void
lh_os_render_backend_gdi_context_deinit(lh_os_render_backend_gdi_context_t *self);

/**
 * @brief @p context of a backend call as the GDI context it is; asserts it is
 *        not ::lh_null.
 */
lh_os_render_backend_gdi_context_t *
lh_os_render_backend_gdi_context_from(lh_ptr context);

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

/* ── Counters ────────────────────────────────────────────────────────────── */

/**
 * @brief Zero the per-primitive call counts of @p self and start the frame clock.
 */
lh_void
lh_os_render_backend_gdi_reset_counters(lh_os_render_backend_gdi_context_t *self);

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

/* ── Backend slots (context: ::lh_os_render_backend_gdi_context_t) ───────── */

/**
 * @brief Size the surface of @p self to the client and reset the counters.
 *        False without a client size.
 */
lh_bool_t
lh_os_render_backend_gdi_begin_surface(lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Backend `begin`: ::lh_os_render_backend_gdi_begin_surface, then point
 *        the software context at the surface pixels (an empty pixmap, so
 *        nothing is drawn, when there is no surface).
 */
lh_void
lh_os_render_backend_gdi_begin(lh_ptr context);

/**
 * @brief Backend `end`: blit the surface into the paint DC.
 */
lh_void
lh_os_render_backend_gdi_end(lh_ptr context);

/**
 * @brief Backend `clear`: ::lh_ui_canvas_sw_clear on the surface pixels.
 */
lh_void
lh_os_render_backend_gdi_clear(lh_ptr context, const lh_ui_color_t *color);

/**
 * @brief Backend `fill_rect`: counted, then ::lh_ui_canvas_sw_fill_rect.
 */
lh_void
lh_os_render_backend_gdi_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color);

/**
 * @brief Backend `fill_round_rect`: counted, then ::lh_ui_canvas_sw_fill_round_rect.
 */
lh_bool_t
lh_os_render_backend_gdi_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                         const lh_ui_color_t *color);

/**
 * @brief Backend `set_clip`: counted, then ::lh_ui_canvas_sw_set_clip.
 */
lh_void
lh_os_render_backend_gdi_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip);

/**
 * @brief Backend `fill_mask`: counted, then ::lh_ui_canvas_sw_fill_mask.
 */
lh_bool_t
lh_os_render_backend_gdi_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                                   const lh_ui_color_t *color);

/**
 * @brief The GDI backend table: software drawing into the surface, GDI blit.
 */
extern const lh_ui_canvas_backend_t lh_os_render_backend_gdi;

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_RENDER_BACKEND_GDI_H */
