/**
 * @file initializer.h
 * @brief Brace-enclosed initializer of ::lh_gx_shadow_t, for a theme in flash.
 */

#ifndef LH_GX_SHADOW_INITIALIZER_H
#define LH_GX_SHADOW_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gx_shadow_initializer(color, spread, offset_x, offset_y)
 * @brief The members of a shadow, in order, for a constant:
 *        `lh_gx_shadow_initializer(0x80000000, 12, 0, 4)`.
 *
 * @param color    `0xAARRGGBB`, its alpha the darkest.
 * @param spread   How far the fade reaches either side of the edge.
 * @param offset_x How far right.
 * @param offset_y How far down.
 */
#define lh_gx_shadow_initializer(color, spread, offset_x, offset_y)                                \
    lh_initializer((color), (spread), (offset_x), (offset_y))

#endif /* LH_GX_SHADOW_INITIALIZER_H */
