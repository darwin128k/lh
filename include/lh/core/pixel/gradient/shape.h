/**
 * @file shape.h
 * @brief How the stops of a gradient lie over a box: ::lh_pixel_gradient_shape_t.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_SHAPE_H
#define LH_CORE_PIXEL_GRADIENT_SHAPE_H

/**
 * @enum lh_pixel_gradient_shape
 * @brief How the stops of a gradient lie over a box.
 */
enum lh_pixel_gradient_shape
{
    /** Along a line at the gradient's angle, corner to corner of the box seen
        along it (::lh_pixel_gradient_get_angle). */
    lh_pixel_gradient_shape_linear = 0,
    /** Out from the middle of the box to its corners, on an ellipse of its shape. */
    lh_pixel_gradient_shape_radial
};
/** @copybrief lh_pixel_gradient_shape */
typedef enum lh_pixel_gradient_shape lh_pixel_gradient_shape_t;

#endif /* LH_CORE_PIXEL_GRADIENT_SHAPE_H */
