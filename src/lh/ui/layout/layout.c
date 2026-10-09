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

/* What one child wants along the flow, before anything is shared out: its own
   length, or its content, or nothing at all for a child that wants to fill —
   that one is decided once every other child has said. A `wrap` child with no
   content asks for nothing, and that is what makes an empty label collapse. */
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
        return lh_ui_place_get_size(place);
    }
    content = lh_ui_entity_get_content_bounds(child);
    return lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(content)), self->axis);
}

/* Where a child of @p own rows sits in @p room when it is centred across the flow.

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

/* A child across the flow keeps the size it has and is put where it asked; only
   `fill` takes the whole cross side of the content box.

   @p line is where the row's baseline sits, measured down from the cross origin
   of @p content, or -1 when the row has none. It is measured before this pass
   because a child's own place depends on the line and the line is the deepest of
   the children's: measured the other way round each child would be setting a line
   and sitting on it at once. */
static void
layout_across(const lh_ui_layout_t *self, lh_ui_entity_t *child, const lh_ui_rect_t *content,
              lh_ui_scalar_t line, lh_ui_scalar_t *start, lh_ui_scalar_t *length)
{
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    const lh_ui_rect_t rect = lh_ui_entity_get_rect(child);
    const lh_ui_place_align_t align = lh_ui_place_get_align(lh_ui_entity_get_place(child));
    const lh_ui_scalar_t own = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(rect)), cross);
    const lh_ui_scalar_t room = lh_ui_size_get_along(lh_ui_rect_get_size_as_const(content), cross);
    lh_ui_scalar_t at = lh_ui_scalar(0);

    if (align == lh_ui_place_align_fill)
    {
        *length = room;
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
            at = (base >= lh_ui_scalar(0) && line >= lh_ui_scalar(0)) ? line - base
                                                                       : layout_centred(room, own);
        }
    }
    *start = lh_math_max(
        lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(content), cross) +
            lh_math_max(at, lh_ui_scalar(0)),
        lh_ui_scalar(0));
}

lh_void
lh_ui_layout_apply(const lh_ui_layout_t *self, lh_ui_entity_t *parent)
{
    const lh_ui_insets_t padding = lh_ui_entity_get_padding(parent);
    const lh_ui_rect_t rect = lh_ui_entity_get_rect(parent);
    const lh_ui_rect_t content = lh_ui_insets_shrink(lh_addr_of(padding), lh_addr_of(rect));
    /* ::lh_ui_rect_init_along measures `start` past the origin of its base, and
       the rects a child carries are absolute, so the slice is cut out of nothing
       and the two calls hand the child its own place. */
    const lh_ui_rect_t nowhere = {0, 0, 0, 0};
    lh_ui_scalar_t along = lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(content)),
                                                 self->axis);
    lh_ui_scalar_t used = lh_ui_scalar(0);
    lh_ui_scalar_t share = lh_ui_scalar(0);
    lh_ui_scalar_t left = lh_ui_scalar(0);
    lh_ui_scalar_t line = lh_ui_scalar(-1);
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    int shown = 0;
    int fills = 0;
    lh_ui_entity_t *child;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(parent);

    /* Measure: what everyone asked for, and what is left of the content box. The
       gap is counted between children that take room only, so a collapsed one
       leaves neither space nor a hole in the row. */
    for (child = lh_ui_entity_get_first_child(parent); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(parent, child))
    {
        const lh_bool_t fills_too =
            lh_ui_place_get_size_mode(lh_ui_entity_get_place(child)) == lh_ui_place_size_fill;
        lh_ui_scalar_t wants;

        if (lh_ui_entity_is_hidden(child))
        {
            continue;
        }
        if (fills_too)
        {
            ++fills;
        }
        wants = layout_wants(self, child);
        /* A child that asked to fill has room to take even before anyone has said
           how much, so it counts for a gap; one that asked for nothing does not,
           and neither does one that is not there to be seen. */
        if (wants > lh_ui_scalar(0) || fills_too)
        {
            used += wants;
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
           row to stand on and never asks. */
        if (cross == lh_ui_axis_vertical &&
            lh_ui_place_get_align(lh_ui_entity_get_place(child)) == lh_ui_place_align_baseline)
        {
            const lh_ui_scalar_t at = lh_ui_entity_get_baseline(child);
            const lh_ui_rect_t rect = lh_ui_entity_get_rect(child);
            const lh_ui_scalar_t own =
                lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(rect)), cross);
            const lh_ui_scalar_t room =
                lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(content)), cross);

            if (at >= lh_ui_scalar(0))
            {
                line = lh_math_max(line, layout_centred(room, own) + at);
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
    if (fills > 0)
    {
        share = left / fills;
        left = lh_ui_scalar(0);
    }
    else if (lh_ui_layout_get_justify(self) == lh_ui_justify_center)
    {
        along += (left + 1) / 2;
        left = lh_ui_scalar(0);
    }
    else if (lh_ui_layout_get_justify(self) == lh_ui_justify_end)
    {
        along += left;
        left = lh_ui_scalar(0);
    }

    /* Place. */
    for (child = lh_ui_entity_get_first_child(parent); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(parent, child))
    {
        lh_ui_scalar_t wants;
        lh_ui_scalar_t across_start = lh_ui_scalar(0);
        lh_ui_scalar_t across_length = lh_ui_scalar(0);
        lh_ui_point_t origin;
        lh_ui_rect_t placed;

        if (lh_ui_entity_is_hidden(child))
        {
            continue;
        }
        wants = (lh_ui_place_get_size_mode(lh_ui_entity_get_place(child)) == lh_ui_place_size_fill)
                    ? share
                    : layout_wants(self, child);
        layout_across(self, child, lh_addr_of(content), line, lh_addr_of(across_start), lh_addr_of(across_length));
        lh_ui_rect_init_along(lh_addr_of(placed), lh_addr_of(nowhere), self->axis, along, wants);
        lh_ui_rect_init_along(lh_addr_of(placed), lh_addr_of(placed), lh_ui_axis_get_cross(self->axis),
                              across_start, across_length);
        /* Moved, not resized: a child that carries a subtree has to take it along
           (::lh_ui_entity_move_to), and the size comes after, because moving is
           a delta and the child has to still be where it was for it to be right. */
        origin = *lh_ui_rect_get_origin_as_const(lh_addr_of(placed));
        lh_ui_entity_move_to(child, origin);
        lh_ui_entity_set_rect(child, placed);
        if (wants > lh_ui_scalar(0))
        {
            along += wants + self->gap;
        }
    }
}