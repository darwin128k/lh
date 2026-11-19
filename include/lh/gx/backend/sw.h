/**
 * @file sw.h
 * @brief The software backend of the canvas: ::lh_gx_backend_sw_t.
 *
 * Draws into a ::lh_gx_pixmap_t with the CPU — `argb8888`, `rgb565` and
 * `rgb565_swapped` — and fills the canvas contract (::lh_gx_canvas_t) with it:
 * ::lh_gx_backend_sw_canvas_init. The inner loops are the core's pixel spans
 * (`lh/core/pixel/span.h`), the same code gl's pixmap runs.
 *
 * The backend is the canvas's context: it holds where to draw (the target and
 * its origin on the screen) and nothing else — no clip, no damage, no frame.
 * Coordinates come in screen space; a strip buffer is the same backend with its
 * origin moved down the screen, and the canvas above does not know.
 *
 * Ownership: the backend, the target and the target's memory are the caller's;
 * the backend only writes pixels.
 */

#ifndef LH_GX_BACKEND_SW_H
#define LH_GX_BACKEND_SW_H

#include <lh/gx/backend/sw/fields.h>
#include <lh/gx/canvas.h>

/**
 * @struct lh_gx_backend_sw
 * @brief The target and where it sits on the screen.
 */
struct lh_gx_backend_sw
{
    /** Members: ::lh_gx_backend_sw_fields. */
    lh_gx_backend_sw_fields(lh_gx_pixmap_t, lh_math_point_t);
};
/** @copybrief lh_gx_backend_sw */
typedef struct lh_gx_backend_sw lh_gx_backend_sw_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The pixmap @p self draws into (level 1).
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_pixmap_t *
lh_gx_backend_sw_get_target(const lh_gx_backend_sw_t *self);

/**
 * @brief The pixmap @p self draws into, to write its pixels (level 1).
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 */
lh_gx_pixmap_t *
lh_gx_backend_sw_get_target_mut(const lh_gx_backend_sw_t *self);

/**
 * @brief Store the pixmap @p self draws into (level 1).
 *
 * @param self   The backend; checked by ::lh_assert_runtime_ref.
 * @param target The caller's pixmap.
 */
lh_void
lh_gx_backend_sw_set_target(lh_gx_backend_sw_t *self, lh_gx_pixmap_t *target);

/**
 * @brief The screen position of the target's top-left pixel (level 1).
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 */
const lh_math_point_t *
lh_gx_backend_sw_get_origin(const lh_gx_backend_sw_t *self);

/**
 * @brief Store the screen position of the target's top-left pixel (level 1).
 *
 * @param self   The backend; checked by ::lh_assert_runtime_ref.
 * @param origin The new position, in whole pixels.
 */
lh_void
lh_gx_backend_sw_set_origin(lh_gx_backend_sw_t *self, lh_math_point_t origin);

/**
 * @brief Draw into @p target, whose top-left pixel is at @p x, @p y on the screen.
 *
 * @param self   The backend; must not be null.
 * @param target The caller's pixmap, valid (::lh_gx_pixmap_is_valid).
 * @param x      Screen column of the target's left edge.
 * @param y      Screen row of the target's top edge.
 */
lh_void
lh_gx_backend_sw_init(lh_gx_backend_sw_t *self, lh_gx_pixmap_t *target, lh_math_scalar_t x,
                      lh_math_scalar_t y);

/**
 * @brief The pixels of @p rect inside @p clip and inside the target, in the
 *        target's own coordinates.
 *
 * Both rectangles are narrowed to whole pixels (::lh_gx_rect_to_rect), cut by
 * each other, moved by the origin, and cut by the target.
 *
 * @param self The backend; must not be null.
 * @param clip The clip, screen coordinates.
 * @param rect The rectangle, screen coordinates.
 * @param out  Receives the box; must not be null.
 * @return True when the box has at least one pixel.
 */
lh_bool_t
lh_gx_backend_sw_get_box(const lh_gx_backend_sw_t *self, const lh_gx_rect_t *clip,
                         const lh_gx_rect_t *rect, lh_math_rect_t *out);

/**
 * @brief Lay @p color over @p count pixels of the target from column @p x of row
 *        @p y, in the target's format (::lh_pixel_span_blend_argb,
 *        ::lh_pixel_span_blend_rgb565).
 *
 * @param self  The backend; must not be null.
 * @param x     The first column, target coordinates; the run must fit the target.
 * @param y     The row, target coordinates.
 * @param count How many pixels.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_backend_sw_blend_row(const lh_gx_backend_sw_t *self, lh_u32_t x, lh_u32_t y, lh_usize_t count,
                           lh_gx_color_t color);

/**
 * @brief Lay @p count colours of @p colors over as many pixels of the target, each
 *        at its own alpha (::lh_pixel_span_blend_each_argb, `_rgb565`).
 *
 * @param self   The backend; must not be null.
 * @param x      The first column, target coordinates; the run must fit the target.
 * @param y      The row, target coordinates.
 * @param count  How many pixels.
 * @param colors @p count colours, `0xAARRGGBB`.
 */
lh_void
lh_gx_backend_sw_blend_row_each(const lh_gx_backend_sw_t *self, lh_u32_t x, lh_u32_t y,
                                lh_usize_t count, const lh_u32_t *colors);

/**
 * @brief Lay @p color through @p count coverages of @p cover over as many pixels
 *        of the target (::lh_pixel_span_cover_argb, `_rgb565`).
 *
 * @param self  The backend; must not be null.
 * @param x     The first column, target coordinates; the run must fit the target.
 * @param y     The row, target coordinates.
 * @param count How many pixels.
 * @param cover @p count coverages, 0..255.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_backend_sw_cover_row(const lh_gx_backend_sw_t *self, lh_u32_t x, lh_u32_t y, lh_usize_t count,
                           const lh_byte_t *cover, lh_gx_color_t color);

/**
 * @brief Lay @p count colours through as many coverages over as many pixels of the
 *        target (::lh_pixel_span_cover_each_argb, `_rgb565`).
 *
 * @param self   The backend; must not be null.
 * @param x      The first column, target coordinates; the run must fit the target.
 * @param y      The row, target coordinates.
 * @param count  How many pixels.
 * @param cover  @p count coverages, 0..255.
 * @param colors @p count colours, `0xAARRGGBB`.
 */
lh_void
lh_gx_backend_sw_cover_row_each(const lh_gx_backend_sw_t *self, lh_u32_t x, lh_u32_t y,
                                lh_usize_t count, const lh_byte_t *cover, const lh_u32_t *colors);

/**
 * @brief Fill @p box of the target with one colour, a row at a time.
 *
 * @param self  The backend; must not be null.
 * @param box   Target coordinates, inside the target (::lh_gx_backend_sw_get_box).
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_backend_sw_fill_box_color(const lh_gx_backend_sw_t *self, const lh_math_rect_t *box,
                                lh_gx_color_t color);

/**
 * @brief Fill @p box of the target with the gradient of @p brush, laid over the
 *        brush's box on the screen (the target's origin taken into account).
 *
 * A gradient that runs straight up or down (::lh_pixel_gradient_is_vertical) has
 * one colour a row: its column is walked once and each row is a one-colour run, a
 * store when opaque. Any other gives each row its colours
 * (::lh_pixel_gradient_table_get_row) and lays them over
 * (::lh_gx_backend_sw_blend_row_each). The table is the brush's, or worked out
 * here once for the call (::lh_gx_brush_prepare).
 *
 * @param self  The backend; must not be null.
 * @param box   Target coordinates, inside the target.
 * @param brush A gradient brush; must not be null.
 */
lh_void
lh_gx_backend_sw_fill_box_gradient(const lh_gx_backend_sw_t *self, const lh_math_rect_t *box,
                                   const lh_gx_brush_t *brush);

/**
 * @brief `fill_mask` of the canvas (::lh_gx_canvas_fill_mask_fn) into the target:
 *        each row of the mask inside the clip and the target laid over with one
 *        colour (::lh_gx_backend_sw_cover_row) or the gradient's colours of that
 *        row (::lh_gx_backend_sw_cover_row_each).
 *
 * @param context The backend (::lh_gx_backend_sw_t).
 * @param clip    The clip, screen coordinates.
 * @param mask    The coverage, format `a8`.
 * @param at      Where the mask's top-left pixel lands, screen coordinates.
 * @param brush   What to fill with.
 */
lh_void
lh_gx_backend_sw_fill_mask(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_pixmap_t *mask,
                           lh_gx_point_t at, const lh_gx_brush_t *brush);

/**
 * @brief `fill_rect` of the canvas (::lh_gx_canvas_fill_rect_fn) into the target:
 *        nothing for a clear brush, else the part inside the clip and the target
 *        filled with one colour (::lh_gx_backend_sw_fill_box_color) or a gradient
 *        (::lh_gx_backend_sw_fill_box_gradient).
 *
 * @param context The backend (::lh_gx_backend_sw_t).
 * @param clip    The clip, screen coordinates.
 * @param rect    The rectangle, screen coordinates.
 * @param brush   What to fill with.
 */
lh_void
lh_gx_backend_sw_fill_rect(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_rect_t *rect,
                           const lh_gx_brush_t *brush);

/**
 * @brief Fill @p table with the slots of the software backend.
 *
 * The backend itself is the context the table is used with.
 *
 * @param table The table; must not be null.
 */
lh_void
lh_gx_backend_sw_canvas_init(lh_gx_canvas_t *table);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BACKEND_SW_H */
