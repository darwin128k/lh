/**
 * @file decl.h
 * @brief Forward declaration of ::lh_gl_canvas_t.
 *
 * Only the name, so entity draw callbacks can refer to a canvas without
 * including its drawing-state definition in `lh/gl/canvas.h`.
 */

#ifndef LH_GL_CANVAS_DECL_H
#define LH_GL_CANVAS_DECL_H

/**
 * @struct lh_gl_canvas
 * @brief Drawing state passed to an entity draw callback.
 */
struct lh_gl_canvas;
/** @copybrief lh_gl_canvas */
typedef struct lh_gl_canvas lh_gl_canvas_t;

#endif /* LH_GL_CANVAS_DECL_H */