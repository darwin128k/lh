/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_t.
 */

#ifndef LH_UI_CANVAS_FIELDS_H
#define LH_UI_CANVAS_FIELDS_H

/**
 * @def lh_ui_canvas_fields(pixel_type, coord_type, rect_type)
 * @brief The pixels drawn into and where drawing is allowed.
 *
 * `pixels` is the first row. Row `y` starts at `pixels + y * stride`.
 * `clip` is always inside the image. The canvas does not own `pixels`.
 *
 * @param pixel_type Type of one pixel.
 * @param coord_type Type of `width`, `height` and `stride`.
 * @param rect_type  Type of `clip`.
 */
#define lh_ui_canvas_fields(pixel_type, coord_type, rect_type)                                      \
    pixel_type *pixels;                                                                             \
    coord_type width;                                                                               \
    coord_type height;                                                                              \
    coord_type stride;                                                                              \
    rect_type clip

#endif /* LH_UI_CANVAS_FIELDS_H */
