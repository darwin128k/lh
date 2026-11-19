/**
 * @file box.h
 * @brief The card: ::lh_gx_box_class, a class that draws a rounded box with a border.
 *
 * The first class of gx, and the one most others are built on: a rectangle of
 * the entity's style — filled with `fill` or its `gradient` (laid over the whole
 * box, through the canvas's brush, `lh/gx/brush.h`), edged with `border` `border_width`
 * wide, each corner rounded to its own `radius` (::lh_gx_corners_t) and
 * anti-aliased. A button is a box that takes input; a card, a panel and a list
 * row are boxes; a tab is a box rounded on top only.
 *
 * How it draws, so that it is fast where it can be: the straight parts — the
 * bands of the border and the middle of the fill — go to the canvas as
 * rectangles, a handful per box whatever its size, a strip of rows at a time
 * (::lh_gx_box_paint_strip); only the pixels of the corners are computed one by
 * one (`lh/core/pixel/round.h`) and go through the canvas as masks
 * (`fill_mask`, ::lh_gx_box_paint_corner). On the corner the border is the ring
 * between the outer and the inner edge: the coverage of the outer rounded
 * rectangle less that of the inner one.
 *
 * It covers what is under it (`covers`) where its fill and its border are opaque
 * and the area is clear of its rounded corners, so a renderer draws nothing
 * under an opaque card.
 *
 * Each radius is at most ::LH_GX_BOX_RADIUS_MAX and at most half the shorter
 * side, so no two corners meet; the border at most half the shorter side.
 * Lengths are taken in whole pixels.
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

/**
 * @def LH_GX_BOX_CORNER_BYTES
 * @brief The bytes of one corner mask kept on the stack: a corner of radius `r`
 *        goes to the canvas in bands of `LH_GX_BOX_CORNER_BYTES / r` rows — a
 *        whole corner up to radius 32, four bands at radius 64.
 *
 * Two of them are on the stack while a corner is drawn (the fill's and the
 * border's), so the stack a box takes stays bounded whatever its radius.
 */
#define LH_GX_BOX_CORNER_BYTES 1024

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a box: `paint` ::lh_gx_box_paint, `covers` ::lh_gx_box_covers.
 */
extern const lh_gx_class_t lh_gx_box_class;

/**
 * @brief A corner's @p radius on @p outer, in whole pixels: at least `0`, at most
 *        half the shorter side and ::LH_GX_BOX_RADIUS_MAX — so no two corners of
 *        the box meet, whatever their radii.
 *
 * @param radius One corner's radius, from the style's ::lh_gx_corners_t.
 * @param outer  The box, whole pixels; must not be null.
 */
lh_s32_t
lh_gx_box_get_radius_px(lh_gx_scalar_t radius, const lh_math_rect_t *outer);

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
 * @brief Fill the pixel box @p x, @p y, @p width by @p height with @p brush through
 *        @p paint; nothing for an empty box or a clear brush.
 *
 * @param paint  The paint; must not be null.
 * @param x      Left column.
 * @param y      Top row.
 * @param width  Columns.
 * @param height Rows.
 * @param brush  What to fill with; must not be null.
 */
lh_void
lh_gx_box_fill_px(const lh_gx_paint_t *paint, lh_s32_t x, lh_s32_t y, lh_s32_t width,
                  lh_s32_t height, const lh_gx_brush_t *brush);

/**
 * @brief The brush the inside of the box is filled with: the style's gradient laid
 *        over @p place, with the style's table of its colours
 *        (::lh_gx_style_get_gradient_table); else the fill colour.
 *
 * The box works nothing of the gradient out itself: the theme fills the table
 * once, and the canvas draws. So a board whose canvas draws colours only
 * (::lh_gx_backend_sw_canvas_init_solid) links none of the gradient's code.
 *
 * @param style The style; must not be null.
 * @param place The whole box on the screen, which the gradient is laid over.
 * @param out   The brush; must not be null.
 */
lh_void
lh_gx_box_init_fill(const lh_gx_style_t *style, const lh_gx_rect_t *place, lh_gx_brush_t *out);

/**
 * @brief Draw one corner of the box: the @p radius by @p radius square from
 *        column @p x and row @p y — the fill through the inner rounded rectangle,
 *        the border through the ring between the inner and the outer one.
 *
 * The coverage of the square is worked out into a mask and handed to the
 * canvas whole: one `fill_mask` for the fill and one for the border per band of
 * rows (::LH_GX_BOX_CORNER_BYTES), not one per row; with no border the inner
 * shape is the outer one and its coverage is not worked out twice. On the
 * `cards` scene of `bench/lvgl/` (24 cards of radius 8, 480 by 320, i7-3770K)
 * the two took a whole frame from 337 to 257 microseconds, the same picture to
 * the byte (`bench/lvgl/RESULTS.md`).
 *
 * @param paint  The paint; must not be null.
 * @param outer  The box, whole pixels.
 * @param radius Its radius, whole pixels; at most ::LH_GX_BOX_RADIUS_MAX.
 * @param border Its border width, whole pixels.
 * @param fill   What the inside is filled with (::lh_gx_box_init_fill).
 * @param edge   What the border is filled with.
 * @param x      The corner's first column.
 * @param y      The corner's first row.
 */
lh_void
lh_gx_box_paint_corner(const lh_gx_paint_t *paint, const lh_math_rect_t *outer, lh_s32_t radius,
                       lh_s32_t border, const lh_gx_brush_t *fill, const lh_gx_brush_t *edge,
                       lh_s32_t x, lh_s32_t y);

/**
 * @brief The first column of row @p y that is not a left corner's: past the top
 *        left corner on its rows, past the bottom left one on its, else the box's
 *        left edge.
 *
 * @param style The style: the radii; must not be null.
 * @param outer The box, whole pixels; must not be null.
 * @param y     A row of the box.
 * @return A column, in whole pixels.
 */
lh_s32_t
lh_gx_box_get_left_px(const lh_gx_style_t *style, const lh_math_rect_t *outer, lh_s32_t y);

/**
 * @brief One past the last column of row @p y that is not a right corner's: the
 *        top right corner's first column on its rows, the bottom right one's on
 *        its, else the box's right edge.
 *
 * @param style The style: the radii; must not be null.
 * @param outer The box, whole pixels; must not be null.
 * @param y     A row of the box.
 * @return A column, in whole pixels.
 */
lh_s32_t
lh_gx_box_get_right_px(const lh_gx_style_t *style, const lh_math_rect_t *outer, lh_s32_t y);

/**
 * @brief Where the strip of rows that begins at @p y ends: the first row past @p y
 *        where a corner begins or ends or the border rows begin or end; the box's
 *        bottom edge when none does. Within a strip ::lh_gx_box_get_left_px,
 *        ::lh_gx_box_get_right_px and whether a row is the border are the same.
 *
 * @param style The style: the radii and the border; must not be null.
 * @param outer The box, whole pixels; must not be null.
 * @param y     A row of the box.
 * @return A row past @p y, at most the box's bottom edge.
 */
lh_s32_t
lh_gx_box_get_strip_end_px(const lh_gx_style_t *style, const lh_math_rect_t *outer, lh_s32_t y);

/**
 * @brief Whether @p area is clear of the rounded corners of the box at @p place:
 *        inside the box less its corner columns, or inside it less its corner
 *        rows — the two arms of a plus, each corner of its own radius.
 *
 * @param style The style: the radii; must not be null.
 * @param place The box on the screen.
 * @param area  The area, inside @p place.
 * @return True when no pixel of @p area is a corner's.
 */
lh_bool_t
lh_gx_box_is_clear_of_corners(const lh_gx_style_t *style, const lh_gx_rect_t *place,
                              const lh_gx_rect_t *area);

/**
 * @brief Draw rows @p top to @p bottom of the box less its corners: a border row
 *        across from corner to corner; any other row the border on each side that
 *        is not a corner's and the fill between — at most three rectangles.
 *
 * The rows are one strip (::lh_gx_box_get_strip_end_px); a box is its corners
 * (::lh_gx_box_paint_corner) and its strips.
 *
 * @param paint  The paint; must not be null.
 * @param style  The style: the radii and the border; must not be null.
 * @param outer  The box, whole pixels; must not be null.
 * @param fill   What the inside is filled with (::lh_gx_box_init_fill).
 * @param edge   What the border is filled with.
 * @param top    The strip's first row.
 * @param bottom One past its last row.
 */
lh_void
lh_gx_box_paint_strip(const lh_gx_paint_t *paint, const lh_gx_style_t *style,
                      const lh_math_rect_t *outer, const lh_gx_brush_t *fill,
                      const lh_gx_brush_t *edge, lh_s32_t top, lh_s32_t bottom);

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

/**
 * @brief `paint` of the box class (::lh_gx_class_paint_fn): the box of the entity's
 *        style at the paint's place — each rounded corner a mask, the rest a strip
 *        of rows at a time.
 *
 * An entity with no style draws nothing.
 *
 * @param entity The entity.
 * @param paint  The paint.
 */
lh_void
lh_gx_box_paint(const lh_gx_entity_t *entity, const lh_gx_paint_t *paint);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BOX_H */
