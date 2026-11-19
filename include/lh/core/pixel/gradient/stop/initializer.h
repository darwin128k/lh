/**
 * @file initializer.h
 * @brief Brace-enclosed initializer of ::lh_pixel_gradient_stop_t, for a constant.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_STOP_INITIALIZER_H
#define LH_CORE_PIXEL_GRADIENT_STOP_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_pixel_gradient_stop_initializer(color, frac)
 * @brief The members of a stop, in order, for a constant:
 *        `lh_pixel_gradient_stop_initializer(0xFFFFFFFFU, 128)`.
 *
 * @param color The colour, `0xAARRGGBB`.
 * @param frac  Where it stands, `0 .. 255`.
 */
#define lh_pixel_gradient_stop_initializer(color, frac) lh_initializer((color), (frac))

#endif /* LH_CORE_PIXEL_GRADIENT_STOP_INITIALIZER_H */
