/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_glass_t.
 */

#ifndef LH_GL_GLASS_FIELDS_H
#define LH_GL_GLASS_FIELDS_H

/**
 * @def lh_gl_glass_fields(entity_type, color_type, scalar_type, count_type, bool_type)
 * @brief A glass panel: the entity, a borrowed tint, and a borrowed scratch.
 *
 * @param entity_type ::lh_gl_entity_t. Must be the first member.
 * @param color_type  ::lh_gl_color_t. `tint` is borrowed and may be null, which
 *                    blurs and does not paint. `scratch` is the caller's buffer
 *                    for the blur and is not owned.
 * @param scalar_type ::lh_gl_scalar_t. `radius` rounds the panel. `blur` is how
 *                    far the average reaches. `0` paints the tint alone.
 * @param count_type  ::lh_usize_t. How many colours @p scratch holds.
 * @param bool_type   ::lh_bool_t. `shown` draws and can take a press. `target`
 *                    takes a press.
 */
#define lh_gl_glass_fields(entity_type, color_type, scalar_type, count_type, bool_type)            \
    entity_type entity;                                                                            \
    const color_type *tint;                                                                        \
    color_type *scratch;                                                                           \
    count_type scratch_count;                                                                      \
    scalar_type radius;                                                                            \
    scalar_type blur;                                                                              \
    bool_type shown;                                                                               \
    bool_type target

#endif /* LH_GL_GLASS_FIELDS_H */
