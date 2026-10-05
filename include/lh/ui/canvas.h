/**
 * @file canvas.h
 * @brief A pixel buffer to draw into, with a clip rectangle.
 *
 * The canvas does not own its pixels and does not know what they belong to.
 * A drawing call takes an area (::lh_math_rect_t) and either a ::lh_ui_brush_t
 * or a ::lh_ui_pen_t. Each of those carries a solid color or a gradient.
 * Anything outside the clip is left alone. Whoever shows
 * the buffer — a window, a browser, a framebuffer — is not this type.
 */

#ifndef LH_UI_CANVAS_H
#define LH_UI_CANVAS_H

#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
#include <lh/math/rect.h>
#include <lh/ui/brush.h>
#include <lh/ui/canvas/fields.h>
#include <lh/ui/color.h>
#include <lh/ui/pen.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas
 * @typedef lh_ui_canvas_t
 * @brief Pixels, their size, and the clip.
 */
struct lh_ui_canvas
{
    lh_ui_canvas_fields(lh_ui_color_t, lh_math_coord_t, lh_math_rect_t);
};
typedef struct lh_ui_canvas lh_ui_canvas_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Draw into @p width by @p height pixels at @p pixels. Rows are
 *        @p stride pixels apart. The clip starts as the whole image.
 */
lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, lh_ui_color_t *pixels, lh_math_coord_t width,
                  lh_math_coord_t height, lh_math_coord_t stride);

/**
 * @brief Image width in pixels.
 */
lh_math_coord_t
lh_ui_canvas_get_width(const lh_ui_canvas_t *self);

/**
 * @brief Image height in pixels.
 */
lh_math_coord_t
lh_ui_canvas_get_height(const lh_ui_canvas_t *self);

/**
 * @brief Where drawing is currently allowed.
 */
lh_math_rect_t
lh_ui_canvas_get_clip(const lh_ui_canvas_t *self);

/**
 * @brief Allow drawing only inside @p clip, cut to the image.
 */
lh_void
lh_ui_canvas_set_clip(lh_ui_canvas_t *self, lh_math_rect_t clip);

/**
 * @brief The pixel at (@p x, @p y). It must lie inside the image.
 */
const lh_ui_color_t *
lh_ui_canvas_get_pixel(const lh_ui_canvas_t *self, lh_math_coord_t x, lh_math_coord_t y);

/**
 * @brief Paint @p brush over every pixel of @p rect that lies inside the clip.
 *
 * Alpha `0` changes nothing. Alpha `255` replaces the pixel.
 */
lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, lh_math_rect_t rect, const lh_ui_brush_t *brush);

/**
 * @brief Outline @p rect with @p pen, inside the rect.
 *
 * The rect does not grow. A width that consumes the interior fills the rect.
 * A width of 0 paints nothing.
 */
lh_void
lh_ui_canvas_stroke_rect(lh_ui_canvas_t *self, lh_math_rect_t rect, const lh_ui_pen_t *pen);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_H */
