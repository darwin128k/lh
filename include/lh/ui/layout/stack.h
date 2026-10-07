/**
 * @file stack.h
 * @brief Children one after another along an axis: ::lh_ui_layout_stack_t.
 *
 * A small add-on, not an entity: it holds the rule (axis, gap, stretch) and
 * ::lh_ui_layout_stack_apply places the shown children of any parent by it.
 * Placement starts at the content box (the parent rect inside its style
 * padding, ::lh_ui_insets_shrink); each child keeps its length along the
 * axis, the gap goes between neighbours, hidden children take no room. With
 * `stretch`, a child takes the whole cross size of the content box. A moved
 * child takes its subtree along (rects are absolute, ::lh_ui_entity_move_to).
 *
 * Call apply after the children or their sizes change; a container's content
 * size then follows (it adds the end padding after the last child).
 */

#ifndef LH_UI_LAYOUT_STACK_H
#define LH_UI_LAYOUT_STACK_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/axis.h>
#include <lh/ui/entity.h>
#include <lh/ui/layout/stack/fields.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_ui_layout_stack
 * @typedef lh_ui_layout_stack_t
 * @brief A row or column rule.
 */
struct lh_ui_layout_stack
{
    lh_ui_layout_stack_fields(lh_ui_axis_t, lh_ui_scalar_t, lh_bool_t);
};
typedef struct lh_ui_layout_stack lh_ui_layout_stack_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Children along @p axis, @p gap apart (not negative), no stretch.
 */
lh_void
lh_ui_layout_stack_init(lh_ui_layout_stack_t *self, lh_ui_axis_t axis, lh_ui_scalar_t gap);

/**
 * @brief The axis children follow.
 */
lh_ui_axis_t
lh_ui_layout_stack_get_axis(const lh_ui_layout_stack_t *self);

/**
 * @brief The room between two neighbours.
 */
lh_ui_scalar_t
lh_ui_layout_stack_get_gap(const lh_ui_layout_stack_t *self);

/**
 * @brief True when children take the whole cross size of the content box.
 */
lh_bool_t
lh_ui_layout_stack_is_stretch(const lh_ui_layout_stack_t *self);

/**
 * @brief Make children take the whole cross size of the content box, or keep
 *        their own.
 */
lh_void
lh_ui_layout_stack_set_stretch(lh_ui_layout_stack_t *self, lh_bool_t stretch);

/**
 * @brief Place @p child at @p cursor (its cross size from @p content when
 *        stretching) and return where the next one starts. A hidden child is
 *        left where it is and takes no room.
 */
lh_ui_point_t
lh_ui_layout_stack_place(const lh_ui_layout_stack_t *self, lh_ui_entity_t *child, lh_ui_point_t cursor,
                         const lh_ui_rect_t *content);

/**
 * @brief Place every child of @p parent by @p self, from the top-left of its
 *        content box (rect inside the style padding).
 */
lh_void
lh_ui_layout_stack_apply(const lh_ui_layout_stack_t *self, lh_ui_entity_t *parent);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_STACK_H */
