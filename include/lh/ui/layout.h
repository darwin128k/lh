/**
 * @file layout.h
 * @brief Where the children of a parent sit: ::lh_ui_layout_t.
 *
 * This is the general flow, and it replaces `lh_ui_layout_stack_t`: that one was
 * this one with every child told to fill the cross axis, and two names for one
 * thing is two things to keep in step.
 *
 * A pass is three steps, and nothing here knows what a child is:
 * 1. **measure** — each shown child is asked how long it wants to be along the
 *    flow (::lh_ui_place_t): its own length, its share of what is left, or its
 *    content, which is empty for a child with none.
 * 2. **distribute** — what is left after the gaps goes to the children that asked
 *    to fill; what is still left goes where ::lh_ui_justify_t says.
 * 3. **place** — each child is given an absolute rect at its own place across
 *    the flow (::lh_ui_place_align_t). Hidden children take no room and are left
 *    where they are, so nothing that follows moves because of something nobody
 *    can see.
 *
 * The pass starts at the parent's content box (its rect inside the style padding,
 * ::lh_ui_insets_shrink), and rects it writes are absolute — the same space as
 * the parent's — so a pass has to run again whenever the parent moves or a child
 * changes size. That is the whole reason it is a value the app owns and calls
 * (::lh_ui_layout_apply), rather than something hidden inside a draw.
 */

#ifndef LH_UI_LAYOUT_H
#define LH_UI_LAYOUT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/axis.h>
#include <lh/ui/entity.h>
#include <lh/ui/layout/fields.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @typedef lh_ui_justify_t
 * @brief Where the leftover room goes along the flow.
 *
 * Only asked when no child took it: a child that fills (::lh_ui_place_size_fill)
 * gets what is left, and this is what centres or ends the row when none does —
 * a button with a picture and no text, for instance.
 */
typedef enum lh_ui_justify
{
    lh_ui_justify_start = 0,
    lh_ui_justify_center,
    lh_ui_justify_end
} lh_ui_justify_t;

/**
 * @struct lh_ui_layout
 * @typedef lh_ui_layout_t
 * @brief A row or column rule, with a place for what is left over.
 */
struct lh_ui_layout
{
    lh_ui_layout_fields(lh_ui_axis_t, lh_ui_scalar_t, lh_ui_justify_t);
};
typedef struct lh_ui_layout lh_ui_layout_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Children along @p axis, @p gap apart (not negative), leftovers at the
 *        start.
 */
lh_void
lh_ui_layout_init(lh_ui_layout_t *self, lh_ui_axis_t axis, lh_ui_scalar_t gap);

/**
 * @brief The axis children follow.
 */
lh_ui_axis_t
lh_ui_layout_get_axis(const lh_ui_layout_t *self);

/**
 * @brief The room between two neighbours.
 */
lh_ui_scalar_t
lh_ui_layout_get_gap(const lh_ui_layout_t *self);

/**
 * @brief Where the leftover room goes.
 */
lh_ui_justify_t
lh_ui_layout_get_justify(const lh_ui_layout_t *self);

/**
 * @brief Put the leftover room at the start, in the middle or at the end.
 */
lh_void
lh_ui_layout_set_justify(lh_ui_layout_t *self, lh_ui_justify_t justify);

/**
 * @brief Place every shown child of @p parent by @p self, from the top-left of
 *        its content box (rect inside the style padding).
 *
 * Call after the parent moves or a child changes size: the pass writes absolute
 * rects, so a parent that moved leaves its children where it used to be until it
 * is run again.
 */
lh_void
lh_ui_layout_apply(const lh_ui_layout_t *self, lh_ui_entity_t *parent);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_H */