/**
 * @file fields.h
 * @brief Member fields ::lh_entity_3d_t adds to a 2D entity.
 */

#ifndef LH_ENTITY_3D_FIELDS_H
#define LH_ENTITY_3D_FIELDS_H

/**
 * @def lh_entity_3d_fields(scalar_type, quat_type)
 * @brief What a place in space has beyond a place in the plane: the depth
 *        `z`, a `rotation` out of the plane and the `scale_z`. Expanded after
 *        ::lh_entity_2d_fields.
 *
 * @param scalar_type Type of `z` and `scale_z` (::lh_float_t).
 * @param quat_type   Type of `rotation` (::lh_quat_t).
 */
#define lh_entity_3d_fields(scalar_type, quat_type)                                                \
    scalar_type z;                                                                                 \
    quat_type rotation;                                                                            \
    scalar_type scale_z

#endif /* LH_ENTITY_3D_FIELDS_H */
