/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_shadow_fill_t.
 */

#ifndef LH_GL_SHADOW_FILL_FIELDS_H
#define LH_GL_SHADOW_FILL_FIELDS_H

/**
 * @def lh_gl_shadow_fill_fields(entity_type, shadow_type, rect_type, scalar_type)
 * @brief The paint bounds, the borrowed shadow, and the box that casts it.
 *
 * The `entity` member comes first. Its rectangle is the area the shadow can
 * reach, not the box. `box` is that box, in the same coordinates as the entity.
 * `radius` rounds the box. `0` leaves it square.
 *
 * @param entity_type ::lh_gl_entity_t. Must be the first member.
 * @param shadow_type ::lh_gl_shadow_t. The member is `shadow`, borrowed. Null
 *                    draws nothing.
 * @param rect_type   ::lh_gl_rect_t. The member is `box`.
 * @param scalar_type ::lh_gl_scalar_t. The member is `radius`.
 */
#define lh_gl_shadow_fill_fields(entity_type, shadow_type, rect_type, scalar_type)                 \
    entity_type entity;                                                                            \
    const shadow_type *shadow;                                                                     \
    rect_type box;                                                                                 \
    scalar_type radius

#endif /* LH_GL_SHADOW_FILL_FIELDS_H */
