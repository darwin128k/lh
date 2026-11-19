/**
 * @file paint.h
 * @brief What a class draws with: ::lh_gx_paint_t.
 *
 * The renderer hands one paint to the `paint` slot of an entity's class
 * (::lh_gx_class_paint_fn): the canvas to draw on, the clip, and the entity's
 * rectangle on the screen. The class draws through the paint — "fill my
 * rectangle with my colour" — and never sees the clip: the paint cuts every
 * shape by it on the way to the canvas. So a class knows what an entity of its
 * kind looks like, and nothing about where on the screen it is drawn or what
 * else is there.
 *
 * A paint is a value on the renderer's stack for one call; it owns nothing.
 * Where the canvas has no slot for a shape, the paint brings it down to one it
 * has (::lh_gx_paint_fill_mask without `fill_mask`).
 */

#ifndef LH_GX_PAINT_H
#define LH_GX_PAINT_H

#include <lh/gx/canvas.h>
#include <lh/gx/paint/fields.h>

/**
 * @struct lh_gx_paint
 * @brief The canvas, the clip, and the place of the entity being painted.
 */
struct lh_gx_paint
{
    /** Members: ::lh_gx_paint_fields. */
    lh_gx_paint_fields(lh_gx_canvas_t, lh_gx_rect_t);
};
/** @copybrief lh_gx_paint */
typedef struct lh_gx_paint lh_gx_paint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The canvas table the paint draws on (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_canvas_t *
lh_gx_paint_get_canvas(const lh_gx_paint_t *self);

/**
 * @brief Store the canvas table the paint draws on (level 1).
 *
 * @param self   The paint; checked by ::lh_assert_runtime_ref.
 * @param canvas The table; not owned.
 */
lh_void
lh_gx_paint_set_canvas(lh_gx_paint_t *self, const lh_gx_canvas_t *canvas);

/**
 * @brief The context every canvas call gets: the backend (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gx_paint_get_canvas_context(const lh_gx_paint_t *self);

/**
 * @brief Store the context every canvas call gets (level 1).
 *
 * @param self    The paint; checked by ::lh_assert_runtime_ref.
 * @param context The backend; not owned.
 */
lh_void
lh_gx_paint_set_canvas_context(lh_gx_paint_t *self, lh_ptr context);

/**
 * @brief The part of the screen the paint may touch (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_rect_t *
lh_gx_paint_get_clip(const lh_gx_paint_t *self);

/**
 * @brief Store the part of the screen the paint may touch (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param clip Screen coordinates.
 */
lh_void
lh_gx_paint_set_clip(lh_gx_paint_t *self, lh_gx_rect_t clip);

/**
 * @brief The rectangle of the entity being painted, on the screen (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_rect_t *
lh_gx_paint_get_place(const lh_gx_paint_t *self);

/**
 * @brief Store the rectangle of the entity being painted (level 1).
 *
 * @param self  The paint; checked by ::lh_assert_runtime_ref.
 * @param place Screen coordinates.
 */
lh_void
lh_gx_paint_set_place(lh_gx_paint_t *self, lh_gx_rect_t place);

/**
 * @brief A paint on @p canvas with @p context, inside @p clip, for an entity at
 *        @p place.
 *
 * @param self    The paint; must not be null.
 * @param canvas  The canvas table; must have `fill_rect`.
 * @param context The backend the table is used with.
 * @param clip    The part of the screen the paint may touch.
 * @param place   The entity's rectangle on the screen.
 */
lh_void
lh_gx_paint_init(lh_gx_paint_t *self, const lh_gx_canvas_t *canvas, lh_ptr context,
                 lh_gx_rect_t clip, lh_gx_rect_t place);

/**
 * @brief Fill @p rect with @p brush, cut by the clip: the canvas's `fill_rect`.
 *
 * @param self  The paint; must not be null.
 * @param rect  The rectangle, screen coordinates.
 * @param brush What to fill with; must not be null.
 */
lh_void
lh_gx_paint_fill_rect(const lh_gx_paint_t *self, const lh_gx_rect_t *rect,
                      const lh_gx_brush_t *brush);

/**
 * @brief Fill @p rect with one colour, cut by the clip: ::lh_gx_paint_fill_rect
 *        with a brush of @p color (::lh_gx_brush_init_color).
 *
 * @param self  The paint; must not be null.
 * @param rect  The rectangle, screen coordinates.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_paint_fill_rect_color(const lh_gx_paint_t *self, const lh_gx_rect_t *rect,
                            lh_gx_color_t color);

/**
 * @brief Lay @p brush through @p mask with its top-left corner at @p at, cut by
 *        the clip, one pixel at a time through `fill_rect`.
 *
 * The fallback for a canvas without `fill_mask`: every pixel of the mask with
 * any coverage becomes a one-pixel rectangle of the brush's colour, at the alpha
 * the coverage gives it. A gradient brush draws its first colour here, as a
 * canvas that draws colours only does (::lh_gx_backend_sw_canvas_init_solid): a
 * canvas that draws gradients has its own `fill_mask`, and the fallback calls
 * nothing of the gradient, so a board without one links none of it. Correct on
 * every canvas for one colour, and slow — a backend that draws text fills
 * `fill_mask`.
 *
 * @param self  The paint; must not be null.
 * @param mask  The coverage, format `a8`.
 * @param at    Where the mask's top-left pixel lands, screen coordinates.
 * @param brush What to fill with; must not be null.
 */
lh_void
lh_gx_paint_fill_mask_by_rects(const lh_gx_paint_t *self, const lh_gx_pixmap_t *mask,
                               lh_gx_point_t at, const lh_gx_brush_t *brush);

/**
 * @brief Lay @p brush through @p mask with its top-left corner at @p at, cut by
 *        the clip.
 *
 * Through the canvas's `fill_mask` when it has one, else
 * ::lh_gx_paint_fill_mask_by_rects.
 *
 * @param self  The paint; must not be null.
 * @param mask  The coverage, format `a8`.
 * @param at    Where the mask's top-left pixel lands, screen coordinates.
 * @param brush What to fill with; must not be null.
 */
lh_void
lh_gx_paint_fill_mask(const lh_gx_paint_t *self, const lh_gx_pixmap_t *mask, lh_gx_point_t at,
                      const lh_gx_brush_t *brush);

/**
 * @brief Lay one colour through @p mask: ::lh_gx_paint_fill_mask with a brush of
 *        @p color — a glyph.
 *
 * @param self  The paint; must not be null.
 * @param mask  The coverage, format `a8`.
 * @param at    Where the mask's top-left pixel lands, screen coordinates.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_gx_paint_fill_mask_color(const lh_gx_paint_t *self, const lh_gx_pixmap_t *mask, lh_gx_point_t at,
                            lh_gx_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_PAINT_H */
