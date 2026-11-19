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
#include <lh/gx/backend/sw/shadow.h>

/**
 * @def LH_GX_BACKEND_SW_SHADOW_RUN
 * @brief The alphas of a shadow's row worked out at a time on the stack; a longer
 *        row is done in pieces this long.
 */
#define LH_GX_BACKEND_SW_SHADOW_RUN 128
#include <lh/gx/canvas.h>

/**
 * @struct lh_gx_backend_sw
 * @brief The target and where it sits on the screen.
 */
struct lh_gx_backend_sw
{
    /** Members: ::lh_gx_backend_sw_fields. */
    lh_gx_backend_sw_fields(lh_gx_pixmap_t, lh_math_point_t, lh_gx_backend_sw_shadow_t);
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
 * @brief The mask a shadow's alphas are kept in for the next one like it, or
 *        ::lh_null (level 1).
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 * @return The caller's mask.
 */
lh_gx_backend_sw_shadow_t *
lh_gx_backend_sw_get_shadow(const lh_gx_backend_sw_t *self);

/**
 * @brief Give the backend a mask to keep a shadow's alphas in (level 1): every
 *        shadow like the last one is then that mask laid down again
 *        (::lh_gx_backend_sw_fill_shadow); ::lh_null works every shadow out where
 *        it falls.
 *
 * @param self   The backend; checked by ::lh_assert_runtime_ref.
 * @param shadow The caller's mask (::lh_gx_backend_sw_shadow_init), or ::lh_null;
 *               not owned.
 */
lh_void
lh_gx_backend_sw_set_shadow(lh_gx_backend_sw_t *self, lh_gx_backend_sw_shadow_t *shadow);

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
 * @brief Where a mask placed at @p at lands in the target, cut by @p clip and the
 *        target: the box, target coordinates, and the pixel of the mask its
 *        top-left corner is.
 *
 * @param self   The backend; must not be null.
 * @param clip   The clip, screen coordinates.
 * @param mask   The coverage; must not be null.
 * @param at     Where the mask's top-left pixel lands, screen coordinates.
 * @param box    Receives the box; must not be null.
 * @param inside Receives the mask pixel at the box's top-left; must not be null.
 * @return True when the box has at least one pixel.
 */
lh_bool_t
lh_gx_backend_sw_get_mask_box(const lh_gx_backend_sw_t *self, const lh_gx_rect_t *clip,
                              const lh_gx_pixmap_t *mask, lh_gx_point_t at, lh_math_rect_t *box,
                              lh_math_point_t *inside);

/**
 * @brief Lay the gradient of @p brush through @p mask over @p box of the target, a
 *        row at a time (::lh_pixel_gradient_table_get_row,
 *        ::lh_gx_backend_sw_cover_row_each).
 *
 * @param self   The backend; must not be null.
 * @param mask   The coverage; must not be null.
 * @param box    Target coordinates (::lh_gx_backend_sw_get_mask_box).
 * @param inside The mask pixel at the box's top-left.
 * @param brush  A gradient brush; must not be null.
 */
lh_void
lh_gx_backend_sw_fill_mask_gradient(const lh_gx_backend_sw_t *self, const lh_gx_pixmap_t *mask,
                                    const lh_math_rect_t *box, const lh_math_point_t *inside,
                                    const lh_gx_brush_t *brush);

/**
 * @brief Lay the alphas @p kept holds over @p area of the target, in @p color: the
 *        mask placed at @p bounds on the screen.
 *
 * @param self   The backend; must not be null.
 * @param area   Target coordinates, inside @p bounds less the origin.
 * @param bounds Where the mask's first alpha lands, screen coordinates, whole pixels.
 * @param kept   The mask; must hold the shadow's alphas.
 * @param color  `0xFF` and the shadow's colour: the alphas are the coverage.
 */
lh_void
lh_gx_backend_sw_stamp_shadow(const lh_gx_backend_sw_t *self, const lh_math_rect_t *area,
                              const lh_math_rect_t *bounds, const lh_gx_backend_sw_shadow_t *kept,
                              lh_gx_color_t color);

/**
 * @brief Work @p cast out over @p area of the target and lay it in @p color: its ramp
 *        once, then every row a run of ::LH_GX_BACKEND_SW_SHADOW_RUN alphas at a time
 *        (::lh_pixel_shadow_get_span, ::lh_gx_backend_sw_cover_row). Nothing when it
 *        casts nothing.
 *
 * @param self  The backend; must not be null.
 * @param area  Target coordinates.
 * @param cast  The core's shadow, screen coordinates; must not be null.
 * @param color `0xFF` and the shadow's colour.
 */
lh_void
lh_gx_backend_sw_lay_shadow(const lh_gx_backend_sw_t *self, const lh_math_rect_t *area,
                            const lh_pixel_shadow_t *cast, lh_gx_color_t color);

/**
 * @brief `fill_rect` of a canvas that draws colours only: the brush's colour (a
 *        gradient's first) over the part inside the clip and the target.
 *
 * Calls nothing of the gradient, so a program whose canvas is
 * ::lh_gx_backend_sw_canvas_init_solid links none of it.
 *
 * @param context The backend (::lh_gx_backend_sw_t).
 * @param clip    The clip, screen coordinates.
 * @param rect    The rectangle, screen coordinates.
 * @param brush   What to fill with; only its colour is read.
 */
lh_void
lh_gx_backend_sw_fill_rect_solid(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_rect_t *rect,
                                 const lh_gx_brush_t *brush);

/**
 * @brief `fill_mask` of a canvas that draws colours only: the brush's colour
 *        through the mask (::lh_gx_backend_sw_cover_row). Calls nothing of the
 *        gradient.
 *
 * @param context The backend (::lh_gx_backend_sw_t).
 * @param clip    The clip, screen coordinates.
 * @param mask    The coverage, format `a8`.
 * @param at      Where the mask's top-left pixel lands, screen coordinates.
 * @param brush   What to fill with; only its colour is read.
 */
lh_void
lh_gx_backend_sw_fill_mask_solid(lh_ptr context, const lh_gx_rect_t *clip,
                                 const lh_gx_pixmap_t *mask, lh_gx_point_t at,
                                 const lh_gx_brush_t *brush);

/**
 * @brief `fill_shadow` of the canvas (::lh_gx_canvas_fill_shadow_fn) into the target:
 *        with a mask (::lh_gx_backend_sw_get_shadow) that holds or can hold this
 *        shadow's alphas, the mask laid down (::lh_gx_backend_sw_stamp_shadow) —
 *        worked out first when it held another; else the shadow worked out where
 *        it falls (::lh_gx_backend_sw_lay_shadow).
 *
 * On the `shadow` scene of `bench/lvgl/` — 24 cards casting the same shadow — the
 * mask took a frame from 1368 to 917 microseconds.
 *
 * @param context The backend (::lh_gx_backend_sw_t).
 * @param clip    The clip, screen coordinates.
 * @param box     The casting box, screen coordinates.
 * @param radius  Its corners' radius, pixels.
 * @param shadow  The shadow.
 */
lh_void
lh_gx_backend_sw_fill_shadow(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_rect_t *box,
                             lh_gx_scalar_t radius, const lh_gx_shadow_t *shadow);

/**
 * @brief `fill_mask` of the canvas (::lh_gx_canvas_fill_mask_fn) into the target:
 *        one colour as ::lh_gx_backend_sw_fill_mask_solid, a gradient through
 *        ::lh_gx_backend_sw_fill_mask_gradient.
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
 *        one colour as ::lh_gx_backend_sw_fill_rect_solid, a gradient over the
 *        part inside the clip and the target
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
 * @brief Fill @p table with the slots of the software backend that draw colours
 *        only (::lh_gx_backend_sw_fill_rect_solid, `_fill_mask_solid`): a
 *        gradient brush draws its first colour, and no `fill_shadow`, so no shadow
 *        is drawn.
 *
 * For a board that draws no gradient and no shadow: nothing of either is called,
 * so none of it is linked. On the Cortex-M4 of `bench/mcu/` a button screen is
 * that much smaller (`bench/mcu/RESULTS.md`).
 *
 * @param table The table; must not be null.
 */
lh_void
lh_gx_backend_sw_canvas_init_solid(lh_gx_canvas_t *table);

/**
 * @brief Fill @p table with the slots of the software backend: `fill_rect` and
 *        `fill_mask` with one colour or a gradient, and `fill_shadow`.
 *
 * The backend itself is the context the table is used with.
 *
 * @param table The table; must not be null.
 */
lh_void
lh_gx_backend_sw_canvas_init(lh_gx_canvas_t *table);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BACKEND_SW_H */
