/**
 * @file box.c
 * @brief ::lh_ui_canvas_fill_box: a box with a radius for each corner and a paint that
 *        may be a gradient.
 *
 * A solid box with one radius is ::lh_ui_canvas_fill_round_rect, and goes there: the
 * backend's own rounded fill, unchanged. Anything else is drawn here with the canvas's
 * own sends, so every backend draws it the same: the straight part of each row as
 * spans, the corners pixel by pixel at the coverage of the corner they are in
 * (::lh_ui_radii_get_at). A gradient that runs straight down is one colour a row and
 * one straight across one colour a column, so those go out as spans too; any other
 * angle and a radial one are a colour a pixel, with runs of one colour joined.
 */

#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/box.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint.h>
#include <lh/ui/radii.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* The colour of @p paint at the pixel (@p x, @p y) of @p box. */
static lh_ui_color_t
lh_ui_canvas_box_color(const lh_ui_paint_t *paint, const lh_ui_rect_t *box, lh_s32_t x, lh_s32_t y)
{
    const lh_ui_gradient_t *gradient = lh_ui_paint_get_gradient(paint);

    lh_return_if(lh_null_eq(gradient), *lh_ui_paint_get_color(paint));
    return lh_ui_gradient_color_at(gradient, lh_ui_gradient_frac_at(gradient, box, x, y));
}

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

/* True when the colour of @p paint does not change down a column. */
static lh_bool_t
lh_ui_canvas_box_columns_are_one_colour(const lh_ui_paint_t *paint)
{
    const lh_ui_gradient_t *gradient = lh_ui_paint_get_gradient(paint);

    lh_return_if(lh_null_eq(gradient), lh_bool_true);
    return lh_ui_gradient_get_shape(gradient) == lh_ui_gradient_shape_linear &&
                   (lh_ui_gradient_get_angle(gradient) % 180) == 90
               ? lh_bool_true
               : lh_bool_false;
}

/* Pixels @p x0 .. @p x1 - 1 of row @p y, wholly inside the box: one span when the row is
   one colour, else runs of one colour. */
static lh_void
lh_ui_canvas_box_span(lh_ui_canvas_t *self, const lh_ui_paint_t *paint, const lh_ui_rect_t *box, lh_s32_t x0,
                      lh_s32_t x1, lh_s32_t y)
{
    lh_s32_t x;
    lh_s32_t run;
    lh_ui_color_t colour;

    lh_return_if(x1 <= x0);
    if (lh_ui_canvas_box_rows_are_one_colour(paint))
    {
        colour = lh_ui_canvas_box_color(paint, box, x0, y);
        lh_ui_canvas_fill_pixels(self, x0, y, x1 - x0, 1, lh_addr_of(colour));
        return;
    }
    run = x0;
    colour = lh_ui_canvas_box_color(paint, box, x0, y);
    for (x = x0 + 1; x <= x1; ++x)
    {
        lh_ui_color_t next;

        if (x < x1)
        {
            next = lh_ui_canvas_box_color(paint, box, x, y);
            if (lh_ui_color_equals(lh_addr_of(next), lh_addr_of(colour)))
            {
                continue;
            }
        }
        lh_ui_canvas_fill_pixels(self, run, y, x - run, 1, lh_addr_of(colour));
        if (x < x1)
        {
            run = x;
            colour = next;
        }
    }
}

/* Rows @p y0 .. @p y1 - 1, which no corner reaches: one box when the paint is one
   colour, a column a colour when it changes only across, else row by row. */
static lh_void
lh_ui_canvas_box_band(lh_ui_canvas_t *self, const lh_ui_paint_t *paint, const lh_ui_rect_t *box, lh_s32_t x0,
                      lh_s32_t x1, lh_s32_t y0, lh_s32_t y1)
{
    lh_s32_t x;
    lh_s32_t y;
    lh_ui_color_t colour;

    lh_return_if(y1 <= y0 || x1 <= x0);
    if (lh_null_eq(lh_ui_paint_get_gradient(paint)))
    {
        lh_ui_canvas_fill_pixels(self, x0, y0, x1 - x0, y1 - y0, lh_ui_paint_get_color(paint));
        return;
    }
    if (lh_ui_canvas_box_columns_are_one_colour(paint))
    {
        for (x = x0; x < x1; ++x)
        {
            colour = lh_ui_canvas_box_color(paint, box, x, y0);
            lh_ui_canvas_fill_pixels(self, x, y0, 1, y1 - y0, lh_addr_of(colour));
        }
        return;
    }
    for (y = y0; y < y1; ++y)
    {
        lh_ui_canvas_box_span(self, paint, box, x0, x1, y);
    }
}

/* The corner pixels @p x0 .. @p x1 - 1 of row @p y, each at its coverage by @p radius. */
static lh_void
lh_ui_canvas_box_corner(lh_ui_canvas_t *self, const lh_ui_paint_t *paint, const lh_ui_rect_t *box,
                        lh_ui_scalar_t radius, lh_s32_t x0, lh_s32_t x1, lh_s32_t y)
{
    lh_s32_t x;

    for (x = x0; x < x1; ++x)
    {
        const lh_ui_color_t colour = lh_ui_canvas_box_color(paint, box, x, y);

        lh_ui_canvas_fill_coverage_pixel(self, box, radius, x, y, lh_addr_of(colour));
    }
}

lh_void
lh_ui_canvas_fill_box(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_radii_t *radii,
                      const lh_ui_paint_t *paint)
{
    lh_ui_radii_t fit;
    lh_ui_rect_t box;
    lh_ui_rect_t cut;
    lh_s32_t left;
    lh_s32_t right;
    lh_s32_t top;
    lh_s32_t bottom;
    lh_s32_t first;
    lh_s32_t last;
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
    /* Only the rows the clip shows: a long gradient list scrolled to its end
       does not colour the rows above the window. */
    first = lh_math_max(top, lh_ui_canvas_round_top(lh_addr_of(cut)));
    last = lh_math_min(bottom, lh_ui_canvas_round_bottom(lh_addr_of(cut)));
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
           band, then this one, its corners pixel by pixel. */
        lh_ui_canvas_box_band(self, paint, lh_addr_of(box), left, right, plain_from, y);
        plain_from = y + 1;
        lh_ui_canvas_box_corner(self, paint, lh_addr_of(box), near, left, left + zone_near, y);
        lh_ui_canvas_box_span(self, paint, lh_addr_of(box), left + zone_near, right - zone_far, y);
        lh_ui_canvas_box_corner(self, paint, lh_addr_of(box), far, right - zone_far, right, y);
    }
    lh_ui_canvas_box_band(self, paint, lh_addr_of(box), left, right, plain_from, last);
}
