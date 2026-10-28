/**
 * @file shape.h
 * @brief ::lh_ui_gradient_shape_t — how the stops lie over a box.
 */

#ifndef LH_UI_GRADIENT_SHAPE_H
#define LH_UI_GRADIENT_SHAPE_H

/**
 * @enum lh_ui_gradient_shape
 * @brief How the stops lie over a box.
 */
typedef enum lh_ui_gradient_shape
{
    lh_ui_gradient_shape_linear = 0, /**< Along a line at ::lh_ui_gradient_get_angle. */
    lh_ui_gradient_shape_radial      /**< Out from the middle to the farthest corner. */
} lh_ui_gradient_shape_t;            /**< How the stops lie over a box. */

#endif /* LH_UI_GRADIENT_SHAPE_H */
