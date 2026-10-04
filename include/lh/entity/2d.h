/**
 * @file 2d.h
 * @brief An entity with a place, a box and a style: the object that is
 *        drawn (LVGL's `lv_obj`).
 *
 * Position, angle and scale are relative to the parent. The box is
 * `[0, width) x [0, height)` in the entity's own space, from its position
 * (the top-left corner, `y` growing downward). A style
 * (::lh_ui_style_t) is how that box is painted; the entity only points at
 * one. A new entity has size 0 and no style, so it only places its children
 * until given a size and a style.
 *
 * Children are positioned relative to the parent's top-left and are cut to
 * the parent's box when that box has a positive size, unless the parent has
 * ::lh_entity_flags_overflow_visible. An entity with no box does not cut: a
 * transform, or a 3D node, is not a clip.
 *
 * The world transform is the parent's world transform times this entity's
 * own (::lh_entity_2d_get_world_matrix). Ancestors that are not spatial
 * count as no transform. ::lh_entity_3d_t extends the same place into
 * space, so one tree can mix them.
 *
 * Changing the place, the size or which style it points at marks the covered
 * screen area for redrawing (::lh_entity_invalidate), as does deleting the
 * entity.
 */

#ifndef LH_ENTITY_2D_H
#define LH_ENTITY_2D_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity.h>
#include <lh/entity/2d/fields.h>
#include <lh/float.h>
#include <lh/math/mat4.h>
#include <lh/math/rect.h>
#include <lh/math/vec2.h>
#include <lh/ui/canvas.h>
#include <lh/ui/effect.h>
#include <lh/ui/style.h>

/**
 * @struct lh_entity_2d
 * @brief Fields via ::lh_entity_fields, then ::lh_entity_2d_fields.
 */
struct lh_entity_2d
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
};
typedef struct lh_entity_2d lh_entity_2d_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_2d_t, derived from ::lh_entity_base_class.
 *
 * A new instance sits at its parent's origin: position 0, angle 0, scale 1,
 * size 0, no style.
 */
extern const lh_entity_class_t lh_entity_2d_class;

/**
 * @brief Position relative to the parent.
 */
lh_math_vec2_t
lh_entity_2d_get_position(const lh_entity_2d_t *self);

/**
 * @brief Set the position relative to the parent.
 */
lh_void
lh_entity_2d_set_position(lh_entity_2d_t *self, lh_math_vec2_t position);

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
lh_math_vec2_t
lh_entity_2d_get_scale(const lh_entity_2d_t *self);

/**
 * @brief Set the scale along the local x and y axes.
 */
lh_void
lh_entity_2d_set_scale(lh_entity_2d_t *self, lh_math_vec2_t scale);

/**
 * @brief Width (`x`) and height (`y`) of the box in the entity's own space.
 */
lh_math_vec2_t
lh_entity_2d_get_size(const lh_entity_2d_t *self);

/**
 * @brief Set the width (`x`) and height (`y`) of the box. Zero or less is
 *        no box.
 */
lh_void
lh_entity_2d_set_size(lh_entity_2d_t *self, lh_math_vec2_t size);

/**
 * @brief The style @p self is painted with, or ::lh_null when it has none.
 */
const lh_ui_style_t *
lh_entity_2d_get_style(const lh_entity_2d_t *self);

/**
 * @brief Point @p self at @p style (::lh_null paints nothing).
 *
 * @p style is not copied and not owned: it must outlive the entities that
 * point at it. The colors stay in the style.
 */
lh_void
lh_entity_2d_set_style(lh_entity_2d_t *self, const lh_ui_style_t *style);

/**
 * @brief The effect drawn before @p self's fill, or ::lh_null.
 */
const lh_ui_effect_t *
lh_entity_2d_get_effect(const lh_entity_2d_t *self);

/**
 * @brief Point @p self at @p effect (::lh_null draws none).
 *
 * @p effect is not copied and not owned. It is painted before the fill, so
 * a shadow sits under the box and glass blurs what is already there.
 */
lh_void
lh_entity_2d_set_effect(lh_entity_2d_t *self, const lh_ui_effect_t *effect);

/**
 * @brief From @p self's own space into its parent's.
 *
 * Scale, then rotate by the angle, then move. A class derived from it with
 * more to its place (::lh_entity_3d_t) answers with its own matrix
 * (::LH_ENTITY_EVENT_GET_LOCAL_MATRIX).
 */
lh_math_mat4_t
lh_entity_2d_get_local_matrix(const lh_entity_2d_t *self);

/**
 * @brief From @p self's own space into the world (the root's space): the
 *        local matrices of @p self and of every spatial entity above it.
 */
lh_math_mat4_t
lh_entity_2d_get_world_matrix(const lh_entity_2d_t *self);

/**
 * @brief The whole pixels the box touches on screen: the bounding box of
 *        its corners, rounded outward. Empty when there is no box. Depth
 *        is ignored (seen straight along z).
 *
 * The result is an integer rectangle (::lh_math_coord_t). A caller that
 * wants one side takes it from this value.
 */
lh_math_rect_t
lh_entity_2d_get_screen_bounds(const lh_entity_2d_t *self);

/**
 * @brief True when the view ray through @p point meets @p self's box.
 *
 * The view is orthographic and looks along -z, so the ray is the screen
 * point at every depth. It is tested where it crosses the entity's local
 * `z = 0` plane, against `[0, width) x [0, height)`. A box edge-on to the
 * view contains nothing, and an entity with no box contains nothing.
 */
lh_bool_t
lh_entity_2d_contains(const lh_entity_2d_t *self, lh_math_vec2_t point);

/**
 * @brief The entity on top at the world point @p point within the tree of
 *        @p root (including @p root), or ::lh_null if there is none.
 *
 * The closer surface wins: a larger world z is nearer the view. At equal
 * depth the draw order wins, so a child is over its parent and a younger
 * sibling is over an older one. Hidden entities, and children cut away by a
 * parent's box, are not hit. An entity with no box is searched through (its
 * children can still be hit) but never returned.
 */
lh_entity_t *
lh_entity_2d_find_at(lh_entity_t *root, lh_math_vec2_t point);

/**
 * @brief Paint @p self's box into @p canvas with its background style.
 *
 * Nothing when @p self has no style, the style background is transparent,
 * or there is no box. Stays inside the canvas clip. A box parallel to the
 * screen and axis-aligned is one fill at a constant depth. Any other box
 * tests each pixel center of its bounds against the local `z = 0` plane and
 * writes that pixel at the depth of the hit. A larger depth is closer.
 *
 * The screen calls this before ::LH_ENTITY_EVENT_DRAW, so whatever the
 * entity paints in that event lies on top of the background.
 */
lh_void
lh_entity_2d_draw_background(const lh_entity_2d_t *self, lh_ui_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_2D_H */
