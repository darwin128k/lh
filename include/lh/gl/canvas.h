/**
 * @file canvas.h
 * @brief Drawing commands for one borrowed target.
 *
 * A canvas is the tool: it fills and strokes shapes into the target. It does
 * not remember an origin, a clip or a mask. Each command takes a ::lh_gl_place_t
 * for that call. Coordinates on the shape are relative to the place origin.
 * The canvas shifts them, drops a shape that misses the clip, and scales a
 * pixel by the mask when the place has one. It owns no pixels.
 */

#ifndef LH_GL_CANVAS_H
#define LH_GL_CANVAS_H

#include <lh/gl/canvas/backend.h>
#include <lh/gl/canvas/decl.h>
#include <lh/gl/canvas/fields.h>
#include <lh/gl/place.h>

/** @brief Borrowed target and call table. */
struct lh_gl_canvas
{
    lh_gl_canvas_fields(lh_ptr, lh_gl_canvas_backend_t);
};

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Clear the target and the backend. */
lh_void
lh_gl_canvas_init(lh_gl_canvas_t *self);

/** @brief Opaque target the backend draws into, or ::lh_null. */
lh_ptr
lh_gl_canvas_get_target(const lh_gl_canvas_t *self);

/** @brief Store the opaque target. Not owned. */
lh_void
lh_gl_canvas_set_target(lh_gl_canvas_t *self, lh_ptr target);

/** @brief Borrowed call table, or ::lh_null when the canvas draws nothing. */
const lh_gl_canvas_backend_t *
lh_gl_canvas_get_backend(const lh_gl_canvas_t *self);

/** @brief Store the call table. Not owned. ::lh_null draws nothing. */
lh_void
lh_gl_canvas_set_backend(lh_gl_canvas_t *self, const lh_gl_canvas_backend_t *backend);

/**
 * @brief Fill @p rect. Coordinates are relative to the place origin.
 *
 * The root rectangle is @p rect shifted by the origin. A rectangle that misses
 * the clip is not passed on. Without a backend call, nothing is drawn.
 */
lh_void
lh_gl_canvas_fill_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_rect_t *rect,
                       const lh_gl_color_t *color);

/**
 * @brief Paint @p count colors along one row, origin-relative.
 *
 * @p colors[i] is pixel @p x + i on row @p y. Without a backend call, each
 * pixel is one ::lh_gl_canvas_fill_rect.
 */
lh_void
lh_gl_canvas_fill_colors(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_scalar_t x,
                         lh_gl_scalar_t y, const lh_gl_color_t *colors, lh_gl_scalar_t count);

/**
 * @brief Stroke the border of @p rect with @p pen, in origin-relative coordinates.
 *
 * The pen grows the tested bounds outward. A negative pen width tests as zero.
 */
lh_void
lh_gl_canvas_stroke_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_rect_t *rect,
                         const lh_gl_pen_t *pen);

/**
 * @brief Fill the circle around @p center. @p center is origin-relative.
 *        A radius below 1 draws nothing.
 */
lh_void
lh_gl_canvas_fill_circle(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_point_t center,
                         lh_gl_scalar_t radius, const lh_gl_color_t *color);

/** @brief Stroke that circle. The pen width grows the tested bounds. */
lh_void
lh_gl_canvas_stroke_circle(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_point_t center,
                           lh_gl_scalar_t radius, const lh_gl_pen_t *pen);

/**
 * @brief Fill @p rect with corner radius @p radius, origin-relative.
 *
 * @p radius is limited to half the shorter side before the backend sees it.
 * A negative radius becomes zero.
 */
lh_void
lh_gl_canvas_fill_round_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place,
                             const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                             const lh_gl_color_t *color);

/** @brief Stroke that rounded rectangle. */
lh_void
lh_gl_canvas_stroke_round_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place,
                               const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                               const lh_gl_pen_t *pen);

/**
 * @brief Fill the triangle @p a @p b @p c. The points are origin-relative.
 *        A triangle with no area draws nothing.
 */
lh_void
lh_gl_canvas_fill_triangle(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_point_t *a,
                           const lh_gl_point_t *b, const lh_gl_point_t *c,
                           const lh_gl_color_t *color);

/**
 * @brief Stroke the straight segment from @p a to @p b with @p pen.
 *
 * Both points are origin-relative. A pen width below 1 draws nothing. Width
 * 1 is one pixel. A wider pen paints a square of that side on each pixel,
 * centered on it. The mask, when the place has one, scales each pixel.
 */
lh_void
lh_gl_canvas_stroke_line(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_point_t *a,
                         const lh_gl_point_t *b, const lh_gl_pen_t *pen);

/**
 * @brief Fill a sector. @p center is origin-relative.
 *
 * @p start and @p sweep are degrees, the same way as ::lh_gl_cover_sector.
 * A radius below 1 draws nothing. Each covered pixel is one fill, so the
 * place mask scales it.
 */
lh_void
lh_gl_canvas_fill_sector(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_point_t center,
                         lh_gl_scalar_t radius, lh_gl_scalar_t start, lh_gl_scalar_t sweep,
                         const lh_gl_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CANVAS_H */
