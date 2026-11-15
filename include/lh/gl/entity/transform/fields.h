/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_entity_transform_t.
 */

#ifndef LH_GL_ENTITY_TRANSFORM_FIELDS_H
#define LH_GL_ENTITY_TRANSFORM_FIELDS_H

/**
 * @def lh_gl_entity_transform_fields(point_type, bool_type)
 * @brief How far the children move, and whether they are cut to the
 *        entity's rect.
 *
 * @param point_type Type of the offset.
 * @param bool_type  Type of the clip flag.
 */
#define lh_gl_entity_transform_fields(point_type, bool_type)                                       \
    point_type offset;                                                                             \
    bool_type clip

#endif /* LH_GL_ENTITY_TRANSFORM_FIELDS_H */
