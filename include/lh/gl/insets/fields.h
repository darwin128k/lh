/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_insets_t.
 */

#ifndef LH_GL_INSETS_FIELDS_H
#define LH_GL_INSETS_FIELDS_H

/**
 * @def lh_gl_insets_fields(scalar_type)
 * @brief Distance in from each side of a rect.
 *
 * @param scalar_type ::lh_gl_scalar_t.
 */
#define lh_gl_insets_fields(scalar_type)                                                           \
    scalar_type left;                                                                              \
    scalar_type top;                                                                               \
    scalar_type right;                                                                             \
    scalar_type bottom

#endif /* LH_GL_INSETS_FIELDS_H */
