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
 * - `pixels`: the first row; row `y` starts at `pixels + y * stride`.
 * - `width`, `height`: the image size in pixels.
 * - `stride`: pixels from one row to the next (>= `width`).
 * - `clip`: drawing outside it is discarded; always inside the image.
 *
 * @param pixel_type Type of one pixel (::lh_ui_color_t).
 * @param coord_type Type of `width`, `height` and `stride` (::lh_ui_coord_t).
 * @param rect_type  Type of `clip` (::lh_ui_rect_t).
 */
#define lh_ui_canvas_fields(pixel_type, coord_type, rect_type)                                     \
    pixel_type *pixels;                                                                            \
    coord_type width;                                                                              \
    coord_type height;                                                                             \
    coord_type stride;                                                                             \
    rect_type clip

#endif /* LH_UI_CANVAS_FIELDS_H */
