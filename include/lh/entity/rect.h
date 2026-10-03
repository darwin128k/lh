/**
 * @file rect.h
 * @brief A rectangular entity: the 2D element (panel, button, label, ...).
 *
 * A 2D entity with a size and a fill color. In its own space the rectangle
 * spans `[0, width) x [0, height)` in the `z = 0` plane, from its position
 * (the top-left corner, with `y` growing downward like screen coordinates).
 * It moves, rotates and scales with its 2D transform; inside a 3D parent it
 * is placed in the scene like any other child.
 *
 * Children are positioned relative to the parent's top-left corner and are
 * cut to the parent when drawn and hit, unless it has
 * ::lh_entity_flags_overflow_visible. The cut follows the parent's screen
 * bounds (::lh_entity_rect_get_screen_bounds): exact for an unrotated
 * parent, its bounding box for a rotated one.
 *
 * Changing the size or color marks the covered screen area for redrawing
 * (::lh_entity_invalidate), as do the 2D setters and deleting the entity.
 */

#ifndef LH_ENTITY_RECT_H
#define LH_ENTITY_RECT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/rect/fields.h>
#include <lh/math.h>
#include <lh/ui/color.h>
#include <lh/vec2.h>

/**
 * @struct lh_entity_rect
 * @brief Fields via ::lh_entity_fields, ::lh_entity_2d_fields, then
 *        ::lh_entity_rect_fields.
 */
struct lh_entity_rect
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_vec2_t, lh_float_t);
    lh_entity_rect_fields(lh_vec2_t, lh_ui_color_t);
};
typedef struct lh_entity_rect lh_entity_rect_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_rect_t, derived from ::lh_entity_2d_class.
 *
 * A new instance has size 0 (covers nothing) and a fully transparent color
 * (draws nothing): a rectangle only groups and cuts its children until given
 * a color.
 */
extern const lh_entity_class_t lh_entity_rect_class;

/**
 * @brief Width (`x`) and height (`y`) in the rectangle's own space.
 */
lh_vec2_t
lh_entity_rect_get_size(const lh_entity_rect_t *self);

/**
 * @brief Set the width (`x`) and height (`y`) in the rectangle's own space.
 */
lh_void
lh_entity_rect_set_size(lh_entity_rect_t *self, lh_vec2_t size);

/**
 * @brief The color the rectangle is filled with.
 */
lh_ui_color_t
lh_entity_rect_get_color(const lh_entity_rect_t *self);

/**
 * @brief Set the color the rectangle is filled with (alpha 0: not filled).
 */
lh_void
lh_entity_rect_set_color(lh_entity_rect_t *self, lh_ui_color_t color);

/**
 * @brief The whole pixels the rectangle touches on screen: the bounding box
 *        of its corners, rounded outward. Depth is ignored (seen straight
 *        along z).
 */
lh_math_rect_t
lh_entity_rect_get_screen_bounds(const lh_entity_rect_t *self);

/**
 * @brief True when the world point @p point (`z = 0`, e.g. a mouse position)
 *        falls inside @p self.
 *
 * The point is taken into the rectangle's own space and tested against
 * `[0, width) x [0, height)`; its depth there is ignored, so a rectangle
 * seen at an angle is tested by its projection along the local z axis.
 */
lh_bool_t
lh_entity_rect_contains(const lh_entity_rect_t *self, lh_vec2_t point);

/**
 * @brief The rectangle on top at the world point @p point within the tree of
 *        @p root (including @p root), or ::lh_null if there is none.
 *
 * "On top" means what is drawn last: a child over its parent, a younger
 * sibling over an older one. Hidden entities and children cut away by their
 * parent are not hit. Entities that are not rectangles are searched through
 * (their children can still be hit) but never returned.
 */
lh_entity_t *
lh_entity_rect_find_at(lh_entity_t *root, lh_vec2_t point);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_RECT_H */
