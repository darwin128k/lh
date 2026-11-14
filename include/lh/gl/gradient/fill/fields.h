/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_gradient_fill_t.
 */

#ifndef LH_GL_GRADIENT_FILL_FIELDS_H
#define LH_GL_GRADIENT_FILL_FIELDS_H

/**
 * @def lh_gl_gradient_fill_fields(entity_type, gradient_type)
 * @brief The entity a gradient fill starts with, and the gradient it borrows.
 *
 * The `entity` member comes first, so a pointer to the fill is also a pointer
 * to its entity.
 *
 * @param entity_type   ::lh_gl_entity_t. Must be the first member.
 * @param gradient_type ::lh_gl_gradient_t. The member is `gradient`, borrowed.
 *                      Null draws nothing.
 */
#define lh_gl_gradient_fill_fields(entity_type, gradient_type)                                     \
    entity_type entity;                                                                            \
    const gradient_type *gradient

#endif /* LH_GL_GRADIENT_FILL_FIELDS_H */
