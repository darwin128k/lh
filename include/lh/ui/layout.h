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
 *    to fill, by weight; what is still left goes where ::lh_ui_justify_t says.
 *    A child's margin is room it takes on both sides of its own length, and its
 *    minimum and maximum hold whatever length it was given.
 * 3. **place** — each child is given an absolute rect at its own place across
 *    the flow (::lh_ui_place_align_t). Hidden children take no room and are left
 *    where they are, so nothing that follows moves because of something nobody
 *    can see.
 *
 * A flow that **wraps** (::lh_ui_layout_set_wrap) breaks its children into lines
 * first -- a line ends before the child that would run past the content box -- and
 * then does the three steps above for each line on its own, in a band across the
 * flow as deep as the deepest child in it. What is left across the bands is shared
 * between them equally, which is CSS's `align-content: stretch`, so a line of
 * children that stretch across fills its share.
 *
 * The pass starts at the parent's content box (its rect inside the style padding,
 * ::lh_ui_insets_shrink), and rects it writes are absolute — the same space as
 * the parent's — so a pass has to run again whenever the parent moves or a child
 * changes size. That is the whole reason it is a value the app owns and calls
 * (::lh_ui_layout_apply), rather than something hidden inside a draw.
 */

#ifndef LH_UI_LAYOUT_H
#define LH_UI_LAYOUT_H

#include <lh/ui/entity.h>
#include <lh/ui/layout/fields.h>
#include <lh/ui/justify.h>
#include <lh/ui/layout/cross.h>
#include <lh/ui/layout/run.h>

/**
 * @struct lh_ui_layout
 * @brief A row or column rule, with a place for what is left over.
 */
struct lh_ui_layout
{
    /** Members: ::lh_ui_layout_fields. */
    lh_ui_layout_fields(lh_ui_axis_t, lh_ui_scalar_t, lh_ui_justify_t, lh_bool_t);
};
/** @copybrief lh_ui_layout */
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
 * @brief Break the children of the flow into lines that fit (@p wrap), @p line_gap
 *        apart -- CSS's `flex-wrap: wrap` with its `row-gap` for a row -- or keep
 *        them on one line (the default).
 */
lh_void
lh_ui_layout_set_wrap(lh_ui_layout_t *self, lh_bool_t wrap, lh_ui_scalar_t line_gap);

/**
 * @brief True when the flow breaks its children into lines.
 */
lh_bool_t
lh_ui_layout_get_wrap(const lh_ui_layout_t *self);

/**
 * @brief The room between two lines of a flow that wraps.
 */
lh_ui_scalar_t
lh_ui_layout_get_line_gap(const lh_ui_layout_t *self);

/** How deep @p child is across the flow, margins and all: its own size, or -- for one
   that stretches across -- only its minimum, since the line it is in decides the rest. */
lh_ui_scalar_t
lh_ui_layout_depth(const lh_ui_layout_t *self, lh_ui_entity_t *child);

/** The child after the last one that fits on the line starting at @p first in @p room
   along the flow (::lh_null when every one does), and in @p depth how deep the line is.
   A line always takes its first child, so a child longer than the room is a line on
   its own rather than none. */
lh_ui_entity_t *
lh_ui_layout_line_end(const lh_ui_layout_t *self, lh_ui_entity_t *parent, lh_ui_entity_t *first,
                      lh_ui_scalar_t room, lh_ui_scalar_t *depth);

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

/** What one child wants along the flow, before anything is shared out: its own
   length, or its content, or nothing at all for a child that wants to fill —
   that one is decided once every other child has said. A `wrap` child with no
   content asks for nothing, and that is what makes an empty label collapse.

   Held between the child's minimum and maximum: a fixed length or a content that
   is out of bounds is what the bounds are for. A fill is held when its share is
   known, in the second pass. */
lh_ui_scalar_t
lh_ui_layout_wants(const lh_ui_layout_t *self, lh_ui_entity_t *child);

/** How much of the fill a filling child weighs. Its length is read by nothing when
   it fills, so that is where the weight lives, and a fill that says nothing weighs
   one: every fill that was written before there were weights takes an equal share,
   as it always did. */
lh_ui_scalar_t
lh_ui_layout_weight(const lh_ui_place_t *place);

/** The margin a child keeps on either side of it along the flow, together. */
lh_ui_scalar_t
lh_ui_layout_margin_along(const lh_ui_layout_t *self, const lh_ui_place_t *place);

/** The room a child is placed in across the flow: the content box, less the margin
   the child keeps on either side of it across. */
lh_void
lh_ui_layout_slot(const lh_ui_layout_t *self, const lh_ui_place_t *place,
                  const lh_ui_rect_t *content, lh_ui_rect_t *out);

/** A child across the flow keeps the size it has and is put where it asked; only
   `fill` takes the whole cross side of the room it is placed in, held between its
   minimum and maximum.

   @p line is where the row's baseline sits, measured down from the cross origin
   of the content box, or -1 when the row has none. It is measured before this pass
   because a child's own place depends on the line and the line is the deepest of
   the children's: measured the other way round each child would be setting a line
   and sitting on it at once. */
lh_void
lh_ui_layout_across(const lh_ui_layout_t *self, lh_ui_entity_t *child, const lh_ui_rect_t *content,
                    lh_ui_scalar_t line, lh_ui_scalar_t *start, lh_ui_scalar_t *length);

/**
 * @brief The baseline row @p child would set for its line, down from the cross
 *        origin of @p content: its centred place plus its own baseline; -1 when
 *        the flow is vertical, the child is not baseline-aligned or has none.
 */
lh_ui_scalar_t
lh_ui_layout_child_line(const lh_ui_layout_t *self, lh_ui_entity_t *child,
                        const lh_ui_rect_t *content);

/**
 * @brief Lead before the first child and @p spread after each one for
 *        @p justify, with @p left room over @p shown children (`shown > 0`).
 *
 * Neither out may be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_layout_justify_lead(lh_ui_justify_t justify, lh_ui_scalar_t left, int shown,
                          lh_ui_scalar_t *lead, lh_ui_scalar_t *spread);

/**
 * @brief One line of the flow: the children @p first up to @p end (::lh_null
 *        for the last), measured, justified and placed in @p content_box. A flow
 *        that does not wrap is this once over every child in the content box.
 */
lh_void
lh_ui_layout_run(const lh_ui_layout_t *self, lh_ui_entity_t *parent, lh_ui_entity_t *first,
                 lh_ui_entity_t *end, const lh_ui_rect_t *content_box);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_H */