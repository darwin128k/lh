/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_gradient_table_t.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_TABLE_FIELDS_H
#define LH_CORE_PIXEL_GRADIENT_TABLE_FIELDS_H

/**
 * @def lh_pixel_gradient_table_fields(color_type)
 * @brief The colour of every position of a gradient, `colors[frac]` for `frac`
 *        `0 .. 255`.
 *
 * @param color_type Type of a colour (::lh_u32_t, `0xAARRGGBB`).
 */
#define lh_pixel_gradient_table_fields(color_type) color_type colors[256]

#endif /* LH_CORE_PIXEL_GRADIENT_TABLE_FIELDS_H */
