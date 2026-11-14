/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_pixmap_t.
 */

#ifndef LH_GL_PIXMAP_FIELDS_H
#define LH_GL_PIXMAP_FIELDS_H

/**
 * @def lh_gl_pixmap_fields(color_type, size_type)
 * @brief Borrowed pixels and the size they cover.
 *
 * @param color_type ::lh_gl_color_t. The member is `pixels`, a borrowed row of
 *                   `width * height` colors, row by row, tightly packed. Null
 *                   means a fill writes nothing. The pixmap does not own them.
 * @param size_type  ::lh_gl_size_t. The member is `size`, in pixels. The first
 *                   row starts at the origin.
 */
#define lh_gl_pixmap_fields(color_type, size_type)                                                 \
    color_type *pixels;                                                                           \
    size_type size

#endif /* LH_GL_PIXMAP_FIELDS_H */
