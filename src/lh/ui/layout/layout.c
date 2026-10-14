/**
 * @file layout.c
 * @brief Implementation of `lh/ui/layout.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/insets.h>
#include <lh/ui/layout.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_layout_init(lh_ui_layout_t *self, lh_ui_axis_t axis, lh_ui_scalar_t gap)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(gap < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->axis = axis;
    self->gap = gap;
    self->justify = lh_ui_justify_start;
    self->wrap = lh_bool_false;
    self->line_gap = lh_ui_scalar(0);
}

lh_void
lh_ui_layout_set_wrap(lh_ui_layout_t *self, lh_bool_t wrap, lh_ui_scalar_t line_gap)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(line_gap < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->wrap = wrap;
    self->line_gap = line_gap;
}

lh_bool_t
lh_ui_layout_get_wrap(const lh_ui_layout_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wrap;
}

lh_ui_scalar_t
lh_ui_layout_get_line_gap(const lh_ui_layout_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line_gap;
}

lh_ui_axis_t
lh_ui_layout_get_axis(const lh_ui_layout_t *self)
{
    lh_assert_runtime_ref(self);
    return self->axis;
}

lh_ui_scalar_t
lh_ui_layout_get_gap(const lh_ui_layout_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gap;
}

lh_ui_justify_t
lh_ui_layout_get_justify(const lh_ui_layout_t *self)
{
    lh_assert_runtime_ref(self);
    return self->justify;
}

lh_void
lh_ui_layout_set_justify(lh_ui_layout_t *self, lh_ui_justify_t justify)
{
    lh_assert_runtime_ref(self);
    self->justify = justify;
}

/* ── The pass ────────────────────────────────────────────────────────────── */

/** What one child wants along the flow, before anything is shared out: its own
   length, or its content, or nothing at all for a child that wants to fill —
   that one is decided once every other child has said. A `wrap` child with no
   content asks for nothing, and that is what makes an empty label collapse.

   Held between the child's minimum and maximum: a fixed length or a content that
   is out of bounds is what the bounds are for. A fill is held when its share is
   known, in the second pass. */
static lh_ui_scalar_t
layout_wants(const lh_ui_layout_t *self, lh_ui_entity_t *child)
{
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    lh_ui_rect_t content;

    if (lh_ui_place_get_size_mode(place) == lh_ui_place_size_fill)
    {
        return lh_ui_scalar(0);
    }
    if (lh_ui_place_get_size_mode(place) == lh_ui_place_size_fixed)
    {
        return lh_ui_place_clamp(place, self->axis, lh_ui_place_get_size(place));
    }
    content = lh_ui_entity_get_content_bounds(child);
    return lh_ui_place_clamp(place, self->axis,
                             lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(content)), self->axis));
}

/** How much of the fill a filling child weighs. Its length is read by nothing when
   it fills, so that is where the weight lives, and a fill that says nothing weighs
   one: every fill that was written before there were weights takes an equal share,
   as it always did. */
static lh_ui_scalar_t
layout_weight(const lh_ui_place_t *place)
{
    const lh_ui_scalar_t weight = lh_ui_place_get_size(place);

    return weight > lh_ui_scalar(0) ? weight : lh_ui_scalar(1);
}

/** The margin a child keeps on either side of it along the flow, together. */
static lh_ui_scalar_t
layout_margin_along(const lh_ui_layout_t *self, const lh_ui_place_t *place)
{
    const lh_ui_insets_t *margin = lh_ui_place_get_margin(place);

    return lh_ui_insets_get_start(margin, self->axis) + lh_ui_insets_get_end(margin, self->axis);
}

/** The room a child is placed in across the flow: the content box, less the margin
   the child keeps on either side of it across. */
static lh_ui_rect_t
layout_slot(const lh_ui_layout_t *self, const lh_ui_place_t *place, const lh_ui_rect_t *content)
{
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    const lh_ui_insets_t *margin = lh_ui_place_get_margin(place);
    const lh_ui_scalar_t before = lh_ui_insets_get_start(margin, cross);
    const lh_ui_scalar_t after = lh_ui_insets_get_end(margin, cross);
    const lh_ui_scalar_t room = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(content), cross);
    lh_ui_rect_t slot;

    lh_ui_rect_init_along(lh_addr_of(slot), content, cross, before,
                          lh_math_max(room - before - after, lh_ui_scalar(0)));
    return slot;
}

/** Where a child of @p own rows sits in @p room when it is centred across the flow.

   Rounded half up, like ::lh_ui_text_align_get_origin and for the same reason: an
   odd child in even room cannot land on the middle, and truncating hands the
   leftover half-pixel to the top. Measured on the demo's Hide panel button — room 16
   rows, a 15-row caption and a 9-row glyph — that put both on 193.5 inside a box
   centred on 194.0, half a pixel high and visible as it is one. The leftover
   half-pixel is the one below.

   The row's line is measured with this and so is every child's place, so the two
   cannot drift apart: a child measured here and placed by a different formula would
   move by a row for no reason anyone could see. */
static lh_ui_scalar_t
layout_centred(lh_ui_scalar_t room, lh_ui_scalar_t own)
{
    return lh_math_max((room - own + lh_ui_scalar(1)) / lh_ui_scalar(2), lh_ui_scalar(0));
}

/** A child across the flow keeps the size it has and is put where it asked; only
   `fill` takes the whole cross side of the room it is placed in, held between its
   minimum and maximum.

   @p line is where the row's baseline sits, measured down from the cross origin
   of the content box, or -1 when the row has none. It is measured before this pass
   because a child's own place depends on the line and the line is the deepest of
   the children's: measured the other way round each child would be setting a line
   and sitting on it at once. */
static lh_void
layout_across(const lh_ui_layout_t *self, lh_ui_entity_t *child, const lh_ui_rect_t *content,
              lh_ui_scalar_t line, lh_ui_scalar_t *start, lh_ui_scalar_t *length)
{
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    const lh_ui_rect_t rect = lh_ui_entity_get_rect(child);
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    const lh_ui_place_align_t align = lh_ui_place_get_align(place);
    const lh_ui_rect_t slot = layout_slot(self, place, content);
    const lh_ui_scalar_t own = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(rect)), cross);
    const lh_ui_scalar_t room = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(slot)), cross);
    /* The line is measured from the content box; the slot starts the margin further
       in, so the line is that much nearer to it. */
    const lh_ui_scalar_t before = lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(slot)), cross) -
                                  lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(content), cross);
    lh_ui_scalar_t at = lh_ui_scalar(0);

    if (align == lh_ui_place_align_fill)
    {
        *length = lh_ui_place_clamp(place, cross, room);
    }
    else
    {
        *length = own;
        if (align == lh_ui_place_align_center)
        {
            at = layout_centred(room, own);
        }
        else if (align == lh_ui_place_align_end)
        {
            at = room - own;
        }
        else if (align == lh_ui_place_align_baseline)
        {
            const lh_ui_scalar_t base = lh_ui_entity_get_baseline(child);

            /* On the row's line rather than in the middle of the room: an icon next
               to a caption is one line of text, and two things centred in the same
               16 rows are on one line only by accident.

               A child that has no line of its own is **not** on one, and -1 is not a
               line: subtracting it would put the child a row *below* the row's line,
               which is where the measurement put it and is the one row in this whole
               arrangement nobody could explain. So the answer for a child with no
               baseline, and for everybody in a vertical flow where there is no cross
               row to stand on, is the centring above. */
            at = (base >= lh_ui_scalar(0) && line >= lh_ui_scalar(0)) ? line - before - base
                                                                       : layout_centred(room, own);
        }
    }
    *start = lh_math_max(
        lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(slot)), cross) +
            lh_math_max(at, lh_ui_scalar(0)),
        lh_ui_scalar(0));
}

/** One line of the flow: the children from @p first up to @p end (::lh_null for the
   last child), measured, shared out and placed in @p content_box. A flow that does not
   wrap is this once, over every child, in the parent's content box. */
static lh_void
layout_run(const lh_ui_layout_t *self, lh_ui_entity_t *parent, lh_ui_entity_t *first, lh_ui_entity_t *end,
           const lh_ui_rect_t *content_box)
{
    const lh_ui_rect_t content = *content_box;
    /* ::lh_ui_rect_init_along measures `start` past the origin of its base, and
       the rects a child carries are absolute, so the slice is cut out of nothing
       and the two calls hand the child its own place. */
    const lh_ui_rect_t nowhere = {0, 0, 0, 0};
    lh_ui_scalar_t along = lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(content)),
                                                 self->axis);
    lh_ui_scalar_t used = lh_ui_scalar(0);
    lh_ui_scalar_t weights = lh_ui_scalar(0);
    lh_ui_scalar_t left = lh_ui_scalar(0);
    lh_ui_scalar_t spread = lh_ui_scalar(0);
    lh_ui_scalar_t line = lh_ui_scalar(-1);
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    int shown = 0;
    lh_ui_entity_t *child;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(parent);

    /* Measure: what everyone asked for, and what is left of the content box. The
       gap is counted between children that take room only, so a collapsed one
       leaves neither space nor a hole in the row. */
    for (child = first; child != end; child = lh_ui_entity_get_next_child(parent, child))
    {
        const lh_ui_place_t *place = lh_ui_entity_get_place(child);
        const lh_bool_t fills_too = lh_ui_place_get_size_mode(place) == lh_ui_place_size_fill;
        const lh_ui_scalar_t margin = layout_margin_along(self, place);
        lh_ui_scalar_t wants;

        if (lh_ui_entity_is_hidden(child))
        {
            continue;
        }
        if (fills_too)
        {
            weights += layout_weight(place);
        }
        wants = layout_wants(self, child);
        /* A child that asked to fill has room to take even before anyone has said
           how much, so it counts for a gap; one that asked for nothing does not,
           and neither does one that is not there to be seen. A margin is room too. */
        if (wants > lh_ui_scalar(0) || fills_too || margin > lh_ui_scalar(0))
        {
            used += wants + margin;
            ++shown;
        }
        /* The line the row stands on, down from the cross origin of the content.
           It is the **deepest** of the baselines the children would have had on
           their own, and each of those is a place centring would have chosen, so
           what this says in one line is that nobody is ever pulled below the
           middle of the room it is in: the row only ever lifts a child up to meet
           a deeper one.

           The deepest and not the first, which is the rule CSS flex uses, because
           the first is whichever child the app happened to add first. An app puts
           an icon on the left of a caption, so the icon is the first child, and the
           text was the thing that moved: measured on the demo's Hide panel button,
           the 'x' sets the line at row 199 and the caption rides up from 188..199 to
           187..198. An icon is not the line — it has a box, and the text is what has
           a line to be on.

           A row of one caption is exactly the row it was before anyone asked: its
           own baseline is the deepest there is. A **vertical** flow has no cross
           row to line up and never asks. */
        if (cross == lh_ui_axis_vertical && lh_ui_place_get_align(place) == lh_ui_place_align_baseline)
        {
            const lh_ui_scalar_t at = lh_ui_entity_get_baseline(child);
            const lh_ui_rect_t own_rect = lh_ui_entity_get_rect(child);
            const lh_ui_rect_t slot = layout_slot(self, place, lh_addr_of(content));
            const lh_ui_scalar_t own =
                lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(own_rect)), cross);
            const lh_ui_scalar_t room = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(slot)), cross);
            const lh_ui_scalar_t before =
                lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(slot)), cross) -
                lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(content)), cross);

            if (at >= lh_ui_scalar(0))
            {
                line = lh_math_max(line, before + layout_centred(room, own) + at);
            }
        }
    }
    if (shown > 0)
    {
        used += self->gap * (shown - 1);
    }
    left = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(content)), self->axis) - used;
    if (left <= lh_ui_scalar(0))
    {
        /* A row that does not fit still starts where the content box does: going
           backwards would put the first child before its own parent. */
        left = lh_ui_scalar(0);
    }
    if (weights <= lh_ui_scalar(0) && shown > 0)
    {
        switch (lh_ui_layout_get_justify(self))
        {
        case lh_ui_justify_center:
            along += (left + 1) / 2;
            break;
        case lh_ui_justify_end:
            along += left;
            break;
        case lh_ui_justify_space_between:
            /* One child has nothing to be between, and sits at the start. */
            spread = shown > 1 ? left / (shown - 1) : lh_ui_scalar(0);
            break;
        case lh_ui_justify_space_around:
            spread = left / shown;
            along += spread / 2;
            break;
        case lh_ui_justify_space_evenly:
            spread = left / (shown + 1);
            along += spread;
            break;
        default:
            break;
        }
    }

    /* Place. */
    for (child = first; child != end; child = lh_ui_entity_get_next_child(parent, child))
    {
        const lh_ui_place_t *place = lh_ui_entity_get_place(child);
        const lh_ui_insets_t *margin = lh_ui_place_get_margin(place);
        const lh_ui_scalar_t before = lh_ui_insets_get_start(margin, self->axis);
        const lh_ui_scalar_t after = lh_ui_insets_get_end(margin, self->axis);
        lh_ui_scalar_t wants;
        lh_ui_scalar_t across_start = lh_ui_scalar(0);
        lh_ui_scalar_t across_length = lh_ui_scalar(0);
        lh_ui_point_t origin;
        lh_ui_rect_t placed;

        if (lh_ui_entity_is_hidden(child))
        {
            continue;
        }
        wants = (lh_ui_place_get_size_mode(place) == lh_ui_place_size_fill)
                    ? lh_ui_place_clamp(place, self->axis, left * layout_weight(place) / weights)
                    : layout_wants(self, child);
        layout_across(self, child, lh_addr_of(content), line, lh_addr_of(across_start), lh_addr_of(across_length));
        lh_ui_rect_init_along(lh_addr_of(placed), lh_addr_of(nowhere), self->axis, along + before, wants);
        lh_ui_rect_init_along(lh_addr_of(placed), lh_addr_of(placed), lh_ui_axis_get_cross(self->axis),
                              across_start, across_length);
        /* Moved, not resized: a child that carries a subtree has to take it along
           (::lh_ui_entity_move_to), and the size comes after, because moving is
           a delta and the child has to still be where it was for it to be right. */
        origin = *lh_ui_rect_get_origin_as_const(lh_addr_of(placed));
        lh_ui_entity_move_to(child, origin);
        lh_ui_entity_set_rect(child, placed);
        if (wants > lh_ui_scalar(0) || before + after > lh_ui_scalar(0))
        {
            along += before + wants + after + self->gap + spread;
        }
    }
}

/** How deep @p child is across the flow, margins and all: its own size, or -- for one
   that stretches across -- only its minimum, since the line it is in decides the rest. */
static lh_ui_scalar_t
layout_depth(const lh_ui_layout_t *self, lh_ui_entity_t *child)
{
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    const lh_ui_insets_t *margin = lh_ui_place_get_margin(place);
    const lh_ui_rect_t rect = lh_ui_entity_get_rect(child);
    const lh_ui_scalar_t own =
        lh_ui_place_get_align(place) == lh_ui_place_align_fill
            ? lh_ui_place_clamp(place, cross, lh_ui_scalar(0))
            : lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(rect)), cross);

    return own + lh_ui_insets_get_start(margin, cross) + lh_ui_insets_get_end(margin, cross);
}

/** The child after the last one that fits on the line starting at @p first in @p room
   along the flow (::lh_null when every one does), and in @p depth how deep the line is.
   A line always takes its first child, so a child longer than the room is a line on
   its own rather than none. */
static lh_ui_entity_t *
layout_line_end(const lh_ui_layout_t *self, lh_ui_entity_t *parent, lh_ui_entity_t *first, lh_ui_scalar_t room,
                lh_ui_scalar_t *depth)
{
    lh_ui_scalar_t used = lh_ui_scalar(0);
    int count = 0;
    lh_ui_entity_t *child;

    *depth = lh_ui_scalar(0);
    for (child = first; lh_null_ne(child); child = lh_ui_entity_get_next_child(parent, child))
    {
        lh_ui_scalar_t size;

        if (lh_ui_entity_is_hidden(child))
        {
            continue;
        }
        size = layout_wants(self, child) + layout_margin_along(self, lh_ui_entity_get_place(child));
        if (count > 0 && used + self->gap + size > room)
        {
            return child;
        }
        used += (count > 0 ? self->gap : lh_ui_scalar(0)) + size;
        ++count;
        *depth = lh_math_max(*depth, layout_depth(self, child));
    }
    return lh_null;
}

lh_void
lh_ui_layout_apply(const lh_ui_layout_t *self, lh_ui_entity_t *parent)
{
    const lh_ui_insets_t padding = lh_ui_entity_get_padding(parent);
    const lh_ui_rect_t rect = lh_ui_entity_get_rect(parent);
    const lh_ui_rect_t content = lh_ui_insets_shrink(lh_addr_of(padding), lh_addr_of(rect));
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    const lh_ui_scalar_t room = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(content)), self->axis);
    const lh_ui_scalar_t room_across = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(content)), cross);
    lh_ui_scalar_t depths = lh_ui_scalar(0);
    lh_ui_scalar_t extra = lh_ui_scalar(0);
    lh_ui_scalar_t at = lh_ui_scalar(0);
    lh_ui_scalar_t depth;
    lh_ui_entity_t *first;
    lh_ui_entity_t *end;
    int lines = 0;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(parent);
    if (!self->wrap)
    {
        layout_run(self, parent, lh_ui_entity_get_first_child(parent), lh_null, lh_addr_of(content));
        return;
    }
    /* How many lines, and how deep they are together; what is left across is then
       shared between them, which is CSS's `align-content: stretch`. */
    for (first = lh_ui_entity_get_first_child(parent); lh_null_ne(first); first = end)
    {
        end = layout_line_end(self, parent, first, room, lh_addr_of(depth));
        depths += depth;
        ++lines;
    }
    lh_return_if(lines == 0);
    if (room_across - depths - self->line_gap * (lines - 1) > lh_ui_scalar(0))
    {
        extra = (room_across - depths - self->line_gap * (lines - 1)) / lines;
    }
    for (first = lh_ui_entity_get_first_child(parent); lh_null_ne(first); first = end)
    {
        lh_ui_rect_t band;

        end = layout_line_end(self, parent, first, room, lh_addr_of(depth));
        lh_ui_rect_init_along(lh_addr_of(band), lh_addr_of(content), cross, at, depth + extra);
        layout_run(self, parent, first, end, lh_addr_of(band));
        at += depth + extra + self->line_gap;
    }
}
