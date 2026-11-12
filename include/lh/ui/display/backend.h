/**
 * @file backend.h
 * @brief ::lh_ui_display_backend — a canvas backend that draws a display in strips
 *        and presents each strip as it is done.
 *
 * A display (lh/ui/display.h) owns a pixel buffer that may be only a strip of the
 * panel — 48 rows of 240 on a microcontroller — and the sinks that put pixels on
 * the panel. Its own canvas uses the plain software backend, which has no
 * `begin_area`: a view split into strips (::lh_ui_view_set_strip_height) then falls
 * back to one whole frame, and the caller has to move the strip and present by
 * hand.
 *
 * This backend closes that gap. Its context is the display itself:
 * - `begin` (a whole frame) puts the strip at row 0;
 * - `begin_area` puts the strip at the area's top row
 *   (::lh_ui_display_set_strip_y), so the canvas draws the area into the buffer;
 * - `end` presents what the frame drew (::lh_ui_display_present), so every sink
 *   gets the strip while its rows are still in the buffer;
 * - every drawing slot is the software backend's, on the display's own
 *   ::lh_ui_canvas_sw_t.
 *
 * A sink learns where the strip sits from ::lh_ui_display_get_strip_y: give it the
 * display as its context.
 *
 * ::lh_ui_display_backend_bind puts a display's canvas on this backend. A display
 * that is not bound keeps the plain software backend, as before.
 */

#ifndef LH_UI_DISPLAY_BACKEND_H
#define LH_UI_DISPLAY_BACKEND_H

#include <lh/ui/display.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Backend `begin`: a whole frame, so the strip starts at row 0.
 *
 * @param context The display (::lh_ui_display_t); not ::lh_null.
 */
lh_void
lh_ui_display_backend_begin(lh_ptr context);

/**
 * @brief Backend `begin_area`: the strip moves to the top row of @p area.
 *
 * The canvas has already set its origin to minus the area's corner, so what is
 * drawn lands in the buffer; the buffer must be at least as tall as the area
 * (the view sizes its areas to the strip height).
 *
 * @param context The display; not ::lh_null.
 * @param area    The area of the panel this frame draws; not ::lh_null.
 */
lh_void
lh_ui_display_backend_begin_area(lh_ptr context, const lh_ui_rect_t *area);

/**
 * @brief Backend `end`: present what the frame drew to the display's sinks.
 *
 * @param context The display; not ::lh_null.
 * @param drawn   The rects the frame drew, in buffer space; ::lh_null or empty
 *                presents the whole buffer.
 */
lh_void
lh_ui_display_backend_end(lh_ptr context, const lh_ui_rects_t *drawn);

/** @brief Backend `clear`: ::lh_ui_canvas_sw_clear on the display's software canvas. */
lh_void
lh_ui_display_backend_clear(lh_ptr context, const lh_ui_color_t *color);

/** @brief Backend `fill_rect`: ::lh_ui_canvas_sw_fill_rect on the display's software canvas. */
lh_void
lh_ui_display_backend_fill_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                const lh_ui_color_t *color);

/**
 * @brief Backend `fill_round_rect`: ::lh_ui_canvas_sw_fill_round_rect on the display's
 *        software canvas.
 * @return What that call returns.
 */
lh_bool_t
lh_ui_display_backend_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                      lh_ui_scalar_t radius, const lh_ui_color_t *color);

/** @brief Backend `set_clip`: ::lh_ui_canvas_sw_use_clip on the display's software canvas. */
lh_void
lh_ui_display_backend_use_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip);

/**
 * @brief Backend `fill_mask`: ::lh_ui_canvas_sw_fill_mask on the display's software canvas.
 * @return What that call returns.
 */
lh_bool_t
lh_ui_display_backend_fill_mask(lh_ptr context, const lh_ui_point_t *origin,
                                const lh_ui_mask_t *mask, const lh_ui_color_t *color);

/**
 * @brief Backend `shadow`: ::lh_ui_canvas_sw_shadow on the display's software canvas.
 * @return What that call returns.
 */
lh_bool_t
lh_ui_display_backend_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                             const lh_ui_shadow_t *shadow);

/**
 * @brief Backend `blur`: ::lh_ui_canvas_sw_blur on the display's software canvas.
 * @return What that call returns.
 */
lh_bool_t
lh_ui_display_backend_blur(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius,
                           lh_u8_t *scratch, lh_usize_t bytes);

/**
 * @brief Backend `glass`: ::lh_ui_canvas_sw_glass on the display's software canvas.
 * @return What that call returns.
 */
lh_bool_t
lh_ui_display_backend_glass(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                            lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint,
                            lh_u8_t *scratch, lh_usize_t bytes);

/**
 * @brief Backend `fill_colors`: ::lh_ui_canvas_sw_fill_colors on the display's software
 *        canvas.
 * @return What that call returns.
 */
lh_bool_t
lh_ui_display_backend_fill_colors(lh_ptr context, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                                  const lh_ui_color_t *colors, lh_ui_axis_t axis);

/**
 * @brief The table: the strip and present slots above, the software drawing slots.
 */
extern const lh_ui_canvas_backend_t lh_ui_display_backend;

/**
 * @brief Put the canvas of @p display on ::lh_ui_display_backend, with the display as
 *        its context. The canvas keeps its size (the panel).
 *
 * Call it once the display has its panel, buffer and sinks. Draw with
 * ::lh_ui_display_get_canvas_mut: a view on that canvas with a strip height draws
 * and presents strip by strip; without one it draws one whole frame and presents
 * it at its end.
 *
 * @param display The display; not ::lh_null.
 */
lh_void
lh_ui_display_backend_bind(lh_ui_display_t *display);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_DISPLAY_BACKEND_H */
