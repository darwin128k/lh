/**
 * @file decl.h
 * @brief Forward declaration of ::lh_gl_canvas_t.
 *
 * Only the name, so the draw call of an entity class has a signature before
 * any canvas exists. What a canvas is (a handle, a frame buffer, a command
 * list) is not decided here; the definition will come with `lh/gl/canvas.h`.
 */

#ifndef LH_GL_CANVAS_DECL_H
#define LH_GL_CANVAS_DECL_H

/**
 * @struct lh_gl_canvas
 * @brief The surface an entity draws on; not defined yet.
 */
struct lh_gl_canvas;
/** @copybrief lh_gl_canvas */
typedef struct lh_gl_canvas lh_gl_canvas_t;

#endif /* LH_GL_CANVAS_DECL_H */