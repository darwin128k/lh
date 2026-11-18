/**
 * @file initializer.h
 * @brief Brace-enclosed initializer of ::lh_gx_style_t, for a theme in flash.
 */

#ifndef LH_GX_STYLE_INITIALIZER_H
#define LH_GX_STYLE_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gx_style_initializer(fill, border, border_width, radius, font)
 * @brief The members of a style, in order, for a constant.
 *
 * A theme is the application's array of styles, usually in flash:
 * `const lh_gx_style_t my_theme[] = { lh_gx_style_initializer(0xFFFFFFFF,
 * 0xFFCCCCCC, 1, 8, lh_null), … };`, and every entity points at one of them.
 *
 * @param fill         The colour inside, `0xAARRGGBB`.
 * @param border       The colour of the edge.
 * @param border_width How wide the edge is; `0` for none.
 * @param radius       How round the corners are; `0` for square.
 * @param font         Where a text takes its glyphs; ::lh_null for none.
 */
#define lh_gx_style_initializer(fill, border, border_width, radius, font)                          \
    lh_initializer((fill), (border), (border_width), (radius), (font))

#endif /* LH_GX_STYLE_INITIALIZER_H */
