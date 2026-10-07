/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_pixmap_t.
 */

#ifndef LH_UI_PIXMAP_FIELDS_H
#define LH_UI_PIXMAP_FIELDS_H

/**
 * @def lh_ui_pixmap_fields(pixel_type, length_type)
 * @brief Pixel words (not owned), their size, and the row stride.
 *
 * @param pixel_type  Type of one ARGB8888 pixel word.
 * @param length_type Type of the width, height and stride (in pixels).
 */
#define lh_ui_pixmap_fields(pixel_type, length_type)                                                \
    pixel_type *pixels;                                                                             \
    length_type width;                                                                              \
    length_type height;                                                                             \
    length_type stride

#endif /* LH_UI_PIXMAP_FIELDS_H */
