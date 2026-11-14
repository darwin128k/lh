/**
 * @file canvas.h
 * @brief Drawing state shared with an entity's draw callback.
 *
 * The origin is the current entity's origin in root coordinates, and the clip
 * is the visible rectangle in root coordinates. The canvas owns no output or
 * pixel storage. A caller initializes its state; layer drawing saves and
 * restores it across callbacks and recursive traversal.
 */

#ifndef LH_GL_CANVAS_H
#define LH_GL_CANVAS_H

#include <lh/gl/canvas/decl.h>
#include <lh/gl/canvas/fields.h>
#include <lh/gl/rect.h>

/** @brief A drawing origin and clip, both in root coordinates. */
struct lh_gl_canvas
{
    lh_gl_canvas_fields(lh_gl_point_t, lh_gl_rect_t);
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

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CANVAS_H */
