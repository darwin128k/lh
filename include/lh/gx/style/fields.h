/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_style_t.
 */

#ifndef LH_GX_STYLE_FIELDS_H
#define LH_GX_STYLE_FIELDS_H

/**
 * @def lh_gx_style_fields(color_type, scalar_type, corners_type, font_type, gradient_type,
 *                         table_type, shadow_type)
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
 * entity; the style points at it and does not own it. `gradient_table` is its 256
 * colours worked out once (::lh_pixel_gradient_table_fill) by whoever made the
 * theme, so that no frame works them out again; ::lh_null leaves that to the
 * canvas, on every call. `shadow` is the soft shadow the entity casts
 * (::lh_gx_shadow_t), ::lh_null for none; the style points at it. More looks are
 * added when a class needs them.
 *
 * @param color_type   Type of a colour (::lh_gx_color_t).
 * @param scalar_type  Type of a length (::lh_gx_scalar_t).
 * @param corners_type Type of a length per corner (::lh_gx_corners_t).
 * @param font_type    Type of the font (::lh_gx_font_t).
 * @param gradient_type Type of the gradient (::lh_pixel_gradient_t).
 * @param table_type   Type of its colour table (::lh_pixel_gradient_table_t).
 * @param shadow_type  Type of the shadow (::lh_gx_shadow_t).
 */
#define lh_gx_style_fields(color_type, scalar_type, corners_type, font_type, gradient_type,        \
                           table_type, shadow_type)                                                \
    color_type fill;                                                                               \
    color_type border;                                                                             \
    scalar_type border_width;                                                                      \
    corners_type radius;                                                                           \
    color_type ink;                                                                                \
    const font_type *font;                                                                         \
    const gradient_type *gradient;                                                                 \
    const table_type *gradient_table;                                                              \
    const shadow_type *shadow

#endif /* LH_GX_STYLE_FIELDS_H */
