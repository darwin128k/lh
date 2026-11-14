/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_shadow_t.
 */

#ifndef LH_GL_SHADOW_FIELDS_H
#define LH_GL_SHADOW_FIELDS_H

/**
 * @def lh_gl_shadow_fields(color_type, scalar_type)
 * @brief Peak color, fade distance, and a shift.
 *
 * @param color_type  ::lh_gl_color_t. The member is `color`. Its alpha is the
 *                    strongest the shadow gets.
 * @param scalar_type ::lh_gl_scalar_t. `spread` is how far the fade reaches.
 *                    `offset_x` is right, `offset_y` is down.
 */
#define lh_gl_shadow_fields(color_type, scalar_type)                                               \
    color_type color;                                                                              \
    scalar_type spread;                                                                            \
    scalar_type offset_x;                                                                          \
    scalar_type offset_y

#endif /* LH_GL_SHADOW_FIELDS_H */
