/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_gradient_stop_t.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_STOP_FIELDS_H
#define LH_CORE_PIXEL_GRADIENT_STOP_FIELDS_H

/**
 * @def lh_pixel_gradient_stop_fields(color_type, frac_type)
 * @brief A colour and where along the gradient it stands.
 *
 * `color` is `0xAARRGGBB`; `frac` the position, `0` at the start of the gradient
 * and `255` at its end.
 *
 * @param color_type Type of the colour (::lh_u32_t).
 * @param frac_type  Type of the position (::lh_byte_t).
 */
#define lh_pixel_gradient_stop_fields(color_type, frac_type)                                       \
    color_type color;                                                                              \
    frac_type frac

#endif /* LH_CORE_PIXEL_GRADIENT_STOP_FIELDS_H */
