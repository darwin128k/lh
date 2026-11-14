/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_fill_t.
 */

#ifndef LH_GL_FILL_FIELDS_H
#define LH_GL_FILL_FIELDS_H

/**
 * @def lh_gl_fill_fields(entity_type, color_type)
 * @brief The entity a fill starts with, and the color it borrows.
 *
 * The `entity` member comes first, so a pointer to a fill is also a pointer to
 * its entity, and a fill can be put into a layer.
 *
 * @param entity_type ::lh_gl_entity_t. Must be the first member.
 * @param color_type  ::lh_gl_color_t. The member is `color`, borrowed. Null
 *                    draws nothing. The fill does not own it.
 */
#define lh_gl_fill_fields(entity_type, color_type)                                                 \
    entity_type entity;                                                                            \
    const color_type *color

#endif /* LH_GL_FILL_FIELDS_H */
