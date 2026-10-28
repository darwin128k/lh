#include <lh/ui/layout/run.h>
#include <lh/math.h>
#include <lh/ui/layout.h>
#include <lh/util/return.h>

lh_void
lh_ui_layout_run_init(const lh_ui_layout_t *self, const lh_ui_rect_t *content,
                      lh_ui_layout_run_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    out->along = lh_ui_point_get_along(lh_ui_rect_get_origin(content), self->axis);
    out->used = lh_ui_scalar(0);
    out->weights = lh_ui_scalar(0);
    out->left = lh_ui_scalar(0);
    out->spread = lh_ui_scalar(0);
    out->line = lh_ui_scalar(-1);
    out->shown = 0;
}

lh_void
lh_ui_layout_run_measure_child(const lh_ui_layout_t *self, lh_ui_layout_run_t *run,
                               lh_ui_entity_t *child, const lh_ui_rect_t *content)
{
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    const lh_bool_t fills_too = lh_ui_place_get_size_mode(place) == lh_ui_place_size_fill;
    const lh_ui_scalar_t margin = lh_ui_layout_margin_along(self, place);
    lh_ui_scalar_t wants;

    lh_assert_runtime_ref(run);
    lh_return_if(lh_ui_entity_is_hidden(child));
    if (fills_too)
    {
        run->weights += lh_ui_layout_weight(place);
    }
    wants = lh_ui_layout_wants(self, child);
    /* A child that asked to fill has room to take even before anyone has said
       how much, so it counts for a gap; one that asked for nothing does not,
       and neither does one that is not there to be seen. A margin is room too. */
    if (wants > lh_ui_scalar(0) || fills_too || margin > lh_ui_scalar(0))
    {
        run->used += wants + margin;
        ++run->shown;
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
       own baseline is the deepest there is. */
    run->line = lh_math_max(run->line, lh_ui_layout_child_line(self, child, content));
}

lh_void
lh_ui_layout_run_measure(const lh_ui_layout_t *self, lh_ui_layout_run_t *run,
                         lh_ui_entity_t *parent, lh_ui_entity_t *first, lh_ui_entity_t *end,
                         const lh_ui_rect_t *content)
{
    lh_ui_entity_t *child;

    lh_assert_runtime_ref(run);
    /* Measure: what everyone asked for, and what is left of the content box. The
       gap is counted between children that take room only, so a collapsed one
       leaves neither space nor a hole in the row. */
    for (child = first; child != end; child = lh_ui_entity_get_next_child(parent, child))
    {
        lh_ui_layout_run_measure_child(self, run, child, content);
    }
    if (run->shown > 0)
    {
        run->used += self->gap * (run->shown - 1);
    }
    /* A row that does not fit still starts where the content box does: going
       backwards would put the first child before its own parent. */
    run->left = lh_ui_scalar_slack(lh_ui_size_get_along(lh_ui_rect_get_size(content), self->axis),
                                   run->used);
}

lh_void
lh_ui_layout_run_justify(const lh_ui_layout_t *self, lh_ui_layout_run_t *run)
{
    lh_ui_scalar_t lead;

    lh_assert_runtime_ref(run);
    lh_return_if(run->weights > lh_ui_scalar(0) || run->shown <= 0);
    lh_ui_layout_justify_lead(lh_ui_layout_get_justify(self), run->left, run->shown,
                              lh_addr_of(lead), lh_addr_of(run->spread));
    run->along += lead;
}

lh_ui_scalar_t
lh_ui_layout_run_length(const lh_ui_layout_t *self, const lh_ui_layout_run_t *run,
                        lh_ui_entity_t *child)
{
    const lh_ui_place_t *place = lh_ui_entity_get_place(child);
    lh_ui_scalar_t share;

    lh_assert_runtime_ref(run);
    if (lh_ui_place_get_size_mode(place) != lh_ui_place_size_fill)
    {
        return lh_ui_layout_wants(self, child);
    }
    lh_ui_scalar_trunc(run->left * lh_ui_layout_weight(place) / run->weights, lh_addr_of(share));
    return lh_ui_place_clamp(place, self->axis, share);
}

lh_void
lh_ui_layout_run_place_child(const lh_ui_layout_t *self, lh_ui_layout_run_t *run,
                             lh_ui_entity_t *child, const lh_ui_rect_t *content)
{
    /* ::lh_ui_rect_init_along measures `start` past the origin of its base, and
       the rects a child carries are absolute, so the slice is cut out of nothing
       and the two calls hand the child its own place. */
    const lh_ui_rect_t nowhere = {{0, 0}, {0, 0}};
    const lh_ui_insets_t *margin = lh_ui_place_get_margin(lh_ui_entity_get_place(child));
    const lh_ui_scalar_t before = lh_ui_insets_get_start(margin, self->axis);
    const lh_ui_scalar_t after = lh_ui_insets_get_end(margin, self->axis);
    lh_ui_scalar_t wants;
    lh_ui_scalar_t across_start = lh_ui_scalar(0);
    lh_ui_scalar_t across_length = lh_ui_scalar(0);
    lh_ui_rect_t placed;

    lh_assert_runtime_ref(run);
    lh_return_if(lh_ui_entity_is_hidden(child));
    wants = lh_ui_layout_run_length(self, run, child);
    lh_ui_layout_across(self, child, content, run->line, lh_addr_of(across_start),
                        lh_addr_of(across_length));
    lh_ui_rect_init_along(lh_addr_of(placed), lh_addr_of(nowhere), self->axis, run->along + before,
                          wants);
    lh_ui_rect_init_along(lh_addr_of(placed), lh_addr_of(placed), lh_ui_axis_get_cross(self->axis),
                          across_start, across_length);
    /* Moved, not resized: a child that carries a subtree has to take it along
       (::lh_ui_entity_move_to), and the size comes after, because moving is
       a delta and the child has to still be where it was for it to be right. */
    lh_ui_entity_move_to(child, lh_ptr_deref(lh_ui_rect_get_origin(lh_addr_of(placed))));
    lh_ui_entity_set_rect(child, placed);
    if (wants > lh_ui_scalar(0) || before + after > lh_ui_scalar(0))
    {
        run->along += before + wants + after + self->gap + run->spread;
    }
}

lh_void
lh_ui_layout_run_place(const lh_ui_layout_t *self, lh_ui_layout_run_t *run, lh_ui_entity_t *parent,
                       lh_ui_entity_t *first, lh_ui_entity_t *end, const lh_ui_rect_t *content)
{
    lh_ui_entity_t *child;

    for (child = first; child != end; child = lh_ui_entity_get_next_child(parent, child))
    {
        lh_ui_layout_run_place_child(self, run, child, content);
    }
}
