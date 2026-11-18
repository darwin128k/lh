/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_style_t.
 */

#ifndef LH_GX_STYLE_FIELDS_H
#define LH_GX_STYLE_FIELDS_H

/**
 * @def lh_gx_style_fields(color_type, scalar_type)
 * @brief How an entity looks: its fill, its border, the radius of its corners.
 *
 * Flat data, read by a class's `paint`: `fill` is the colour inside, `border` the
 * colour of the edge, `border_width` how wide the edge is (`0` for none), and
 * `radius` how round the corners are (`0` for square). A colour with alpha `0`
 * draws nothing. More looks (a font, a text colour, a shadow) are added when a
 * class needs them.
 *
 * @param color_type  Type of a colour (::lh_gx_color_t).
 * @param scalar_type Type of a length (::lh_gx_scalar_t).
 */
#define lh_gx_style_fields(color_type, scalar_type)                                                \
    color_type fill;                                                                               \
    color_type border;                                                                             \
    scalar_type border_width;                                                                      \
    scalar_type radius

#endif /* LH_GX_STYLE_FIELDS_H */
