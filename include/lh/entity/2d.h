/**
 * @file 2d.h
 * @brief An entity with a place in the plane: position, angle and scale
 *        relative to its parent. The spatial core of lh's entities.
 *
 * 2D elements build on it (::lh_entity_rect_t adds a size), and so does 3D
 * (::lh_entity_3d_t adds depth, a tilt out of the plane and a z scale): a 2D
 * entity is a 3D one that stays in the `z = 0` plane. All of them compose
 * through 4x4 matrices, so one tree can mix them, e.g. a UI panel inside a
 * 3D scene.
 *
 * The world transform of an entity is its parent's world transform times its
 * own (::lh_entity_2d_get_world_matrix): moving a parent moves its children
 * with it. Ancestors that are not spatial (plain containers) count as no
 * transform at all.
 */

#ifndef LH_ENTITY_2D_H
#define LH_ENTITY_2D_H

#include <lh/compiler/extern/c.h>
#include <lh/entity.h>
#include <lh/entity/2d/fields.h>
#include <lh/float.h>
#include <lh/mat4.h>
#include <lh/vec2.h>

/**
 * @struct lh_entity_2d
 * @brief Fields via ::lh_entity_fields, then ::lh_entity_2d_fields.
 */
struct lh_entity_2d
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_uint_t);
    lh_entity_2d_fields(lh_vec2_t, lh_float_t);
};
typedef struct lh_entity_2d lh_entity_2d_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_2d_t, derived from ::lh_entity_base_class.
 *
 * A new instance sits at its parent's origin: position 0, angle 0, scale 1.
 */
extern const lh_entity_class_t lh_entity_2d_class;

/**
 * @brief Position relative to the parent.
 */
lh_vec2_t
lh_entity_2d_get_position(const lh_entity_2d_t *self);

/**
 * @brief Set the position relative to the parent.
 */
lh_void
lh_entity_2d_set_position(lh_entity_2d_t *self, lh_vec2_t position);

/**
 * @brief Rotation relative to the parent, in radians about the z axis.
 */
lh_float_t
lh_entity_2d_get_angle(const lh_entity_2d_t *self);

/**
 * @brief Set the rotation relative to the parent, in radians about the z
 *        axis (from +x toward +y).
 */
lh_void
lh_entity_2d_set_angle(lh_entity_2d_t *self, lh_float_t angle);

/**
 * @brief Scale along the local x and y axes.
 */
lh_vec2_t
lh_entity_2d_get_scale(const lh_entity_2d_t *self);

/**
 * @brief Set the scale along the local x and y axes.
 */
lh_void
lh_entity_2d_set_scale(lh_entity_2d_t *self, lh_vec2_t scale);

/**
 * @brief From @p self's own space into its parent's.
 *
 * For a plain 2D entity: scale, then rotate by the angle, then move. A class
 * derived from it with more to its place (::lh_entity_3d_t) answers with its
 * own matrix (::LH_ENTITY_EVENT_GET_LOCAL_MATRIX).
 */
lh_mat4_t
lh_entity_2d_get_local_matrix(const lh_entity_2d_t *self);

/**
 * @brief From @p self's own space into the world (the root's space): the
 *        local matrices of @p self and of every spatial entity above it.
 */
lh_mat4_t
lh_entity_2d_get_world_matrix(const lh_entity_2d_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_2D_H */
