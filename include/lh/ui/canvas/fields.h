/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_t.
 */

#ifndef LH_UI_CANVAS_FIELDS_H
#define LH_UI_CANVAS_FIELDS_H

/**
 * @def lh_ui_canvas_fields(pixel_type, coord_type, rect_type, depth_type)
 * @brief The pixels drawn into and where drawing is allowed.
 *
 * - `pixels`: the first row; row `y` starts at `pixels + y * stride`.
 * - `width`, `height`: the image size in pixels.
 * - `stride`: pixels from one row to the next (>= `width`).
 * - `clip`: drawing outside it is discarded; always inside the image.
 * - `depth`: one ::lh_float_t per pixel, rows `stride` apart, or null when
 *   the painter's order is the only rule. A larger value is closer.
 * - `draw_z`: depth written by the next fill or pixel.
 *
 * @param pixel_type Type of one pixel (::lh_ui_color_t).
 * @param coord_type Type of `width`, `height` and `stride` (::lh_math_coord_t).
 * @param rect_type  Type of `clip` (::lh_math_rect_t).
 * @param depth_type Type of one depth sample and of `draw_z` (::lh_float_t).
 */
#define lh_ui_canvas_fields(pixel_type, coord_type, rect_type, depth_type)                         \
    pixel_type *pixels;                                                                            \
    coord_type width;                                                                              \
    coord_type height;                                                                             \
    coord_type stride;                                                                             \
    rect_type clip;                                                                                \
    depth_type *depth;                                                                             \
    depth_type draw_z

#endif /* LH_UI_CANVAS_FIELDS_H */
