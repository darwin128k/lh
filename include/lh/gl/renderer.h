/**
 * @file renderer.h
 * @brief One deferred frame: ::lh_gl_renderer_t.
 *
 * The renderer borrows a surface and a canvas. It does not draw when an entity
 * is invalidated. ::lh_gl_renderer_render reads the dirty rectangles that have
 * accumulated, draws each one, and clears the list. A positive band height
 * slices a dirty rectangle into horizontal stripes so the caller can reuse one
 * partial buffer. The renderer stores no pixels and sends no packets.
 *
 * Skipping children that miss the clip, and children fully covered by a later
 * opaque sibling, happens in the layer walk the renderer calls.
 */

#ifndef LH_GL_RENDERER_H
#define LH_GL_RENDERER_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/math/scalar.h>
#include <lh/core/void.h>
#include <lh/gl/canvas/decl.h>
#include <lh/gl/renderer/fields.h>
#include <lh/gl/surface/decl.h>

/** @brief Borrowed surface, borrowed canvas and partial-buffer stripe height. */
struct lh_gl_renderer
{
    /** Members: ::lh_gl_renderer_fields. */
    lh_gl_renderer_fields(lh_gl_surface_t, lh_gl_canvas_t, lh_math_scalar_t);
};
/** @copybrief lh_gl_renderer */
typedef struct lh_gl_renderer lh_gl_renderer_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Borrowed surface, or ::lh_null. */
lh_gl_surface_t *
lh_gl_renderer_get_surface(const lh_gl_renderer_t *self);

/** @brief Store a borrowed surface. */
lh_void
lh_gl_renderer_set_surface(lh_gl_renderer_t *self, lh_gl_surface_t *surface);

/** @brief Borrowed canvas, or ::lh_null. */
lh_gl_canvas_t *
lh_gl_renderer_get_canvas(const lh_gl_renderer_t *self);

/** @brief Store a borrowed canvas. */
lh_void
lh_gl_renderer_set_canvas(lh_gl_renderer_t *self, lh_gl_canvas_t *canvas);

/**
 * @brief Partial-buffer stripe height.
 *
 * Zero and negative values mean one pass per dirty rectangle.
 */
lh_math_scalar_t
lh_gl_renderer_get_band(const lh_gl_renderer_t *self);

/** @brief Store the partial-buffer stripe height. */
lh_void
lh_gl_renderer_set_band(lh_gl_renderer_t *self, lh_math_scalar_t band);

/**
 * @brief Borrow @p surface and @p canvas. @p band is the stripe height.
 *
 * Neither pointer is owned. @p surface may be null until a later set. @p canvas
 * must be set before ::lh_gl_renderer_render.
 */
lh_void
lh_gl_renderer_init(lh_gl_renderer_t *self, lh_gl_surface_t *surface, lh_gl_canvas_t *canvas,
                    lh_math_scalar_t band);

/**
 * @brief Draw the surface's dirty rectangles and clear them.
 *
 * Does nothing when the surface or its root is missing. A positive band slices
 * each dirty rectangle from top to bottom. The canvas clip seen by each draw
 * is that stripe. After the frame the dirty count is zero, including areas
 * invalidated while drawing.
 */
lh_void
lh_gl_renderer_render(lh_gl_renderer_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_RENDERER_H */
