/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_pixmap_t.
 */

#ifndef LH_GL_PIXMAP_FIELDS_H
#define LH_GL_PIXMAP_FIELDS_H

/**
 * @def lh_gl_pixmap_fields(byte_type, length_type, format_type)
 * @brief Pixel bytes (not owned), their size, the row stride and the format.
 *
 * @param byte_type   Type of one byte of pixel storage.
 * @param length_type Type of the width, height (pixels) and stride (bytes).
 * @param format_type ::lh_gl_pixmap_format_t.
 */
#define lh_gl_pixmap_fields(byte_type, length_type, format_type)                                   \
    byte_type *bits;                                                                               \
    length_type width;                                                                             \
    length_type height;                                                                            \
    length_type stride;                                                                            \
    format_type format

#endif /* LH_GL_PIXMAP_FIELDS_H */
