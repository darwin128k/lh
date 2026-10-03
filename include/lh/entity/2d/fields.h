/**
 * @file fields.h
 * @brief Member fields ::lh_entity_2d_t adds to an entity.
 */

#ifndef LH_ENTITY_2D_FIELDS_H
#define LH_ENTITY_2D_FIELDS_H

/**
 * @def lh_entity_2d_fields(vec2_type, angle_type)
 * @brief Where the entity is relative to its parent, in the plane:
 *        `position`, `angle`, `scale`. Expanded after ::lh_entity_fields.
 *
 * @param vec2_type  Type of `position` and `scale` (::lh_vec2_t).
 * @param angle_type Type of `angle`, radians about the z axis (::lh_float_t).
 */
#define lh_entity_2d_fields(vec2_type, angle_type)                                                 \
    vec2_type position;                                                                            \
    angle_type angle;                                                                              \
    vec2_type scale

#endif /* LH_ENTITY_2D_FIELDS_H */
