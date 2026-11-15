/**
 * @file pixmap.h
 * @brief A borrowed grid of colors: ::lh_gl_pixmap_t.
 *
 * The pixmap is one canvas target. It stores straight ::lh_gl_color_t pixels
 * and blends a fill over them (::lh_gl_color_over). A circle, a rounded
 * rectangle and a triangle are filled the same way, and a pixel outside the
 * shape is left alone. A stroke of a rectangle, a circle or a rounded
 * rectangle paints the band outside the shape, as thick as the pen. It does
 * not draw text, and it does not know about a layer or a surface. The caller
 * owns the pixel memory.
 *
 * ::lh_gl_pixmap_get_canvas_backend is the table for ::lh_gl_canvas_set_backend.
 * The canvas still translates and clips; the pixmap clips again to its own size.
 */

#ifndef LH_GL_PIXMAP_H
#define LH_GL_PIXMAP_H

#include <lh/gl/canvas/backend.h>
#include <lh/gl/pixmap/fields.h>

/**
 * @struct lh_gl_pixmap
 * @brief Borrowed colors and their size. Fields via ::lh_gl_pixmap_fields.
 */
struct lh_gl_pixmap
{
    /** Members: ::lh_gl_pixmap_fields. */
    lh_gl_pixmap_fields(lh_gl_color_t, lh_gl_size_t);
};
/** @copybrief lh_gl_pixmap */
typedef struct lh_gl_pixmap lh_gl_pixmap_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Borrowed pixels, or ::lh_null. */
lh_gl_color_t *
lh_gl_pixmap_get_pixels_mut(lh_gl_pixmap_t *self);

/** @brief Borrowed pixels, or ::lh_null. */
const lh_gl_color_t *
lh_gl_pixmap_get_pixels(const lh_gl_pixmap_t *self);

/** @brief Store a borrowed pixel row. @p pixels may be ::lh_null. */
lh_void
lh_gl_pixmap_set_pixels(lh_gl_pixmap_t *self, lh_gl_color_t *pixels);

/** @brief Size in pixels. */
const lh_gl_size_t *
lh_gl_pixmap_get_size(const lh_gl_pixmap_t *self);

/** @brief Store the size. The pixel memory must already cover it. */
lh_void
lh_gl_pixmap_set_size(lh_gl_pixmap_t *self, lh_gl_size_t size);

/**
 * @brief Borrow @p pixels of @p size. @p pixels may be ::lh_null.
 *
 * The pixmap does not allocate and does not free @p pixels.
 */
lh_void
lh_gl_pixmap_init(lh_gl_pixmap_t *self, lh_gl_color_t *pixels, lh_gl_size_t size);

/**
 * @brief Pixel at @p x, @p y, or a null pointer when @p self has no pixels.
 *
 * @p x and @p y are inside the size: the column is half-open at the width and
 * the row at the height.
 */
const lh_gl_color_t *
lh_gl_pixmap_get_pixel(const lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y);

/** @brief Mutable pixel at @p x, @p y. Same bounds as ::lh_gl_pixmap_get_pixel. */
lh_gl_color_t *
lh_gl_pixmap_get_pixel_mut(lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y);

/**
 * @brief The part of @p rect that @p clip and the pixmap both contain.
 *
 * Writes that rectangle to @p out. Returns false when @p self has no pixels or
 * the intersection is empty. @p out is untouched on failure.
 */
lh_bool_t
lh_gl_pixmap_limit(const lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                   lh_gl_rect_t *out);

/**
 * @brief Replace each pixel of @p rect with @p color.
 *
 * @p rect and @p clip are half-open and in pixmap coordinates, origin at the
 * top left. Their intersection is clipped to the pixmap. A null pixel row or
 * an empty intersection writes nothing. Alpha 255 replaces the row with one
 * word fill. A lower alpha blends over the pixel that is already there.
 */
lh_void
lh_gl_pixmap_fill_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                       const lh_gl_color_t *color);

/**
 * @brief ::lh_gl_canvas_fill_rect_fn for a pixmap stored in @p target.
 *
 * @p target is a ::lh_gl_pixmap_t. A null target returns.
 */
lh_void
lh_gl_pixmap_canvas_fill_rect(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                              const lh_gl_color_t *color);

/**
 * @brief Paint @p count colors starting at (@p x, @p y), one pixel each.
 *
 * @p colors[i] is pixel @p x + i. The run is clipped to @p clip and to the
 * pixmap. A run of alpha 255 is one copy. A lower alpha blends. A null row
 * writes nothing.
 */
lh_void
lh_gl_pixmap_fill_colors(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, lh_gl_scalar_t x,
                         lh_gl_scalar_t y, const lh_gl_color_t *colors, lh_gl_scalar_t count);

/** @brief ::lh_gl_canvas_fill_colors_fn for a pixmap stored in @p target. */
lh_void
lh_gl_pixmap_canvas_fill_colors(lh_ptr target, const lh_gl_rect_t *clip, lh_gl_scalar_t x,
                                lh_gl_scalar_t y, const lh_gl_color_t *colors,
                                lh_gl_scalar_t count);

/**
 * @brief Blend @p color over the one pixel (@p x, @p y).
 *
 * @p x and @p y are inside the size. A null pixel row writes nothing.
 */
lh_void
lh_gl_pixmap_blend_pixel(lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y,
                         const lh_gl_color_t *color);

/**
 * @brief Fill the circle of @p radius around @p center. A radius below 1
 *        writes nothing. Pixels outside the circle stay.
 */
lh_void
lh_gl_pixmap_fill_circle(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, lh_gl_point_t center,
                         lh_gl_scalar_t radius, const lh_gl_color_t *color);

/** @brief ::lh_gl_canvas_fill_circle_fn for a pixmap stored in @p target. */
lh_void
lh_gl_pixmap_canvas_fill_circle(lh_ptr target, const lh_gl_rect_t *clip, lh_gl_point_t center,
                                lh_gl_scalar_t radius, const lh_gl_color_t *color);

/**
 * @brief Fill @p rect with corners of @p radius. Radius `0` is the rectangle.
 *        A corner pixel outside the arc stays.
 */
lh_void
lh_gl_pixmap_fill_round_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip,
                             const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                             const lh_gl_color_t *color);

/** @brief ::lh_gl_canvas_fill_round_rect_fn for a pixmap stored in @p target. */
lh_void
lh_gl_pixmap_canvas_fill_round_rect(lh_ptr target, const lh_gl_rect_t *clip,
                                    const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                    const lh_gl_color_t *color);

/**
 * @brief Fill triangle @p a @p b @p c. A pixel outside it stays. No area
 *        writes nothing.
 */
lh_void
lh_gl_pixmap_fill_triangle(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_point_t *a,
                           const lh_gl_point_t *b, const lh_gl_point_t *c,
                           const lh_gl_color_t *color);

/** @brief ::lh_gl_canvas_fill_triangle_fn for a pixmap stored in @p target. */
lh_void
lh_gl_pixmap_canvas_fill_triangle(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_point_t *a,
                                  const lh_gl_point_t *b, const lh_gl_point_t *c,
                                  const lh_gl_color_t *color);

/**
 * @brief Blend @p color at (@p x, @p y) when @p outer covers the pixel and
 *        @p inner does not. Either coverage `0` skips the outside.
 */
lh_void
lh_gl_pixmap_blend_outside(lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y,
                           lh_gl_color_channel_t outer, lh_gl_color_channel_t inner,
                           const lh_gl_color_t *color);

/**
 * @brief Stroke @p rect on the outside. Thickness is the pen width. A width
 *        below 1 writes nothing. The interior stays.
 */
lh_void
lh_gl_pixmap_stroke_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                         const lh_gl_pen_t *pen);

/** @brief ::lh_gl_canvas_stroke_rect_fn for a pixmap stored in @p target. */
lh_void
lh_gl_pixmap_canvas_stroke_rect(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                                const lh_gl_pen_t *pen);

/**
 * @brief Stroke the circle on the outside, from @p radius out to
 *        @p radius plus the pen width. A radius or a width below 1 writes nothing.
 */
lh_void
lh_gl_pixmap_stroke_circle(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, lh_gl_point_t center,
                           lh_gl_scalar_t radius, const lh_gl_pen_t *pen);

/** @brief ::lh_gl_canvas_stroke_circle_fn for a pixmap stored in @p target. */
lh_void
lh_gl_pixmap_canvas_stroke_circle(lh_ptr target, const lh_gl_rect_t *clip, lh_gl_point_t center,
                                  lh_gl_scalar_t radius, const lh_gl_pen_t *pen);

/**
 * @brief Stroke the rounded rectangle on the outside. The arc grows by the
 *        pen width. A width below 1 writes nothing.
 */
lh_void
lh_gl_pixmap_stroke_round_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip,
                               const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                               const lh_gl_pen_t *pen);

/** @brief ::lh_gl_canvas_stroke_round_rect_fn for a pixmap stored in @p target. */
lh_void
lh_gl_pixmap_canvas_stroke_round_rect(lh_ptr target, const lh_gl_rect_t *clip,
                                      const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                      const lh_gl_pen_t *pen);

/** @brief Canvas table for the pixmap fills and strokes. */
const lh_gl_canvas_backend_t *
lh_gl_pixmap_get_canvas_backend(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_PIXMAP_H */
