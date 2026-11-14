/**
 * @file pixmap.h
 * @brief A borrowed grid of colors: ::lh_gl_pixmap_t.
 *
 * The pixmap is one canvas target. It stores straight ::lh_gl_color_t pixels
 * and blends a fill over them (::lh_gl_color_over). It does not draw circles,
 * strokes or text, and it does not know about
 * a layer or a surface. The caller owns the pixel memory.
 *
 * ::lh_gl_pixmap_get_canvas_backend is the table for ::lh_gl_canvas_set_backend.
 * Only the rectangle fill is set. The canvas still translates and clips; the
 * pixmap clips again to its own size.
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
 * @brief Replace each pixel of @p rect with @p color.
 *
 * @p rect and @p clip are half-open and in pixmap coordinates, origin at the
 * top left. Their intersection is clipped to the pixmap. A null pixel row or
 * an empty intersection writes nothing. Alpha 255 replaces the pixel. A lower
 * alpha blends over the pixel that is already there.
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

/** @brief Canvas table whose only call is ::lh_gl_pixmap_canvas_fill_rect. */
const lh_gl_canvas_backend_t *
lh_gl_pixmap_get_canvas_backend(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_PIXMAP_H */
