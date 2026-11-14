/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_gradient_t.
 */

#ifndef LH_GL_GRADIENT_FIELDS_H
#define LH_GL_GRADIENT_FIELDS_H

/**
 * @def lh_gl_gradient_fields(color_type)
 * @brief Two colors. `from` is the start of the axis, `to` is the end.
 *
 * @param color_type ::lh_gl_color_t.
 */
#define lh_gl_gradient_fields(color_type)                                                          \
    color_type from;                                                                               \
    color_type to

#endif /* LH_GL_GRADIENT_FIELDS_H */
