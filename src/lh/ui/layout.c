/**
 * @file layout.c
 * @brief Implementation of `lh/ui/layout.h`.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/ui/layout.h>
#include <lh/util/return.h>

lh_void
lh_ui_layout_init(lh_ui_layout_t *self, lh_ui_axis_t axis, lh_ui_scalar_t gap)
{
    lh_assert_runtime_if(gap < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    lh_ui_layout_set_axis(self, axis);
    lh_ui_layout_set_gap(self, gap);
    lh_ui_layout_set_justify(self, lh_ui_justify_start);
    lh_ui_layout_set_wrap(self, lh_bool_false);
    lh_ui_layout_set_line_gap(self, lh_ui_scalar(0));
}

lh_void
lh_ui_layout_set_wrapping(lh_ui_layout_t *self, lh_bool_t wrap, lh_ui_scalar_t line_gap)
{
    lh_assert_runtime_if(line_gap < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    lh_ui_layout_set_wrap(self, wrap);
    lh_ui_layout_set_line_gap(self, line_gap);
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

/* -- The pass -------------------------------------------------------------- */

lh_ui_scalar_t
lh_ui_layout_wants(const lh_ui_layout_t *self, lh_ui_entity_t *child)
{
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    lh_ui_rect_t content;

    if (lh_ui_place_get_size_mode(place) == lh_ui_place_size_fill)
    {
        return lh_ui_scalar(0);
    }
    if (lh_ui_place_get_size_mode(place) == lh_ui_place_size_fixed)
    {
        return lh_ui_place_clamp(place, lh_ui_layout_get_axis(self), lh_ui_place_get_size(place));
    }
    lh_ui_entity_get_content_bounds(child, lh_addr_of(content));
    return lh_ui_place_clamp(place, lh_ui_layout_get_axis(self),
                             lh_ui_size_get_along(lh_ui_rect_get_size(lh_addr_of(content)),
                                                  lh_ui_layout_get_axis(self)));
}

lh_ui_scalar_t
lh_ui_layout_weight(const lh_ui_place_t *place)
{
    const lh_ui_scalar_t weight = lh_ui_place_get_size(place);

    return weight > lh_ui_scalar(0) ? weight : lh_ui_scalar(1);
}

lh_ui_scalar_t
lh_ui_layout_margin_along(const lh_ui_layout_t *self, const lh_ui_place_t *place)
{
    const lh_ui_insets_t *margin = lh_ui_place_get_margin(place);

    return lh_ui_insets_get_start(margin, lh_ui_layout_get_axis(self)) +
           lh_ui_insets_get_end(margin, lh_ui_layout_get_axis(self));
}

lh_void
lh_ui_layout_slot(const lh_ui_layout_t *self, const lh_ui_place_t *place,
                  const lh_ui_rect_t *content, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(lh_ui_layout_get_axis(self));
    const lh_ui_insets_t *margin = lh_ui_place_get_margin(place);
    const lh_ui_scalar_t before = lh_ui_insets_get_start(margin, cross);
    const lh_ui_scalar_t after = lh_ui_insets_get_end(margin, cross);
    const lh_ui_scalar_t room = lh_ui_size_get_along(lh_ui_rect_get_size(content), cross);
    lh_ui_rect_t slot;

    lh_ui_rect_init_along(lh_addr_of(slot), content, cross, before,
                          lh_ui_scalar_slack(room - before, after));
    lh_ptr_deref(out) = slot;
}

lh_void
lh_ui_layout_across(const lh_ui_layout_t *self, lh_ui_entity_t *child, const lh_ui_rect_t *content,
                    lh_ui_scalar_t line, lh_ui_scalar_t *start, lh_ui_scalar_t *length)
{
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    const lh_ui_place_align_t align = lh_ui_place_get_align(place);
    lh_ui_layout_cross_t fit;
    lh_ui_scalar_t at = lh_ui_scalar(0);

    lh_ui_layout_cross_fit(self, child, content, lh_addr_of(fit));
    if (align == lh_ui_place_align_fill)
    {
        lh_ptr_deref(length) =
            lh_ui_place_clamp(place, lh_ui_axis_get_cross(lh_ui_layout_get_axis(self)),
                              lh_ui_layout_cross_get_room(lh_addr_of(fit)));
    }
    else
    {
        lh_ptr_deref(length) = lh_ui_layout_cross_get_own(lh_addr_of(fit));
        at = lh_ui_layout_cross_at(lh_addr_of(fit), align,
                                   align == lh_ui_place_align_baseline
                                       ? lh_ui_entity_get_baseline(child)
                                       : lh_ui_scalar(-1),
                                   line);
    }
    lh_ptr_deref(start) = lh_math_max(lh_ui_layout_cross_get_start(lh_addr_of(fit)) +
                                          lh_math_max(at, lh_ui_scalar(0)),
                                      lh_ui_scalar(0));
}

lh_ui_scalar_t
lh_ui_layout_child_line(const lh_ui_layout_t *self, lh_ui_entity_t *child,
                        const lh_ui_rect_t *content)
{
    const lh_ui_scalar_t at = lh_ui_entity_get_baseline(child);
    lh_ui_layout_cross_t fit;

    /* A **vertical** flow has no cross row to line up and never asks. */
    if (lh_ui_axis_get_cross(lh_ui_layout_get_axis(self)) != lh_ui_axis_vertical ||
        lh_ui_place_get_align(lh_ui_entity_get_place(child)) != lh_ui_place_align_baseline ||
        at < lh_ui_scalar(0))
    {
        return lh_ui_scalar(-1);
    }
    lh_ui_layout_cross_fit(self, child, content, lh_addr_of(fit));
    return lh_ui_layout_cross_get_before(lh_addr_of(fit)) +
           lh_ui_scalar_centre(lh_ui_layout_cross_get_room(lh_addr_of(fit)),
                               lh_ui_layout_cross_get_own(lh_addr_of(fit))) +
           at;
}

lh_void
lh_ui_layout_justify_lead(lh_ui_justify_t justify, lh_ui_scalar_t left, int shown,
                          lh_ui_scalar_t *lead, lh_ui_scalar_t *spread)
{
    lh_assert_runtime_ref(lead);
    lh_assert_runtime_ref(spread);
    lh_assert_runtime_if(shown <= 0, lh_runtime_error_code_invalid_argument);
    lh_ptr_deref(lead) = lh_ui_scalar(0);
    lh_ptr_deref(spread) = lh_ui_scalar(0);
    switch (justify)
    {
    case lh_ui_justify_center:
        lh_ptr_deref(lead) = lh_ui_scalar_centre(left, lh_ui_scalar(0));
        break;
    case lh_ui_justify_end:
        lh_ptr_deref(lead) = left;
        break;
    case lh_ui_justify_space_between:
        /* One child has nothing to be between, and sits at the start. */
        lh_ui_scalar_trunc(shown > 1 ? left / (shown - 1) : lh_ui_scalar(0), spread);
        break;
    case lh_ui_justify_space_around:
        lh_ui_scalar_trunc(left / shown, spread);
        lh_ui_scalar_trunc(lh_ptr_deref(spread) / 2, lead);
        break;
    case lh_ui_justify_space_evenly:
        lh_ui_scalar_trunc(left / (shown + 1), spread);
        lh_ptr_deref(lead) = lh_ptr_deref(spread);
        break;
    default:
        break;
    }
}

lh_void
lh_ui_layout_run(const lh_ui_layout_t *self, lh_ui_entity_t *parent, lh_ui_entity_t *first,
                 lh_ui_entity_t *end, const lh_ui_rect_t *content_box)
{
    const lh_ui_rect_t content = lh_ptr_deref(content_box);
    lh_ui_layout_run_t run;

    lh_assert_runtime_ref(parent);
    lh_ui_layout_run_init(self, lh_addr_of(content), lh_addr_of(run));
    lh_ui_layout_run_measure(self, lh_addr_of(run), parent, first, end, lh_addr_of(content));
    lh_ui_layout_run_justify(self, lh_addr_of(run));
    lh_ui_layout_run_place(self, lh_addr_of(run), parent, first, end, lh_addr_of(content));
}

lh_ui_scalar_t
lh_ui_layout_depth(const lh_ui_layout_t *self, lh_ui_entity_t *child)
{
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(lh_ui_layout_get_axis(self));
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    const lh_ui_insets_t *margin = lh_ui_place_get_margin(place);
    lh_ui_rect_t rect;
    lh_ui_entity_copy_rect(child, lh_addr_of(rect));
    const lh_ui_scalar_t own =
        lh_ui_place_get_align(place) == lh_ui_place_align_fill
            ? lh_ui_place_clamp(place, cross, lh_ui_scalar(0))
            : lh_ui_size_get_along(lh_ui_rect_get_size(lh_addr_of(rect)), cross);

    return own + lh_ui_insets_get_start(margin, cross) + lh_ui_insets_get_end(margin, cross);
}

lh_ui_entity_t *
lh_ui_layout_line_end(const lh_ui_layout_t *self, lh_ui_entity_t *parent, lh_ui_entity_t *first,
                      lh_ui_scalar_t room, lh_ui_scalar_t *depth)
{
    lh_ui_scalar_t used = lh_ui_scalar(0);
    int count = 0;
    lh_ui_entity_t *child;

    lh_ptr_deref(depth) = lh_ui_scalar(0);
    for (child = first; lh_null_ne(child); child = lh_ui_entity_get_next_child(parent, child))
    {
        lh_ui_scalar_t size;

        if (lh_ui_entity_is_hidden(child))
        {
            continue;
        }
        size = lh_ui_layout_wants(self, child) +
               lh_ui_layout_margin_along(self, lh_ui_entity_get_place(child));
        if (count > 0 && used + lh_ui_layout_get_gap(self) + size > room)
        {
            return child;
        }
        used += (count > 0 ? lh_ui_layout_get_gap(self) : lh_ui_scalar(0)) + size;
        ++count;
        lh_ptr_deref(depth) = lh_math_max(lh_ptr_deref(depth), lh_ui_layout_depth(self, child));
    }
    return lh_null;
}

lh_void
lh_ui_layout_apply(const lh_ui_layout_t *self, lh_ui_entity_t *parent)
{
    lh_ui_insets_t padding;
    lh_ui_entity_get_padding(parent, lh_addr_of(padding));
    lh_ui_rect_t rect;
    lh_ui_entity_copy_rect(parent, lh_addr_of(rect));
    lh_ui_rect_t content;
    lh_ui_insets_shrink(lh_addr_of(padding), lh_addr_of(rect), lh_addr_of(content));
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(lh_ui_layout_get_axis(self));
    const lh_ui_scalar_t room =
        lh_ui_size_get_along(lh_ui_rect_get_size(lh_addr_of(content)), lh_ui_layout_get_axis(self));
    const lh_ui_scalar_t room_across =
        lh_ui_size_get_along(lh_ui_rect_get_size(lh_addr_of(content)), cross);
    lh_ui_scalar_t depths = lh_ui_scalar(0);
    lh_ui_scalar_t extra = lh_ui_scalar(0);
    lh_ui_scalar_t at = lh_ui_scalar(0);
    int lines = 0;

    lh_assert_runtime_ref(parent);
    if (!lh_ui_layout_is_wrap(self))
    {
        lh_ui_layout_run(self, parent, lh_ui_entity_get_first_child(parent), lh_null,
                         lh_addr_of(content));
        return;
    }
    lh_ui_scalar_t depth;
    lh_ui_entity_t *first;
    lh_ui_entity_t *end;
    /* How many lines, and how deep they are together; what is left across is then
       shared between them, which is CSS's `align-content: stretch`. */
    for (first = lh_ui_entity_get_first_child(parent); lh_null_ne(first); first = end)
    {
        end = lh_ui_layout_line_end(self, parent, first, room, lh_addr_of(depth));
        depths += depth;
        ++lines;
    }
    lh_return_if(lines == 0);
    if (room_across - depths - lh_ui_layout_get_line_gap(self) * (lines - 1) > lh_ui_scalar(0))
    {
        lh_ui_scalar_trunc((room_across - depths - lh_ui_layout_get_line_gap(self) * (lines - 1)) /
                               lines,
                           lh_addr_of(extra));
    }
    for (first = lh_ui_entity_get_first_child(parent); lh_null_ne(first); first = end)
    {
        lh_ui_rect_t band;

        end = lh_ui_layout_line_end(self, parent, first, room, lh_addr_of(depth));
        lh_ui_rect_init_along(lh_addr_of(band), lh_addr_of(content), cross, at, depth + extra);
        lh_ui_layout_run(self, parent, first, end, lh_addr_of(band));
        at += depth + extra + lh_ui_layout_get_line_gap(self);
    }
}

lh_void
lh_ui_layout_set_axis(lh_ui_layout_t *self, lh_ui_axis_t axis)
{
    lh_assert_runtime_ref(self);
    self->axis = axis;
}

lh_void
lh_ui_layout_set_gap(lh_ui_layout_t *self, lh_ui_scalar_t gap)
{
    lh_assert_runtime_ref(self);
    self->gap = gap;
}

lh_bool_t
lh_ui_layout_is_wrap(const lh_ui_layout_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wrap;
}

lh_void
lh_ui_layout_set_line_gap(lh_ui_layout_t *self, lh_ui_scalar_t line_gap)
{
    lh_assert_runtime_ref(self);
    self->line_gap = line_gap;
}

lh_void
lh_ui_layout_set_wrap(lh_ui_layout_t *self, lh_bool_t wrap)
{
    lh_assert_runtime_ref(self);
    self->wrap = wrap;
}
