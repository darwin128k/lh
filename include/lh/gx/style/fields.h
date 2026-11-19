/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_style_t.
 */

#ifndef LH_GX_STYLE_FIELDS_H
#define LH_GX_STYLE_FIELDS_H

/**
 * @def lh_gx_style_fields(color_type, scalar_type, corners_type, font_type, gradient_type)
 * @brief How an entity looks: its fill, its border, the radius of its corners, its
 *        ink, its font, and a gradient its fill may be.
 *
 * Flat data, read by a class's `paint`: `fill` is the colour inside, `border` the
 * colour of the edge, `border_width` how wide the edge is (`0` for none), and
 * `radius` how round each corner is (`0` for square), clockwise from the top
 * left, `ink` the colour a text is drawn in, and `font` where a text takes its
 * glyphs from (::lh_gx_font_t; ::lh_null for an entity with no text). A colour
 * with alpha `0` draws nothing. The fill and the ink are two colours, so one
 * style serves a whole control: a button's box takes the fill and the border,
 * its caption the ink and the font. `gradient` is what fills the inside instead
 * of `fill` when it is not ::lh_null (::lh_pixel_gradient_t), laid over the whole
 * entity; the style points at it and does not own it. More looks (a shadow) are
 * added when a class needs them.
 *
 * @param color_type   Type of a colour (::lh_gx_color_t).
 * @param scalar_type  Type of a length (::lh_gx_scalar_t).
 * @param corners_type Type of a length per corner (::lh_gx_corners_t).
 * @param font_type    Type of the font (::lh_gx_font_t).
 * @param gradient_type Type of the gradient (::lh_pixel_gradient_t).
 */
#define lh_gx_style_fields(color_type, scalar_type, corners_type, font_type, gradient_type)        \
    color_type fill;                                                                               \
    color_type border;                                                                             \
    scalar_type border_width;                                                                      \
    corners_type radius;                                                                           \
    color_type ink;                                                                                \
    const font_type *font;                                                                         \
    const gradient_type *gradient

#endif /* LH_GX_STYLE_FIELDS_H */
