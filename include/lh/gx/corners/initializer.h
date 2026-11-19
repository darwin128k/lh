/**
 * @file initializer.h
 * @brief Brace-enclosed initializers of ::lh_gx_corners_t, for a constant.
 */

#ifndef LH_GX_CORNERS_INITIALIZER_H
#define LH_GX_CORNERS_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gx_corners_initializer(top_left, top_right, bottom_right, bottom_left)
 * @brief The four lengths, clockwise from the top left, for a constant — a
 *        style in flash among them (::lh_gx_style_initializer).
 *
 * @param top_left     The top left corner's.
 * @param top_right    The top right corner's.
 * @param bottom_right The bottom right corner's.
 * @param bottom_left  The bottom left corner's.
 */
#define lh_gx_corners_initializer(top_left, top_right, bottom_right, bottom_left)                  \
    lh_initializer((top_left), (top_right), (bottom_right), (bottom_left))

/**
 * @def lh_gx_corners_initializer_all(length)
 * @brief The same length on every corner, for a constant: the common case of a
 *        box rounded alike all round. @p length is named four times; it is
 *        meant to be a constant.
 *
 * @param length The length.
 */
#define lh_gx_corners_initializer_all(length)                                                      \
    lh_gx_corners_initializer(length, length, length, length)

#endif /* LH_GX_CORNERS_INITIALIZER_H */
