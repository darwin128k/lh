/**
 * @file canvas.h
 * @brief Drawing state shared with an entity's draw callback.
 *
 * The origin is the current entity's origin in root coordinates, and the clip
 * is the visible rectangle in root coordinates. Primitives take coordinates
 * relative to that origin. The canvas adds the origin, drops a shape that
 * misses the clip, and calls the borrowed backend with the root-coordinate
 * shape. It owns no pixels. The backend's target is an opaque handle.
 * Layer drawing saves and restores the origin and the clip.
 */

#ifndef LH_GL_CANVAS_H
#define LH_GL_CANVAS_H

#include <lh/gl/canvas/backend.h>
#include <lh/gl/canvas/decl.h>
#include <lh/gl/canvas/fields.h>

/** @brief Origin, clip, and the borrowed drawing target. */
struct lh_gl_canvas
{
    lh_gl_canvas_fields(lh_gl_point_t, lh_gl_rect_t, lh_ptr, lh_gl_canvas_backend_t);
};

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the origin.
 * @param self Canvas; must not be null.
 * @return Borrowed pointer into @p self, valid for its lifetime.
 */
const lh_gl_point_t *
lh_gl_canvas_get_origin(const lh_gl_canvas_t *self);

/**
 * @brief Replace the origin.
 * @param self Canvas; must not be null.
 * @param origin Root-coordinate origin, copied.
 */
lh_void
lh_gl_canvas_set_origin(lh_gl_canvas_t *self, lh_gl_point_t origin);

/**
 * @brief Read the clip.
 * @param self Canvas; must not be null.
 * @return Borrowed pointer into @p self, valid for its lifetime.
 */
const lh_gl_rect_t *
lh_gl_canvas_get_clip(const lh_gl_canvas_t *self);

/**
 * @brief Replace the clip.
 * @param self Canvas; must not be null.
 * @param clip Root-coordinate clip, copied.
 */
lh_void
lh_gl_canvas_set_clip(lh_gl_canvas_t *self, lh_gl_rect_t clip);

/**
 * @brief Initialize the drawing state; neither argument is borrowed.
 * @param self Canvas to initialize; must not be null.
 * @param origin Initial root-coordinate origin.
 * @param clip Initial root-coordinate clip.
 */
lh_void
lh_gl_canvas_init(lh_gl_canvas_t *self, lh_gl_point_t origin, lh_gl_rect_t clip);

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
 * @brief Fill @p rect. Coordinates are relative to the canvas origin.
 *
 * The root rectangle is @p rect shifted by the origin. A rectangle that misses
 * the clip is not passed on. Without a backend call, nothing is drawn.
 */
lh_void
lh_gl_canvas_fill_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, const lh_gl_color_t *color);

/**
 * @brief Stroke the border of @p rect with @p pen, in origin-relative coordinates.
 *
 * The pen grows the tested bounds outward. A negative pen width tests as zero.
 */
lh_void
lh_gl_canvas_stroke_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, const lh_gl_pen_t *pen);

/**
 * @brief Fill the circle around @p center. @p center is origin-relative.
 *        A radius below 1 draws nothing.
 */
lh_void
lh_gl_canvas_fill_circle(lh_gl_canvas_t *self, lh_gl_point_t center, lh_gl_scalar_t radius,
                         const lh_gl_color_t *color);

/** @brief Stroke that circle. The pen width grows the tested bounds. */
lh_void
lh_gl_canvas_stroke_circle(lh_gl_canvas_t *self, lh_gl_point_t center, lh_gl_scalar_t radius,
                           const lh_gl_pen_t *pen);

/**
 * @brief Fill @p rect with corner radius @p radius, origin-relative.
 *
 * @p radius is limited to half the shorter side before the backend sees it.
 * A negative radius becomes zero.
 */
lh_void
lh_gl_canvas_fill_round_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect,
                             lh_gl_scalar_t radius, const lh_gl_color_t *color);

/** @brief Stroke that rounded rectangle. */
lh_void
lh_gl_canvas_stroke_round_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect,
                               lh_gl_scalar_t radius, const lh_gl_pen_t *pen);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CANVAS_H */
