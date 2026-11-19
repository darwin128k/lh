/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_shadow_t.
 */

#ifndef LH_GX_SHADOW_FIELDS_H
#define LH_GX_SHADOW_FIELDS_H

/**
 * @def lh_gx_shadow_fields(color_type, scalar_type)
 * @brief A soft shadow: its colour, how far it fades, how far it is shifted.
 *
 * `color` is `0xAARRGGBB`, its alpha the shadow's at its darkest; `spread` how far
 * the fade reaches either side of the box's edge (`0` casts nothing); `offset_x`,
 * `offset_y` how far right and down the shadow lies from the box.
 *
 * @param color_type  Type of a colour (::lh_gx_color_t).
 * @param scalar_type Type of a length (::lh_gx_scalar_t).
 */
#define lh_gx_shadow_fields(color_type, scalar_type)                                               \
    color_type color;                                                                              \
    scalar_type spread;                                                                            \
    scalar_type offset_x;                                                                          \
    scalar_type offset_y

#endif /* LH_GX_SHADOW_FIELDS_H */
