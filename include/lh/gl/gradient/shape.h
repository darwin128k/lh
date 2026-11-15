/**
 * @file shape.h
 * @brief ::lh_gl_gradient_shape_t — how the stops lie over a box.
 */

#ifndef LH_GL_GRADIENT_SHAPE_H
#define LH_GL_GRADIENT_SHAPE_H

/**
 * @enum lh_gl_gradient_shape
 * @brief How the stops lie over a box.
 */
typedef enum lh_gl_gradient_shape
{
    lh_gl_gradient_shape_linear = 0, /**< Along a line at ::lh_gl_gradient_get_angle. */
    lh_gl_gradient_shape_radial      /**< Out from the middle to the farthest corner. */
} lh_gl_gradient_shape_t;            /**< How the stops lie over a box. */

#endif /* LH_GL_GRADIENT_SHAPE_H */
