/**
 * @file brush.h
 * @brief What the canvas fills with: ::lh_gx_brush_t.
 *
 * The canvas has two slots, `fill_rect` and `fill_mask` (`lh/gx/canvas.h`), and
 * both fill with a brush: one colour (::lh_gx_brush_init_color), or a gradient
 * laid over a box (::lh_gx_brush_init_gradient). A card with a gradient is drawn
 * in many calls — its corners, the strips between them — and every call is
 * given the same brush, laid over the whole card, so the pieces join into one
 * gradient. A backend draws the gradient its own way: the software one through
 * the colour table and the sampler of `lh/core/pixel/gradient/`, the web one as
 * the browser's gradient; a backend that draws colours only fills with the
 * brush's colour, the gradient's first.
 *
 * A value on the stack of whoever draws: it points at the gradient and its table
 * and owns neither; both must outlive the calls the brush is given to.
 */

#ifndef LH_GX_BRUSH_H
#define LH_GX_BRUSH_H

#include <lh/core/pixel/gradient/table.h>
#include <lh/gx/brush/fields.h>
#include <lh/gx/color.h>
#include <lh/gx/rect.h>

/**
 * @struct lh_gx_brush
 * @brief One colour, or a gradient over a box. Members: ::lh_gx_brush_fields.
 */
struct lh_gx_brush
{
    /** Members: ::lh_gx_brush_fields. */
    lh_gx_brush_fields(lh_gx_color_t, lh_pixel_gradient_t, lh_pixel_gradient_table_t, lh_gx_rect_t);
};
/** @copybrief lh_gx_brush */
typedef struct lh_gx_brush lh_gx_brush_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The colour, `0xAARRGGBB`; with a gradient, its first colour (level 1).
 *
 * @param self The brush; checked by ::lh_assert_runtime_ref.
 */
lh_gx_color_t
lh_gx_brush_get_color(const lh_gx_brush_t *self);

/**
 * @brief Store the colour (level 1).
 *
 * @param self  The brush; checked by ::lh_assert_runtime_ref.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_brush_set_color(lh_gx_brush_t *self, lh_gx_color_t color);

/**
 * @brief The gradient, or ::lh_null for one colour (level 1).
 *
 * @param self The brush; checked by ::lh_assert_runtime_ref.
 * @return Not owned.
 */
const lh_pixel_gradient_t *
lh_gx_brush_get_gradient(const lh_gx_brush_t *self);

/**
 * @brief Store the gradient (level 1).
 *
 * @param self     The brush; checked by ::lh_assert_runtime_ref.
 * @param gradient The gradient, or ::lh_null; not owned.
 */
lh_void
lh_gx_brush_set_gradient(lh_gx_brush_t *self, const lh_pixel_gradient_t *gradient);

/**
 * @brief The gradient's 256 colours, or ::lh_null when the canvas is to work them
 *        out (level 1).
 *
 * @param self The brush; checked by ::lh_assert_runtime_ref.
 * @return Not owned.
 */
const lh_pixel_gradient_table_t *
lh_gx_brush_get_table(const lh_gx_brush_t *self);

/**
 * @brief Store the gradient's colours (level 1).
 *
 * @param self  The brush; checked by ::lh_assert_runtime_ref.
 * @param table The table, or ::lh_null; not owned.
 */
lh_void
lh_gx_brush_set_table(lh_gx_brush_t *self, const lh_pixel_gradient_table_t *table);

/**
 * @brief The box the gradient is laid over, screen coordinates (level 1).
 *
 * @param self The brush; checked by ::lh_assert_runtime_ref.
 * @return The brush's own rectangle.
 */
const lh_gx_rect_t *
lh_gx_brush_get_box(const lh_gx_brush_t *self);

/**
 * @brief Store the box the gradient is laid over (level 1).
 *
 * @param self The brush; checked by ::lh_assert_runtime_ref.
 * @param box  Screen coordinates.
 */
lh_void
lh_gx_brush_set_box(lh_gx_brush_t *self, lh_gx_rect_t box);

/**
 * @brief Whether the brush is a gradient, not one colour.
 *
 * @param self The brush; checked by ::lh_gx_brush_get_gradient.
 */
lh_bool_t
lh_gx_brush_is_gradient(const lh_gx_brush_t *self);

/**
 * @brief The colour the brush gives pixel @p x, @p y (screen coordinates): its one
 *        colour, or the gradient's at the pixel's position over the box — from the
 *        table when the brush has one, else worked out
 *        (::lh_pixel_gradient_get_color_at). A pixel at a time: for a canvas with
 *        no better way, and for tests.
 *
 * @param self The brush; checked by the getters.
 * @param x    The pixel's column.
 * @param y    The pixel's row.
 * @return `0xAARRGGBB`.
 */
lh_gx_color_t
lh_gx_brush_get_color_at(const lh_gx_brush_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Make ready to draw the brush's gradient a run at a time: lay it over the
 *        box (@p sampler) and give the table of its colours — the brush's own,
 *        or @p own filled when it has none.
 *
 * @param self    The brush; must be a gradient (::lh_gx_brush_is_gradient).
 * @param own     Room for a table, used when the brush has none.
 * @param sampler Receives the gradient over the brush's box.
 * @return The table to look colours up in: the brush's or @p own.
 */
const lh_pixel_gradient_table_t *
lh_gx_brush_prepare(const lh_gx_brush_t *self, lh_pixel_gradient_table_t *own,
                    lh_pixel_gradient_sampler_t *sampler);

/**
 * @brief One colour, @p color: no gradient, no table, an empty box.
 *
 * @param self  The brush; checked by the setters.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_brush_init_color(lh_gx_brush_t *self, lh_gx_color_t color);

/**
 * @brief @p gradient laid over @p box, its colours from @p table; the colour is
 *        the gradient's first — its first stop's, read, clear with no stops — so
 *        making a brush works nothing of the gradient out.
 *
 * @param self     The brush; checked by the setters.
 * @param gradient The gradient; must not be null; not owned.
 * @param table    Its colours (::lh_pixel_gradient_table_fill), or ::lh_null;
 *                 not owned.
 * @param box      The whole card it is laid over, screen coordinates.
 */
lh_void
lh_gx_brush_init_gradient(lh_gx_brush_t *self, const lh_pixel_gradient_t *gradient,
                          const lh_pixel_gradient_table_t *table, lh_gx_rect_t box);

/**
 * @brief Whether the brush draws nothing: one colour with alpha `0`.
 *
 * @param self The brush; checked by the getters.
 */
lh_bool_t
lh_gx_brush_is_clear(const lh_gx_brush_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BRUSH_H */
