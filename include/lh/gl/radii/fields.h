/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_radii_t.
 */

#ifndef LH_GL_RADII_FIELDS_H
#define LH_GL_RADII_FIELDS_H

/**
 * @def lh_gl_radii_fields(scalar_type)
 * @brief The radius of each corner, clockwise from the top left, as CSS lists them.
 *
 * @param scalar_type ::lh_gl_scalar_t.
 */
#define lh_gl_radii_fields(scalar_type)                                                            \
    scalar_type top_left;                                                                          \
    scalar_type top_right;                                                                         \
    scalar_type bottom_right;                                                                      \
    scalar_type bottom_left

#endif /* LH_GL_RADII_FIELDS_H */
