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

#include <lh/void.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/box.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/gradient.h>
#include <lh/ui/gradient/sampler.h>
#include <lh/ui/paint.h>
#include <lh/ui/radii.h>
#include <lh/ui/radius.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* How many pixels of a row are coloured before they are sent. */
#define LH_UI_CANVAS_BOX_RUN 256

/* True when the colour of @p paint does not change across a row (solid, or a
   gradient that runs straight up or down). */
static lh_bool_t
lh_ui_canvas_box_rows_are_one_colour(const lh_ui_paint_t *paint)
{
    const lh_ui_gradient_t *gradient = lh_ui_paint_get_gradient(paint);

    lh_return_if(lh_null_eq(gradient), lh_bool_true);
    return lh_ui_gradient_get_shape(gradient) == lh_ui_gradient_shape_linear &&
                   (lh_ui_gradient_get_angle(gradient) % 180) == 0
               ? lh_bool_true
               : lh_bool_false;
}

/* True when the colour of @p paint does not change down a column (a gradient that
   runs straight across). */
static lh_bool_t
lh_ui_canvas_box_columns_are_one_colour(const lh_ui_paint_t *paint)
{
    const lh_ui_gradient_t *gradient = lh_ui_paint_get_gradient(paint);

    lh_return_if(lh_null_eq(gradient), lh_bool_false);
    return lh_ui_gradient_get_shape(gradient) == lh_ui_gradient_shape_linear &&
                   (lh_ui_gradient_get_angle(gradient) % 180) == 90
               ? lh_bool_true
               : lh_bool_false;
}

/* The colours of pixels @p x0 .. @p x1 - 1 of row @p y into @p out: the gradient's
   when there is @p sampler, else the paint's one colour. */
static lh_void
lh_ui_canvas_box_paint_row(const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler, lh_s32_t x0,
                           lh_s32_t x1, lh_s32_t y, lh_ui_color_t *out)
{
    lh_s32_t x;

    if (lh_null_ne(sampler))
    {
        lh_ui_gradient_sampler_row(sampler, x0, x1, y, out);
        return;
    }
    for (x = x0; x < x1; ++x)
    {
        out[x - x0] = *lh_ui_paint_get_color(paint);
    }
}

/* The one colour of row @p y of a paint whose rows are one colour each. */
static lh_ui_color_t
lh_ui_canvas_box_row_colour(const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler, lh_s32_t x,
                            lh_s32_t y)
{
    lh_ui_color_t colour;

    lh_return_if(lh_null_eq(sampler), *lh_ui_paint_get_color(paint));
    lh_ui_gradient_sampler_column(sampler, x, y, y + 1, lh_addr_of(colour));
    return colour;
}

/* Pixels @p x0 .. @p x1 - 1 of row @p y in one colour @p colour, each at its
   coverage by the corner @p corner (::lh_ui_radius_run_init). */
static lh_void
lh_ui_canvas_box_corner(lh_ui_canvas_t *self, const lh_ui_color_t *colour, const struct lh_ui_radius_run *corner,
                        lh_s32_t x0, lh_s32_t x1, lh_s32_t y)
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
            colours[x - x0] = lh_ui_color_with_coverage(colour, cover[x - x0]);
        }
        lh_ui_canvas_fill_colors(self, x0, y, end - x0, 1, colours, lh_ui_axis_horizontal);
    }
}

/* Pixels @p x0 .. @p x1 - 1 of row @p y; those left of @p near_end are cut by the
   corner @p near and those from @p far_start on by the corner @p far, each the
   corner of a box rounded all round by its radius (::lh_ui_radius_run_init). A row
   of one colour sends its corners as colours and the middle as one span; any other
   goes out whole as a row of colours. */
static lh_void
lh_ui_canvas_box_row(lh_ui_canvas_t *self, const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler,
                     lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_s32_t near_end, const struct lh_ui_radius_run *near,
                     lh_s32_t far_start, const struct lh_ui_radius_run *far)
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
        const lh_ui_color_t colour = lh_ui_canvas_box_row_colour(paint, sampler, x0, y);

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
                colours[x - x0] = lh_ui_color_with_coverage(colours + (x - x0), cover[x - x0]);
            }
        }
        cut = lh_math_max(x0, far_start);
        if (cut < end)
        {
            lh_ui_radius_run_row(far, cut, end, y, cover);
            for (x = cut; x < end; ++x)
            {
                colours[x - x0] = lh_ui_color_with_coverage(colours + (x - x0), cover[x - cut]);
            }
        }
        lh_ui_canvas_fill_colors(self, x0, y, end - x0, 1, colours, lh_ui_axis_horizontal);
    }
}

/* Rows @p y0 .. @p y1 - 1, which no corner reaches: one box when the paint is one
   colour, one box of a colour a row or a colour a column when it changes only one
   way, else a row of colours a row. */
static lh_void
lh_ui_canvas_box_band(lh_ui_canvas_t *self, const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler,
                      lh_s32_t x0, lh_s32_t x1, lh_s32_t y0, lh_s32_t y1)
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

lh_void
lh_ui_canvas_fill_box(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_radii_t *radii,
                      const lh_ui_paint_t *paint)
{
    lh_ui_gradient_sampler_t sampler;
    lh_ui_gradient_sampler_t *colours = lh_null;
    struct lh_ui_radius_run corners[4];
    lh_ui_radii_t fit;
    lh_ui_rect_t box;
    lh_ui_rect_t cut;
    lh_s32_t left;
    lh_s32_t right;
    lh_s32_t top;
    lh_s32_t bottom;
    lh_s32_t first;
    lh_s32_t last;
    lh_s32_t shown_left;
    lh_s32_t shown_right;
    lh_s32_t plain_from;
    lh_s32_t y;

    lh_return_if(lh_null_eq(self) || lh_null_eq(rect) || lh_null_eq(radii) || lh_null_eq(paint));
    lh_return_if(lh_ui_paint_is_empty(paint));
    fit = lh_ui_radii_clamp(radii, rect);
    /* The common case keeps its own road: one colour and one radius are the
       backend's rounded fill, exactly as before there were corners and gradients. */
    if (lh_null_eq(lh_ui_paint_get_gradient(paint)) && lh_ui_radii_is_uniform(lh_addr_of(fit)))
    {
        lh_ui_canvas_fill_round_rect(self, rect, fit.top_left, lh_ui_paint_get_color(paint));
        return;
    }
    box = lh_ui_canvas_state_to_target(lh_addr_of(self->state), rect);
    cut = lh_ui_canvas_state_cut(lh_addr_of(self->state), lh_addr_of(box));
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    left = lh_ui_canvas_round_left(lh_addr_of(box));
    right = lh_ui_canvas_round_right(lh_addr_of(box));
    top = lh_ui_canvas_round_top(lh_addr_of(box));
    bottom = lh_ui_canvas_round_bottom(lh_addr_of(box));
    /* Only the rows and columns the clip shows: a long gradient list scrolled to its
       end does not colour the rows above the window, nor a wide one the columns off
       to the side. */
    first = lh_math_max(top, lh_ui_canvas_round_top(lh_addr_of(cut)));
    last = lh_math_min(bottom, lh_ui_canvas_round_bottom(lh_addr_of(cut)));
    shown_left = lh_math_max(left, lh_ui_canvas_round_left(lh_addr_of(cut)));
    shown_right = lh_math_min(right, lh_ui_canvas_round_right(lh_addr_of(cut)));
    lh_return_if(shown_right <= shown_left);
    if (lh_null_ne(lh_ui_paint_get_gradient(paint)))
    {
        lh_ui_gradient_sampler_init(lh_addr_of(sampler), lh_ui_paint_get_gradient(paint), lh_addr_of(box));
        colours = lh_addr_of(sampler);
    }
    /* Each corner is the corner of a box rounded all round by its radius, and only
       the pixels in that corner are asked about it. */
    lh_ui_radius_run_init(lh_addr_of(corners[0]), lh_addr_of(box), fit.top_left);
    lh_ui_radius_run_init(lh_addr_of(corners[1]), lh_addr_of(box), fit.top_right);
    lh_ui_radius_run_init(lh_addr_of(corners[2]), lh_addr_of(box), fit.bottom_left);
    lh_ui_radius_run_init(lh_addr_of(corners[3]), lh_addr_of(box), fit.bottom_right);
    plain_from = first;
    for (y = first; y < last; ++y)
    {
        const lh_bool_t upper = (2 * y + 1 < top + bottom) ? lh_bool_true : lh_bool_false;
        const lh_ui_scalar_t near = upper ? fit.top_left : fit.bottom_left;
        const lh_ui_scalar_t far = upper ? fit.top_right : fit.bottom_right;
        const lh_s32_t from_edge = upper ? y - top : bottom - 1 - y;
        const lh_s32_t zone_near =
            from_edge < lh_ui_scalar_ceil_s32(near) ? lh_math_min(lh_ui_scalar_ceil_s32(near), right - left) : 0;
        const lh_s32_t zone_far =
            from_edge < lh_ui_scalar_ceil_s32(far) ? lh_math_min(lh_ui_scalar_ceil_s32(far), right - left - zone_near)
                                                   : 0;

        if (zone_near == 0 && zone_far == 0)
        {
            continue;
        }
        /* A row a corner reaches: the rows before it that none did go out as one
           band, then this one whole, its corners at their coverage. */
        lh_ui_canvas_box_band(self, paint, colours, shown_left, shown_right, plain_from, y);
        plain_from = y + 1;
        lh_ui_canvas_box_row(self, paint, colours, shown_left, shown_right, y, left + zone_near,
                             lh_addr_of(corners[upper ? 0 : 2]), right - zone_far, lh_addr_of(corners[upper ? 1 : 3]));
    }
    lh_ui_canvas_box_band(self, paint, colours, shown_left, shown_right, plain_from, last);
}
