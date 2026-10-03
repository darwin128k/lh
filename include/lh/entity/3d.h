/**
 * @file 3d.h
 * @brief A 2D entity extended into space: a model, a camera, a light.
 *
 * Derived from ::lh_entity_2d_t: the plane position and scale become the
 * `x, y` of a 3D position and scale, with `z` added here, and a quaternion
 * rotation that can turn it any way. The local matrix is
 * `T(position) * R(rotation) * Rz(angle) * S(scale)`; the 2D angle stays 0
 * unless set through the 2D functions, so 3D code just uses the functions
 * below.
 */

#ifndef LH_ENTITY_3D_H
#define LH_ENTITY_3D_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/3d/fields.h>
#include <lh/math/quat.h>
#include <lh/math/vec3.h>

/**
 * @struct lh_entity_3d
 * @brief Fields via ::lh_entity_fields, ::lh_entity_2d_fields, then
 *        ::lh_entity_3d_fields.
 */
struct lh_entity_3d
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t);
    lh_entity_3d_fields(lh_float_t, lh_math_quat_t);
};
typedef struct lh_entity_3d lh_entity_3d_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_3d_t, derived from ::lh_entity_2d_class.
 *
 * A new instance sits at its parent's origin: position 0, no rotation,
 * scale 1.
 */
extern const lh_entity_class_t lh_entity_3d_class;

/**
 * @brief Position relative to the parent.
 */
lh_math_vec3_t
lh_entity_3d_get_position(const lh_entity_3d_t *self);

/**
 * @brief Set the position relative to the parent.
 */
lh_void
lh_entity_3d_set_position(lh_entity_3d_t *self, lh_math_vec3_t position);

/**
 * @brief Rotation relative to the parent (applied after the 2D angle).
 */
lh_math_quat_t
lh_entity_3d_get_rotation(const lh_entity_3d_t *self);

/**
 * @brief Set the rotation relative to the parent (a unit quaternion).
 */
lh_void
lh_entity_3d_set_rotation(lh_entity_3d_t *self, lh_math_quat_t rotation);

/**
 * @brief Scale along each local axis.
 */
lh_math_vec3_t
lh_entity_3d_get_scale(const lh_entity_3d_t *self);

/**
 * @brief Set the scale along each local axis.
 */
lh_void
lh_entity_3d_set_scale(lh_entity_3d_t *self, lh_math_vec3_t scale);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_3D_H */
