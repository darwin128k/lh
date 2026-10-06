/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_mask_t.
 */

#ifndef LH_UI_MASK_FIELDS_H
#define LH_UI_MASK_FIELDS_H

/**
 * @def lh_ui_mask_fields(byte_type, length_type, depth_type)
 * @brief Packed coverage bytes (not owned), their size, row stride and depth.
 *
 * @param byte_type   Type of one packed byte.
 * @param length_type Type of the width, height and row stride.
 * @param depth_type  Type of the bits per pixel.
 */
#define lh_ui_mask_fields(byte_type, length_type, depth_type)                                       \
    const byte_type *bits;                                                                          \
    length_type width;                                                                              \
    length_type height;                                                                             \
    length_type row_bytes;                                                                          \
    depth_type bpp

#endif /* LH_UI_MASK_FIELDS_H */
