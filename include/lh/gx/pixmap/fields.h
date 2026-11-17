/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_pixmap_t.
 */

#ifndef LH_GX_PIXMAP_FIELDS_H
#define LH_GX_PIXMAP_FIELDS_H

/**
 * @def lh_gx_pixmap_fields(bounds_type, length_type, format_type)
 * @brief The pixel bytes (not owned), the shape of the picture and the format.
 *
 * @param bounds_type Type of the pixel memory (::lh_memory_bounds_t).
 * @param length_type Type of the width, height (pixels) and stride (bytes).
 * @param format_type ::lh_gx_pixmap_format_t.
 */
#define lh_gx_pixmap_fields(bounds_type, length_type, format_type)                                  \
    bounds_type bits;                                                                              \
    length_type width;                                                                             \
    length_type height;                                                                            \
    length_type stride;                                                                            \
    format_type format

#endif /* LH_GX_PIXMAP_FIELDS_H */
