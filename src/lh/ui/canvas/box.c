/**
 * @file box.c
 * @brief ::lh_ui_canvas_fill_box: a box with a radius for each corner and a paint that
 *        may be a gradient.
 *
 * A solid box with one radius is ::lh_ui_canvas_fill_round_rect, and goes there: the
 * backend's own rounded fill, unchanged. Anything else is drawn here with the canvas's
 * own sends, so every backend draws it the same. Where the colour does not change
 * across a row, the row is a span -- a run of such rows one box -- and a corner's
 * pixels go out as a short row of colours at the coverage of the corner they are in
 * (::lh_ui_radii_get_at). Where a gradient colours a pixel at a time, the row goes
 * out whole as a row of colours (::lh_ui_canvas_fill_colors), read off a
 * ::lh_ui_gradient_sampler_t, its corner pixels at their coverage.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/ui/canvas/box.h>
#include <lh/ui/canvas/round.h>
#include <lh/util/return.h>

/** How many pixels of a row are coloured before they are sent. */
#define LH_UI_CANVAS_BOX_RUN 256

lh_bool_t
lh_ui_canvas_box_rows_are_one_colour(const lh_ui_paint_t *paint)
{
    const lh_ui_gradient_t *gradient = lh_ui_paint_get_gradient(paint);

    lh_return_if(lh_null_eq(gradient), lh_bool_true);
    return lh_ui_gradient_get_shape(gradient) == lh_ui_gradient_shape_linear &&
                   lh_math_is_multiple_of(lh_ui_gradient_get_angle(gradient), 180)
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_ui_canvas_box_columns_are_one_colour(const lh_ui_paint_t *paint)
{
    const lh_ui_gradient_t *gradient = lh_ui_paint_get_gradient(paint);

    lh_return_if(lh_null_eq(gradient), lh_bool_false);
    return lh_ui_gradient_get_shape(gradient) == lh_ui_gradient_shape_linear &&
                   (lh_ui_gradient_get_angle(gradient) % 180) == 90
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_canvas_box_paint_row(const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler,
                           lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_ui_color_t *out)
{
    lh_s32_t x;

    if (lh_null_ne(sampler))
    {
        lh_ui_gradient_sampler_row(sampler, x0, x1, y, out);
        return;
    }
    for (x = x0; x < x1; ++x)
    {
        out[x - x0] = lh_ptr_deref(lh_ui_paint_get_color(paint));
    }
}

lh_void
lh_ui_canvas_box_row_colour(const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler,
                            lh_s32_t x, lh_s32_t y, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_color_t colour;

    if (lh_null_eq(sampler))
    {
        lh_ptr_deref(out) = lh_ptr_deref(lh_ui_paint_get_color(paint));
        return;
    }
    lh_ui_gradient_sampler_column(sampler, x, y, y + 1, lh_addr_of(colour));
    lh_ptr_deref(out) = colour;
}

lh_void
lh_ui_canvas_box_corner(lh_ui_canvas_t *self, const lh_ui_color_t *colour,
                        const struct lh_ui_radius_run *corner, lh_s32_t x0, lh_s32_t x1, lh_s32_t y)
{
    lh_ui_color_t colours[LH_UI_CANVAS_BOX_RUN];
    lh_byte_t cover[LH_UI_CANVAS_BOX_RUN];
    lh_s32_t end;
    lh_s32_t x;

    for (; x0 < x1; x0 = end)
    {
        end = lh_math_min(x1, x0 + LH_UI_CANVAS_BOX_RUN);
        lh_ui_radius_run_row(corner, x0, end, y, cover);
        for (x = x0; x < end; ++x)
        {
            lh_ui_color_with_coverage(colour, cover[x - x0], lh_addr_of(colours[x - x0]));
        }
        lh_ui_canvas_fill_colors(self, x0, y, end - x0, 1, colours, lh_ui_axis_horizontal);
    }
}

lh_void
lh_ui_canvas_box_row(lh_ui_canvas_t *self, const lh_ui_paint_t *paint,
                     lh_ui_gradient_sampler_t *sampler, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                     lh_s32_t near_end, const struct lh_ui_radius_run *near, lh_s32_t far_start,
                     const struct lh_ui_radius_run *far)
{
    lh_ui_color_t colours[LH_UI_CANVAS_BOX_RUN];
    lh_byte_t cover[LH_UI_CANVAS_BOX_RUN];
    lh_s32_t end;
    lh_s32_t cut;
    lh_s32_t x;

    near_end = lh_math_max(x0, lh_math_min(x1, near_end));
    far_start = lh_math_min(x1, lh_math_max(near_end, far_start));
    if (lh_ui_canvas_box_rows_are_one_colour(paint))
    {
        lh_ui_color_t colour;
        lh_ui_canvas_box_row_colour(paint, sampler, x0, y, lh_addr_of(colour));

        lh_ui_canvas_box_corner(self, lh_addr_of(colour), near, x0, near_end, y);
        lh_ui_canvas_fill_pixels(self, near_end, y, far_start - near_end, 1, lh_addr_of(colour));
        lh_ui_canvas_box_corner(self, lh_addr_of(colour), far, far_start, x1, y);
        return;
    }
    for (; x0 < x1; x0 = end)
    {
        end = lh_math_min(x1, x0 + LH_UI_CANVAS_BOX_RUN);
        lh_ui_canvas_box_paint_row(paint, sampler, x0, end, y, colours);
        cut = lh_math_min(end, near_end);
        if (x0 < cut)
        {
            lh_ui_radius_run_row(near, x0, cut, y, cover);
            for (x = x0; x < cut; ++x)
            {
                lh_ui_color_with_coverage(colours + (x - x0), cover[x - x0],
                                          lh_addr_of(colours[x - x0]));
            }
        }
        cut = lh_math_max(x0, far_start);
        if (cut < end)
        {
            lh_ui_radius_run_row(far, cut, end, y, cover);
            for (x = cut; x < end; ++x)
            {
                lh_ui_color_with_coverage(colours + (x - x0), cover[x - cut],
                                          lh_addr_of(colours[x - x0]));
            }
        }
        lh_ui_canvas_fill_colors(self, x0, y, end - x0, 1, colours, lh_ui_axis_horizontal);
    }
}

lh_void
lh_ui_canvas_box_band(lh_ui_canvas_t *self, const lh_ui_paint_t *paint,
                      lh_ui_gradient_sampler_t *sampler, lh_s32_t x0, lh_s32_t x1, lh_s32_t y0,
                      lh_s32_t y1)
{
    lh_s32_t y;

    lh_return_if(y1 <= y0 || x1 <= x0);
    if (lh_null_eq(sampler))
    {
        lh_ui_canvas_fill_pixels(self, x0, y0, x1 - x0, y1 - y0, lh_ui_paint_get_color(paint));
        return;
    }
    if (lh_ui_canvas_box_rows_are_one_colour(paint))
    {
        /* A colour a row, read down the first column, the rows sent together. */
        lh_ui_color_t colours[LH_UI_CANVAS_BOX_RUN];
        lh_s32_t end;

        for (y = y0; y < y1; y = end)
        {
            end = lh_math_min(y1, y + LH_UI_CANVAS_BOX_RUN);
            lh_ui_gradient_sampler_column(sampler, x0, y, end, colours);
            lh_ui_canvas_fill_colors(self, x0, y, x1 - x0, end - y, colours, lh_ui_axis_vertical);
        }
        return;
    }
    if (lh_ui_canvas_box_columns_are_one_colour(paint))
    {
        /* A colour a column, the same all the way down: the row once, for every row. */
        lh_ui_color_t colours[LH_UI_CANVAS_BOX_RUN];
        lh_s32_t end;
        lh_s32_t x;

        for (x = x0; x < x1; x = end)
        {
            end = lh_math_min(x1, x + LH_UI_CANVAS_BOX_RUN);
            lh_ui_gradient_sampler_row(sampler, x, end, y0, colours);
            lh_ui_canvas_fill_colors(self, x, y0, end - x, y1 - y0, colours, lh_ui_axis_horizontal);
        }
        return;
    }
    for (y = y0; y < y1; ++y)
    {
        lh_ui_canvas_box_row(self, paint, sampler, x0, x1, y, x0, lh_null, x1, lh_null);
    }
}

lh_bool_t
lh_ui_canvas_box_frame_init(const lh_ui_rect_t *box, const lh_ui_rect_t *cut,
                            lh_ui_canvas_box_frame_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_canvas_box_frame_set_left(out, lh_ui_canvas_round_left(box));
    lh_ui_canvas_box_frame_set_right(out, lh_ui_canvas_round_right(box));
    lh_ui_canvas_box_frame_set_top(out, lh_ui_canvas_round_top(box));
    lh_ui_canvas_box_frame_set_bottom(out, lh_ui_canvas_round_bottom(box));
    /* Only the rows and columns the clip shows: a long gradient list scrolled to its
       end does not colour the rows above the window, nor a wide one the columns off
       to the side. */
    lh_ui_canvas_box_frame_set_first(out, lh_math_max(lh_ui_canvas_box_frame_get_top(out), lh_ui_canvas_round_top(cut)));
    lh_ui_canvas_box_frame_set_last(out, lh_math_min(lh_ui_canvas_box_frame_get_bottom(out), lh_ui_canvas_round_bottom(cut)));
    lh_ui_canvas_box_frame_set_shown_left(out, lh_math_max(lh_ui_canvas_box_frame_get_left(out), lh_ui_canvas_round_left(cut)));
    lh_ui_canvas_box_frame_set_shown_right(out, lh_math_min(lh_ui_canvas_box_frame_get_right(out), lh_ui_canvas_round_right(cut)));
    return lh_ui_canvas_box_frame_get_shown_right(out) > lh_ui_canvas_box_frame_get_shown_left(out) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_canvas_box_corners_init(const lh_ui_rect_t *box, const lh_ui_radii_t *fit,
                              struct lh_ui_radius_run *out)
{
    lh_assert_runtime_ref(fit);
    lh_assert_runtime_ref(out);
    /* Each corner is the corner of a box rounded all round by its radius, and only
       the pixels in that corner are asked about it. */
    lh_ui_radius_run_init(out + 0, box, lh_ui_radii_get_top_left(fit));
    lh_ui_radius_run_init(out + 1, box, lh_ui_radii_get_top_right(fit));
    lh_ui_radius_run_init(out + 2, box, lh_ui_radii_get_bottom_left(fit));
    lh_ui_radius_run_init(out + 3, box, lh_ui_radii_get_bottom_right(fit));
}

lh_bool_t
lh_ui_canvas_box_row_is_upper(const lh_ui_canvas_box_frame_t *frame, lh_s32_t y)
{
    lh_assert_runtime_ref(frame);
    return (2 * y + 1 < lh_ui_canvas_box_frame_get_top(frame) + lh_ui_canvas_box_frame_get_bottom(frame)) ? lh_bool_true : lh_bool_false;
}

lh_s32_t
lh_ui_canvas_box_zone(lh_s32_t from_edge, lh_ui_scalar_t radius, lh_s32_t room)
{
    return from_edge < lh_ui_scalar_ceil_s32(radius)
               ? lh_math_min(lh_ui_scalar_ceil_s32(radius), room)
               : 0;
}

lh_bool_t
lh_ui_canvas_box_row_reach(const lh_ui_canvas_box_frame_t *frame, const lh_ui_radii_t *fit,
                           lh_s32_t y, lh_s32_t *near_end, lh_s32_t *far_start)
{
    const lh_bool_t upper = lh_ui_canvas_box_row_is_upper(frame, y);
    const lh_s32_t from_edge = upper ? y - lh_ui_canvas_box_frame_get_top(frame) : lh_ui_canvas_box_frame_get_bottom(frame) - 1 - y;
    const lh_s32_t zone_near = lh_ui_canvas_box_zone(
        from_edge, upper ? lh_ui_radii_get_top_left(fit) : lh_ui_radii_get_bottom_left(fit), lh_ui_canvas_box_frame_get_right(frame) - lh_ui_canvas_box_frame_get_left(frame));
    const lh_s32_t zone_far =
        lh_ui_canvas_box_zone(from_edge, upper ? lh_ui_radii_get_top_right(fit) : lh_ui_radii_get_bottom_right(fit),
                              lh_ui_canvas_box_frame_get_right(frame) - lh_ui_canvas_box_frame_get_left(frame) - zone_near);

    lh_assert_runtime_ref(near_end);
    lh_assert_runtime_ref(far_start);
    lh_ptr_deref(near_end) = lh_ui_canvas_box_frame_get_left(frame) + zone_near;
    lh_ptr_deref(far_start) = lh_ui_canvas_box_frame_get_right(frame) - zone_far;
    return (zone_near != 0 || zone_far != 0) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_canvas_box_rows(lh_ui_canvas_t *self, const lh_ui_paint_t *paint,
                      lh_ui_gradient_sampler_t *sampler, const lh_ui_canvas_box_frame_t *frame,
                      const lh_ui_radii_t *fit, const struct lh_ui_radius_run *corners)
{
    lh_s32_t plain_from = lh_ui_canvas_box_frame_get_first(frame);
    lh_s32_t near_end;
    lh_s32_t far_start;
    lh_s32_t y;

    for (y = lh_ui_canvas_box_frame_get_first(frame); y < lh_ui_canvas_box_frame_get_last(frame); ++y)
    {
        const lh_s32_t pair = lh_ui_canvas_box_row_is_upper(frame, y) ? 0 : 2;

        if (!lh_ui_canvas_box_row_reach(frame, fit, y, lh_addr_of(near_end), lh_addr_of(far_start)))
        {
            continue;
        }
        /* A row a corner reaches: the rows before it that none did go out as one
           band, then this one whole, its corners at their coverage. */
        lh_ui_canvas_box_band(self, paint, sampler, lh_ui_canvas_box_frame_get_shown_left(frame), lh_ui_canvas_box_frame_get_shown_right(frame),
                              plain_from, y);
        plain_from = y + 1;
        lh_ui_canvas_box_row(self, paint, sampler, lh_ui_canvas_box_frame_get_shown_left(frame), lh_ui_canvas_box_frame_get_shown_right(frame), y,
                             near_end, corners + pair, far_start, corners + pair + 1);
    }
    lh_ui_canvas_box_band(self, paint, sampler, lh_ui_canvas_box_frame_get_shown_left(frame), lh_ui_canvas_box_frame_get_shown_right(frame), plain_from,
                          lh_ui_canvas_box_frame_get_last(frame));
}

lh_void
lh_ui_canvas_fill_box(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_radii_t *radii,
                      const lh_ui_paint_t *paint)
{
    lh_ui_gradient_sampler_t sampler;
    lh_ui_gradient_sampler_t *colours = lh_null;
    struct lh_ui_radius_run corners[4];
    lh_ui_canvas_box_frame_t frame;
    lh_ui_radii_t fit;
    lh_ui_rect_t box;
    lh_ui_rect_t cut;

    lh_return_if(lh_null_eq(self) || lh_null_eq(rect) || lh_null_eq(radii) || lh_null_eq(paint));
    lh_return_if(lh_ui_paint_is_empty(paint));
    lh_ui_radii_clamp(radii, rect, lh_addr_of(fit));
    /* The common case keeps its own road: one colour and one radius are the
       backend's rounded fill, exactly as before there were corners and gradients. */
    if (lh_null_eq(lh_ui_paint_get_gradient(paint)) && lh_ui_radii_is_uniform(lh_addr_of(fit)))
    {
        lh_ui_canvas_fill_round_rect(self, rect, lh_ui_radii_get_top_left(lh_addr_of(fit)), lh_ui_paint_get_color(paint));
        return;
    }
    lh_ui_canvas_state_to_target(lh_ui_canvas_get_state_mut(self), rect, lh_addr_of(box));
    lh_ui_canvas_state_cut(lh_ui_canvas_get_state_mut(self), lh_addr_of(box), lh_addr_of(cut));
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    lh_return_if(!lh_ui_canvas_box_frame_init(lh_addr_of(box), lh_addr_of(cut), lh_addr_of(frame)));
    if (lh_null_ne(lh_ui_paint_get_gradient(paint)))
    {
        lh_ui_gradient_sampler_init(lh_addr_of(sampler), lh_ui_paint_get_gradient(paint),
                                    lh_addr_of(box));
        colours = lh_addr_of(sampler);
    }
    lh_ui_canvas_box_corners_init(lh_addr_of(box), lh_addr_of(fit), corners);
    lh_ui_canvas_box_rows(self, paint, colours, lh_addr_of(frame), lh_addr_of(fit), corners);
}

lh_s32_t
lh_ui_canvas_box_frame_get_left(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->left;
}

lh_void
lh_ui_canvas_box_frame_set_left(lh_ui_canvas_box_frame_t *self, lh_s32_t left)
{
    lh_assert_runtime_ref(self);
    self->left = left;
}

lh_s32_t
lh_ui_canvas_box_frame_get_right(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->right;
}

lh_void
lh_ui_canvas_box_frame_set_right(lh_ui_canvas_box_frame_t *self, lh_s32_t right)
{
    lh_assert_runtime_ref(self);
    self->right = right;
}

lh_s32_t
lh_ui_canvas_box_frame_get_top(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

lh_void
lh_ui_canvas_box_frame_set_top(lh_ui_canvas_box_frame_t *self, lh_s32_t top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

lh_s32_t
lh_ui_canvas_box_frame_get_bottom(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom;
}

lh_void
lh_ui_canvas_box_frame_set_bottom(lh_ui_canvas_box_frame_t *self, lh_s32_t bottom)
{
    lh_assert_runtime_ref(self);
    self->bottom = bottom;
}

lh_s32_t
lh_ui_canvas_box_frame_get_shown_left(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shown_left;
}

lh_void
lh_ui_canvas_box_frame_set_shown_left(lh_ui_canvas_box_frame_t *self, lh_s32_t shown_left)
{
    lh_assert_runtime_ref(self);
    self->shown_left = shown_left;
}

lh_s32_t
lh_ui_canvas_box_frame_get_shown_right(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shown_right;
}

lh_void
lh_ui_canvas_box_frame_set_shown_right(lh_ui_canvas_box_frame_t *self, lh_s32_t shown_right)
{
    lh_assert_runtime_ref(self);
    self->shown_right = shown_right;
}

lh_s32_t
lh_ui_canvas_box_frame_get_first(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_void
lh_ui_canvas_box_frame_set_first(lh_ui_canvas_box_frame_t *self, lh_s32_t first)
{
    lh_assert_runtime_ref(self);
    self->first = first;
}

lh_s32_t
lh_ui_canvas_box_frame_get_last(const lh_ui_canvas_box_frame_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last;
}

lh_void
lh_ui_canvas_box_frame_set_last(lh_ui_canvas_box_frame_t *self, lh_s32_t last)
{
    lh_assert_runtime_ref(self);
    self->last = last;
}
