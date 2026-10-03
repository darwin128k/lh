/**
 * @file canvas.h
 * @brief A pixel buffer to draw into, with a clip rectangle.
 *
 * The canvas does not own its pixels: it draws into memory the caller
 * provides (a window's back buffer, a framebuffer on a microcontroller, a
 * texture). Pixels are ::lh_ui_color_t (RGBA, 8 bits per channel); whoever
 * shows the image converts to the device's format.
 *
 * Every drawing call stays inside the clip rectangle, which is how entities
 * are kept inside their parents and how only the changed parts of a screen
 * are redrawn.
 */

#ifndef LH_UI_CANVAS_H
#define LH_UI_CANVAS_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/canvas/fields.h>
#include <lh/ui/color.h>
#include <lh/ui/geom.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas
 * @brief Fields via ::lh_ui_canvas_fields.
 */
struct lh_ui_canvas
{
    lh_ui_canvas_fields(lh_ui_color_t, lh_ui_coord_t, lh_ui_rect_t);
};
typedef struct lh_ui_canvas lh_ui_canvas_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Draw into @p width x @p height pixels at @p pixels, rows @p stride
 *        pixels apart. The clip starts as the whole image.
 */
lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, lh_ui_color_t *pixels, lh_ui_coord_t width,
                  lh_ui_coord_t height, lh_ui_coord_t stride);

/**
 * @brief Image width in pixels.
 */
lh_ui_coord_t
lh_ui_canvas_get_width(const lh_ui_canvas_t *self);

/**
 * @brief Image height in pixels.
 */
lh_ui_coord_t
lh_ui_canvas_get_height(const lh_ui_canvas_t *self);

/**
 * @brief Where drawing is currently allowed.
 */
lh_ui_rect_t
lh_ui_canvas_get_clip(const lh_ui_canvas_t *self);

/**
 * @brief Allow drawing only inside @p clip (cut to the image).
 */
lh_void
lh_ui_canvas_set_clip(lh_ui_canvas_t *self, lh_ui_rect_t clip);

/**
 * @brief The pixel at (@p x, @p y), which must be inside the image.
 */
lh_ui_color_t
lh_ui_canvas_get_pixel(const lh_ui_canvas_t *self, lh_ui_coord_t x, lh_ui_coord_t y);

/**
 * @brief Draw @p color over the pixel at (@p x, @p y), blended by its alpha;
 *        nothing outside the clip.
 */
lh_void
lh_ui_canvas_blend_pixel(lh_ui_canvas_t *self, lh_ui_coord_t x, lh_ui_coord_t y,
                         lh_ui_color_t color);

/**
 * @brief Draw @p color over every pixel of @p rect, blended by its alpha;
 *        nothing outside the clip.
 */
lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, lh_ui_rect_t rect, lh_ui_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_H */
