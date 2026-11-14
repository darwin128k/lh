/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_pen_t.
 */

#ifndef LH_GL_PEN_FIELDS_H
#define LH_GL_PEN_FIELDS_H

/**
 * @def lh_gl_pen_fields(color_type, width_type)
 * @brief The stroke color and its thickness.
 *
 * @param color_type ::lh_gl_color_t: `color`, copied by value.
 * @param width_type ::lh_math_scalar_t: `width`. Zero draws nothing extra
 *                   around the shape. Negative is stored as given; a stroke
 *                   treats it as zero.
 */
#define lh_gl_pen_fields(color_type, width_type)                                                   \
    color_type color;                                                                              \
    width_type width

#endif /* LH_GL_PEN_FIELDS_H */
