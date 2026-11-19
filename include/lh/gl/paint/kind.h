/**
 * @file kind.h
 * @brief What a paint holds: ::lh_gl_paint_kind_t.
 */

#ifndef LH_GL_PAINT_KIND_H
#define LH_GL_PAINT_KIND_H

/**
 * @enum lh_gl_paint_kind
 * @brief Kind of an ::lh_gl_paint_t.
 *
 * A gradient kind is added once a renderer can sample ::lh_pixel_gradient_t.
 */
typedef enum lh_gl_paint_kind
{
    lh_gl_paint_kind_none = 0, /**< Paints nothing. */
    lh_gl_paint_kind_solid,    /**< One solid ::lh_gl_color_t. */
    lh_gl_paint_kind_gradient  /**< An ::lh_pixel_gradient_t over the box it fills. */
} lh_gl_paint_kind_t;          /**< Kind of an ::lh_gl_paint_t. */

#endif /* LH_GL_PAINT_KIND_H */
