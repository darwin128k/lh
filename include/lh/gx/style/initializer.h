/**
 * @file initializer.h
 * @brief Brace-enclosed initializer of ::lh_gx_style_t, for a theme in flash.
 */

#ifndef LH_GX_STYLE_INITIALIZER_H
#define LH_GX_STYLE_INITIALIZER_H

#include <lh/gx/corners/initializer.h>

/**
 * @def lh_gx_style_initializer(fill, border, border_width, radius, ink, font, gradient)
 * @brief The members of a style, in order, for a constant.
 *
 * A theme is the application's array of styles, usually in flash:
 * `const lh_gx_style_t my_theme[] = { lh_gx_style_initializer(0xFFFFFFFF,
 * 0xFFCCCCCC, 1, lh_gx_corners_initializer_all(8), 0xFF000000, lh_null, lh_null),
 * … };`,
 * and every entity points at one of them.
 *
 * @param fill         The colour inside, `0xAARRGGBB`.
 * @param border       The colour of the edge.
 * @param border_width How wide the edge is; `0` for none.
 * @param radius       How round each corner is: ::lh_gx_corners_initializer, or
 *                     ::lh_gx_corners_initializer_all for one radius all round.
 * @param ink          The colour of a text.
 * @param font         Where a text takes its glyphs; ::lh_null for none.
 * @param gradient     What fills the inside instead of @p fill, a gradient in
 *                     flash too; ::lh_null for @p fill.
 */
#define lh_gx_style_initializer(fill, border, border_width, radius, ink, font, gradient)           \
    lh_initializer((fill), (border), (border_width), radius, (ink), (font), (gradient))

#endif /* LH_GX_STYLE_INITIALIZER_H */
