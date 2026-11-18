/**
 * @file box.h
 * @brief The card: ::lh_gx_box_class, a class that draws a rounded box with a border.
 *
 * The first class of gx, and the one most others are built on: a rectangle of
 * the entity's style — filled with `fill`, edged with `border` `border_width`
 * wide, its corners rounded to `radius` and anti-aliased. A button is a box that
 * takes input; a card, a panel and a list row are boxes.
 *
 * How it draws, so that it is fast where it can be: the straight parts — the
 * bands of the border and the middle of the fill — go to the canvas as
 * rectangles, a handful per box whatever its size; only the pixels of the
 * corners are computed one by one (`lh/core/pixel/round.h`), a row of a corner
 * at a time, and go through the canvas as one-row masks (`fill_mask`). On the
 * corner the border is the ring between the outer and the inner edge: the
 * coverage of the outer rounded rectangle less that of the inner one.
 *
 * It covers what is under it (`covers`) where its fill and its border are opaque
 * and the area is clear of its rounded corners, so a renderer draws nothing
 * under an opaque card.
 *
 * The radius is at most ::LH_GX_BOX_RADIUS_MAX — the mask of one corner row is
 * on the stack — and at most half the shorter side; the border at most half the
 * shorter side. Lengths are taken in whole pixels.
 */

#ifndef LH_GX_BOX_H
#define LH_GX_BOX_H

#include <lh/gx/class.h>
#include <lh/gx/entity.h>
#include <lh/gx/paint.h>
#include <lh/gx/style.h>

/**
 * @def LH_GX_BOX_RADIUS_MAX
 * @brief The largest radius a box draws, in pixels: the length of the mask of one
 *        corner row, kept on the stack.
 */
#define LH_GX_BOX_RADIUS_MAX 64

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a box: `paint` ::lh_gx_box_paint, `covers` ::lh_gx_box_covers.
 */
extern const lh_gx_class_t lh_gx_box_class;

/**
 * @brief The radius of @p style on @p outer, in whole pixels: at least `0`, at most
 *        half the shorter side and ::LH_GX_BOX_RADIUS_MAX.
 *
 * @param style The style; must not be null.
 * @param outer The box, whole pixels.
 */
lh_s32_t
lh_gx_box_get_radius_px(const lh_gx_style_t *style, const lh_math_rect_t *outer);

/**
 * @brief The border width of @p style on @p outer, in whole pixels: at least `0`,
 *        at most half the shorter side; `0` when the border colour is clear.
 *
 * @param style The style; must not be null.
 * @param outer The box, whole pixels.
 */
lh_s32_t
lh_gx_box_get_border_px(const lh_gx_style_t *style, const lh_math_rect_t *outer);

/**
 * @brief Fill the pixel box @p x, @p y, @p width by @p height with @p color through
 *        @p paint; nothing for an empty box or a clear colour.
 *
 * @param paint  The paint; must not be null.
 * @param x      Left column.
 * @param y      Top row.
 * @param width  Columns.
 * @param height Rows.
 * @param color  `0xAARRGGBB`.
 */
lh_void
lh_gx_box_fill_px(const lh_gx_paint_t *paint, lh_s32_t x, lh_s32_t y, lh_s32_t width,
                  lh_s32_t height, lh_gx_color_t color);

/**
 * @brief Draw the corner pixels of row @p y from column @p x for @p count columns:
 *        the fill through the inner rounded rectangle, the border through the ring
 *        between the inner and the outer one.
 *
 * @param paint  The paint; must not be null.
 * @param outer  The box, whole pixels.
 * @param radius Its radius, whole pixels.
 * @param border Its border width, whole pixels.
 * @param style  Its style: the colours.
 * @param x      The first column of the corner.
 * @param y      The row.
 * @param count  The columns, at most ::LH_GX_BOX_RADIUS_MAX.
 */
lh_void
lh_gx_box_paint_corner_row(const lh_gx_paint_t *paint, const lh_math_rect_t *outer, lh_s32_t radius,
                           lh_s32_t border, const lh_gx_style_t *style, lh_s32_t x, lh_s32_t y,
                           lh_s32_t count);

/**
 * @brief `paint` of the box class (::lh_gx_class_paint_fn): the box of the entity's
 *        style at the paint's place.
 *
 * An entity with no style draws nothing.
 *
 * @param entity The entity.
 * @param paint  The paint.
 */
lh_void
lh_gx_box_paint(const lh_gx_entity_t *entity, const lh_gx_paint_t *paint);

/**
 * @brief `covers` of the box class (::lh_gx_class_covers_fn): true when the fill —
 *        and the border, if there is one — is opaque and @p area is clear of the
 *        rounded corners.
 *
 * @param entity The entity.
 * @param place  Its rectangle on the screen.
 * @param area   The area, inside @p place.
 * @return True when nothing under the box shows through @p area.
 */
lh_bool_t
lh_gx_box_covers(const lh_gx_entity_t *entity, const lh_gx_rect_t *place, const lh_gx_rect_t *area);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BOX_H */
