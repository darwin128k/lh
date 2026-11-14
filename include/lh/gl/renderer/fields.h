/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_renderer_t.
 */

#ifndef LH_GL_RENDERER_FIELDS_H
#define LH_GL_RENDERER_FIELDS_H

/**
 * @def lh_gl_renderer_fields(surface_type, canvas_type, band_type)
 * @brief The borrowed surface, the borrowed canvas and the stripe height.
 *
 * @param surface_type ::lh_gl_surface_t: `surface`, the dirty-rectangle owner.
 *                     Borrowed; may be null until set.
 * @param canvas_type  ::lh_gl_canvas_t: `canvas`, the drawing state passed to
 *                     the layer walk. Borrowed; may be null until set.
 * @param band_type    ::lh_gl_scalar_t: `band`, the partial-buffer height. Zero
 *                     or a negative value draws each dirty rectangle in one
 *                     pass. A positive value slices it into horizontal stripes
 *                     of at most this height.
 */
#define lh_gl_renderer_fields(surface_type, canvas_type, band_type)                                \
    surface_type *surface;                                                                         \
    canvas_type *canvas;                                                                           \
    band_type band

#endif /* LH_GL_RENDERER_FIELDS_H */
