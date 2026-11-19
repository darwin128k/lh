/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_brush_t.
 */

#ifndef LH_GX_BRUSH_FIELDS_H
#define LH_GX_BRUSH_FIELDS_H

/**
 * @def lh_gx_brush_fields(color_type, gradient_type, table_type, rect_type)
 * @brief What a canvas fills with: one colour, or a gradient laid over a box.
 *
 * `color` is the colour, `0xAARRGGBB`; with a gradient it is the gradient's first
 * colour, for a canvas that draws colours only. `gradient` is ::lh_null for one
 * colour; else `box` is the rectangle it is laid over (screen coordinates —
 * the whole card, not the part a call fills), and `table` its 256 colours,
 * worked out once for every call that fills the card (::lh_null: the canvas
 * works them out itself). The brush points at the gradient and the table; it
 * owns neither.
 *
 * @param color_type    Type of a colour (::lh_gx_color_t).
 * @param gradient_type Type of the gradient (::lh_pixel_gradient_t).
 * @param table_type    Type of its colour table (::lh_pixel_gradient_table_t).
 * @param rect_type     Type of the box (::lh_gx_rect_t).
 */
#define lh_gx_brush_fields(color_type, gradient_type, table_type, rect_type)                       \
    color_type color;                                                                              \
    const gradient_type *gradient;                                                                 \
    const table_type *table;                                                                       \
    rect_type box

#endif /* LH_GX_BRUSH_FIELDS_H */
