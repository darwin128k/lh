/**
 * @file view.h
 * @brief A clip with a scroll offset. A long page sits inside it.
 *
 * The view cuts its children to its box. ::lh_entity_view_set_content names
 * the child that moves; the offset is that child's position, negated, so
 * the page slides under the clip the way a document does. The offset cannot
 * leave the room the content leaves: ::lh_entity_view_get_travel is how much
 * there is, and that is measured from the two sizes rather than asked for.
 *
 * A scrollbar is a separate entity, and ::lh_entity_view_set_scrollbar links
 * it. Once linked, the view measures its own content on every draw, keeps the
 * bar's ends on the travel, gives the bar its screenful as its page, and lets
 * the bar's mode say whether it is there at all. The bar's value is the
 * offset, so dragging it scrolls the view and nothing has to listen to
 * ::LH_ENTITY_EVENT_CLICKED.
 *
 * The view also takes a wheel over itself, along either side it has a bar for,
 * and stops it there so a wheel over a control inside scrolls the view rather
 * than being lost. A view with nothing to scroll along that side leaves the
 * wheel alone and the search goes on up.
 */

#ifndef LH_ENTITY_VIEW_H
#define LH_ENTITY_VIEW_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/scroll.h>
#include <lh/numeric/types.h>

/**
 * @def LH_ENTITY_VIEW_SCROLL_LIMIT
 * @brief How many scrollbars one view carries: a horizontal one and a
 *        vertical one.
 */
#define LH_ENTITY_VIEW_SCROLL_LIMIT 2

/**
 * @struct lh_entity_view
 * @brief A window onto a larger child.
 *
 * The bars are indexed by direction: index 0 is the horizontal one, and index
 * ::LH_ENTITY_VIEW_SCROLL_LIMIT - 1 the vertical one. A bar takes the slot of
 * the way it runs, which is its own axis read at the time it is bound.
 */
struct lh_entity_view
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    struct lh_entity_scroll *bars[LH_ENTITY_VIEW_SCROLL_LIMIT];
    lh_entity_t *content;
    lh_int_t x;
    lh_int_t y;
    /* What a wheel has turned that is not yet a whole pixel. The offset is a
       whole number of pixels and a touchpad reports fractions of one, so the
       remainder waits here instead of being rounded away every event. */
    lh_float_t carry_x;
    lh_float_t carry_y;
};
typedef struct lh_entity_view lh_entity_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_view_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_view_class;

/**
 * @brief The child @p self scrolls. Not owned beyond being a child.
 */
lh_void
lh_entity_view_set_content(lh_entity_view_t *self, lh_entity_t *content);

/**
 * @brief The offset of the content inside @p self, in pixels.
 *
 * @p axis is ::LH_ENTITY_RANGE_AXIS_HORIZONTAL or
 * ::LH_ENTITY_RANGE_AXIS_VERTICAL. ::LH_ENTITY_RANGE_AXIS_AUTO is the long
 * side of the box, which is the same side
 * ::lh_entity_view_get_travel takes.
 */
lh_int_t
lh_entity_view_get_offset(const lh_entity_view_t *self, lh_int_t axis);

/**
 * @brief Slide the content so @p x, @p y of it sits at the view's origin.
 *
 * Both are pulled into ::lh_entity_view_get_travel, so an offset past the end
 * of the content is the end of it and a negative one is the start. A linked
 * scrollbar follows, through its own value, so the bar and the page cannot
 * disagree. The travel is measured, so a view whose content has no size yet
 * holds the offset at 0.
 */
lh_void
lh_entity_view_set_offset(lh_entity_view_t *self, lh_int_t x, lh_int_t y);

/**
 * @brief How far the content can move along @p axis, in pixels.
 *
 * What is left of the content's size once the view's own is off it, and 0 when
 * that is nothing: no content, or a content no longer than the view. This is
 * the value a scrollbar's upper end is, so the view answers "is there anything
 * to scroll" without anyone measuring the content twice.
 *
 * @p axis is read as ::lh_entity_view_get_offset reads it.
 */
lh_int_t
lh_entity_view_get_travel(const lh_entity_view_t *self, lh_int_t axis);

/**
 * @brief Link @p scroll to @p self, so a drag scrolls the view.
 *
 * The bar takes the slot of the way it runs, replacing a bar already in that
 * slot, and it is told about the view in return through
 * ::lh_entity_scroll_set_view. The link runs both ways, so which of the two a
 * caller happens to hold does not matter. Neither is owned or copied.
 *
 * ::lh_null drops every bar @p self carries and tells each of them, so the
 * other half of the link does not outlive it.
 */
lh_void
lh_entity_view_set_scrollbar(lh_entity_view_t *self, lh_entity_scroll_t *scroll);

/**
 * @brief Put @p self's offset and its bars in agreement with the content.
 *
 * Each bar's ends become the travel along its own way, its page becomes
 * @p self's size that way, and ::lh_entity_scroll_update_mode decides whether
 * it is there. Then the offset is taken from the bars, not the other way
 * round: the value a bar carries is the offset, so a drag since the last draw
 * counts, and a caller that moved the offset has already put its number in
 * the bar. Whichever side was written last is the one that counts.
 *
 * The view calls this on every draw and the caller does not, because the
 * content can be resized by a flex layout or by anything else and there is no
 * setter to hang this off. It is cheap when nothing moved, since a bar that
 * keeps its ends and its page does not dirty itself. A view with no bar keeps
 * the offset it had, pulled into the travel that is left.
 */
lh_void
lh_entity_view_sync(lh_entity_view_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_VIEW_H */
