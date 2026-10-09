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
                                    lh_u32_t, lh_u64_t, lh_ui_point_t);
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

/**
 * @brief Pixel format of the off-screen surface of @p self.
 *
 * ARGB8888 by default, and that is the measured answer for a window even though
 * RGB565 draws faster. The frame a user sees is the draw plus the present, and the
 * two do not agree: this machine draws the same 800x600 scene in 137 750 ns as
 * ARGB8888 and 94 100 ns as RGB565, and then pays for getting it on the screen
 * 382 400 ns against 839 050 ns. A 32-bit window DC cannot be copied into from 16
 * bits — GDI expands every pixel — and the present is the larger half of the
 * frame, so the 44 us the draw saves comes back as 457 us.
 *
 * The present is per pixel and nothing else: the same present of one 256x160
 * damage strip costs 32 550 ns against 83 850 ns, so its price scales exactly with
 * the pixels and there is nothing to win from presenting less often.
 *
 * RGB565 is still right where nothing widens the buffer on the way out — an MCU
 * with a DMA2D blits its own 16-bit buffer into a 16-bit framebuffer, and there
 * half the bytes is half the time. On a desktop it is a trap, and it is one call
 * away either way.
 */
lh_ui_pixmap_format_t
lh_os_render_backend_gdi_context_get_format(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Make @p format the format of the off-screen surface of @p self.
 *
 * Before the first frame: the surface refuses a format change once it holds a
 * buffer, and changing it afterwards would be a window drawn in two formats.
 */
lh_void
lh_os_render_backend_gdi_context_set_format(lh_os_render_backend_gdi_context_t *self,
                                            lh_ui_pixmap_format_t format);

/* ── Counters ──────────────────────────────────────────────────────────────
 * They cover what the last `begin` or `begin_area` opened, and nothing more:
 * a whole-target frame is one of those, a partial frame is one per strip, so
 * under PARTIAL these are the last strip's numbers. */

/**
 * @brief Start the clock and zero the counters of @p self.
 */
lh_void
lh_os_render_backend_gdi_reset_counters(lh_os_render_backend_gdi_context_t *self);

/**
 * @brief `fill_mask` calls since the last `begin` or `begin_area`.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_mask_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief `fill_rect` calls since the last `begin` or `begin_area`.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_rect_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief `fill_round_rect` calls since the last `begin` or `begin_area`.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_round_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief `set_clip` calls since the last `begin` or `begin_area`.
 */
lh_u32_t
lh_os_render_backend_gdi_context_get_clip_calls(const lh_os_render_backend_gdi_context_t *self);

/**
 * @brief Microseconds from the last `begin` or `begin_area` — one frame, or
 *        one strip of a partial frame.
 */
lh_u64_t
lh_os_render_backend_gdi_context_get_frame_us(const lh_os_render_backend_gdi_context_t *self);

/* ── Backend slots (context: ::lh_os_render_backend_gdi_context_t) ───────── */

/**
 * @brief Backend `begin`: the client rectangle as the frame area, then point
 *        the software context at the surface pixels (an empty pixmap, so
 *        nothing is drawn, when there is no surface).
 */
lh_void
lh_os_render_backend_gdi_begin(lh_ptr context);

/**
 * @brief Backend `begin_area`: size the surface to @p area alone — that is the
 *        whole memory win of a partial frame — point the software context at
 *        it, and remember the corner `end` presents it back at.
 */
lh_void
lh_os_render_backend_gdi_begin_area(lh_ptr context, const lh_ui_rect_t *area);

/**
 * @brief Backend `end`: blit into the paint DC, at `present_at` — the whole
 *        surface for a frame that was never clipped, and @p drawn for one that
 *        was, which is all this backend wrote.
 *
 * @p drawn arrives from the canvas, not from here: see
 * ::lh_ui_canvas_end_fn.
 */
lh_void
lh_os_render_backend_gdi_end(lh_ptr context, const lh_ui_rects_t *drawn);

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
 * @brief Backend `set_clip`: counted, ::lh_ui_canvas_sw_set_clip, and the
 *        clip added to `drawn` — what `end` presents.
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
 * @brief Backend `shadow`: ::lh_ui_canvas_sw_shadow on the surface.
 */
lh_bool_t
lh_os_render_backend_gdi_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                               const lh_ui_shadow_t *shadow);

/**
 * @brief Backend `blur`: ::lh_ui_canvas_sw_blur on the surface.
 */
lh_bool_t
lh_os_render_backend_gdi_blur(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius,
                             lh_u8_t *scratch, lh_usize_t bytes);

/**
 * @brief Backend `glass`: ::lh_ui_canvas_sw_glass on the surface.
 */
lh_bool_t
lh_os_render_backend_gdi_glass(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                              lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint, lh_u8_t *scratch,
                              lh_usize_t bytes);

/**
 * @brief The GDI backend table: software drawing into the surface, GDI blit.
 */
extern const lh_ui_canvas_backend_t lh_os_render_backend_gdi;

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_RENDER_BACKEND_GDI_H */
