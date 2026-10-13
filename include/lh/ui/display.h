/**
 * @file display.h
 * @brief Soft draw + host buffer + present sinks: ::lh_ui_display_t.
 *
 * This is the package F4VE already proved by hand:
 *
 * 1. Application owns the pixel bytes (full panel or a partial strip).
 * 2. Drawing goes through ::lh_ui_canvas_backend_sw into that pixmap.
 * 3. Present is not the canvas backend — it is one or more
 *    ::lh_ui_present_sink_t (TFT, GDI, web, …) selected by a bit mask.
 *
 * Use ::lh_ui_display_get_canvas with ::lh_ui_view_t. After
 * ::lh_ui_view_draw / ::lh_ui_view_draw_frame, call ::lh_ui_display_present
 * with the canvas damage (or ::lh_null for the whole pixmap).
 *
 * On a microcontroller, set strip_height to ~1/10–1/5 of the panel (as LVGL
 * PARTIAL). On desktop, strip_height `0` and a window-sized pixmap is usual.
 */

#ifndef LH_UI_DISPLAY_H
#define LH_UI_DISPLAY_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/display/fields.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/pixmap/format.h>
#include <lh/ui/present.h>
#include <lh/ui/present/sink.h>
#include <lh/ui/rects.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/void.h>

/**
 * @struct lh_ui_display
 * @typedef lh_ui_display_t
 * @brief Panel policy + sw canvas over host bits + sink table.
 */
struct lh_ui_display
{
    lh_ui_display_fields(lh_ui_size_t, lh_ui_scalar_t, lh_ui_pixmap_format_t, lh_ui_pixmap_t,
                         lh_ui_canvas_sw_t, lh_ui_canvas_t, lh_ui_present_sink_t, lh_u32_t);
};
typedef struct lh_ui_display lh_ui_display_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty display: no bits, no sinks, panel zero, ARGB8888.
 */
lh_void
lh_ui_display_init(lh_ui_display_t *self);

/**
 * @brief Panel size in pixels (logical, top-left origin).
 */
lh_void
lh_ui_display_set_panel(lh_ui_display_t *self, lh_ui_size_t panel);

/**
 * @brief Pixel format of the host buffer (e.g. ::lh_ui_pixmap_format_rgb565).
 */
lh_void
lh_ui_display_set_format(lh_ui_display_t *self, lh_ui_pixmap_format_t format);

/**
 * @brief Rows in the host buffer. `0` → buffer height equals the panel height.
 */
lh_void
lh_ui_display_set_strip_height(lh_ui_display_t *self, lh_ui_scalar_t height);

/**
 * @brief Bind application-owned @p bits. Stride in bytes; height is the strip
 *        (or full panel when strip_height is `0`). Not copied, not freed.
 */
lh_void
lh_ui_display_set_buffer(lh_ui_display_t *self, lh_byte_ptr bits, lh_s32_t stride);

/**
 * @brief Panel Y of row 0 of the bound strip (partial mode). Ignored when
 *        strip_height is `0`.
 */
lh_void
lh_ui_display_set_strip_y(lh_ui_display_t *self, lh_ui_scalar_t y);

/**
 * @brief Replace sink @p index (must be &lt; ::LH_UI_PRESENT_SINKS_MAX). Grows
 *        @c sink_count to @p index+1 when needed. Enables that bit.
 */
lh_void
lh_ui_display_set_sink(lh_ui_display_t *self, lh_u32_t index, const lh_ui_present_sink_t *sink);

/**
 * @brief Enable or disable sink @p index without removing it.
 */
lh_void
lh_ui_display_set_sink_enabled(lh_ui_display_t *self, lh_u32_t index, lh_bool_t enabled);

/**
 * @brief True when sink @p index is present and enabled.
 */
lh_bool_t
lh_ui_display_is_sink_enabled(const lh_ui_display_t *self, lh_u32_t index);

/**
 * @brief Canvas bound to the software backend over the host pixmap.
 */
lh_ui_canvas_t *
lh_ui_display_get_canvas(lh_ui_display_t *self);

/**
 * @brief Current pixmap view (strip or full).
 */
const lh_ui_pixmap_t *
lh_ui_display_get_pixmap(const lh_ui_display_t *self);

/**
 * @brief Fan-out @p drawn (or the whole pixmap when ::lh_null) to every
 *        enabled sink.
 */
lh_void
lh_ui_display_present(lh_ui_display_t *self, const lh_ui_rects_t *drawn);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_DISPLAY_H */
