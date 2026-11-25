/**
 * @file fill.c
 * @brief Implementation of `lh/gl/canvas/sw.h`: the fills the backend table points at: clear, a
 * rect, a rounded rect, a mask, a run of colours.
 */

#include <lh/gl/canvas/sw.h>

#include <lh/gl/canvas/round.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_sw_clear(lh_ptr context, const lh_gl_color_t *color)
{
    lh_gl_pixmap_clear(lh_addr_of(lh_gl_canvas_sw_from(context)->pixmap), color);
}

lh_void
lh_gl_canvas_sw_fill_rect(lh_ptr context, const lh_gl_rect_t *rect, const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t x0 = lh_gl_canvas_sw_cut_x0(self, lh_gl_canvas_round_left(rect));
    const lh_s32_t y0 = lh_gl_canvas_sw_cut_y0(self, lh_gl_canvas_round_top(rect));
    const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, lh_gl_canvas_round_right(rect));
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, lh_gl_canvas_round_bottom(rect));

    if (lh_gl_canvas_sw_is_rounded(self))
    {
        lh_gl_canvas_sw_fill_rows(self, x0, y0, x1, y1, color);
        return;
    }
    lh_gl_pixmap_fill_box(lh_gl_canvas_sw_get_pixmap_mut(self), x0, y0, x1, y1, color);
}

lh_bool_t
lh_gl_canvas_sw_fill_mask(lh_ptr context, const lh_gl_point_t *origin, const lh_gl_mask_t *mask,
                          const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t x0 = lh_gl_scalar_floor_s32(lh_gl_point_get_x(origin));
    const lh_s32_t y0 = lh_gl_scalar_floor_s32(lh_gl_point_get_y(origin));
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, y0 + lh_gl_mask_get_height(mask));

    /* A rounded cut is a coverage a pixel of its own, which the run path
       multiplies in; without one, the mask's coverages go to the pixels straight
       from the register. */
    if (lh_gl_canvas_sw_is_rounded(self))
    {
        for (lh_s32_t y = lh_gl_canvas_sw_cut_y0(self, y0); y < y1; ++y)
        {
            lh_gl_canvas_sw_mask_row(self, mask, x0, y0, y, color);
        }
        return lh_bool_true;
    }
    for (lh_s32_t y = lh_gl_canvas_sw_cut_y0(self, y0); y < y1; ++y)
    {
        lh_gl_canvas_sw_cover_row(self, mask, x0, y0, y, color);
    }
    return lh_bool_true;
}

lh_bool_t
lh_gl_canvas_sw_fill_colors(lh_ptr context, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                            const lh_gl_color_t *colors, lh_gl_axis_t axis)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t x0 = lh_gl_canvas_sw_cut_x0(self, x);
    const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, x + w);
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, y + h);

    /* A rounded cut is a coverage a pixel on top of the colours' own alpha; the
       canvas sends the box through fill_rect, which knows it. */
    lh_return_if(lh_gl_canvas_sw_is_rounded(self), lh_bool_false);
    lh_return_if(x1 <= x0, lh_bool_true);
    for (lh_s32_t row = lh_gl_canvas_sw_cut_y0(self, y); row < y1; ++row)
    {
        if (axis == lh_gl_axis_horizontal)
        {
            lh_gl_pixmap_fill_colors_span(lh_gl_canvas_sw_get_pixmap_mut(self), x0, x1, row,
                                          colors + (x0 - x));
        }
        else
        {
            lh_gl_pixmap_fill_span(lh_gl_canvas_sw_get_pixmap_mut(self), x0, x1, row,
                                   colors + (row - y));
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_gl_canvas_sw_fill_round_rect(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t y0 = lh_gl_canvas_sw_cut_y0(self, lh_gl_canvas_round_top(rect));
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, lh_gl_canvas_round_bottom(rect));
    /* Only the rows the arc actually reaches need a row at a time. Everywhere
       between them lh_gl_canvas_round_full_span answers the whole width, so that
       band is a plain rectangle and is drawn as one — the same single
       lh_gl_pixmap_fill_box a square rect gets, instead of one round_row per
       row. Measured on this machine that per-row path costs about 120 ns before
       it stores a single pixel, so a 240x160 r8 rectangle spent 20 us of its 24
       on 144 rows whose coverage is a flat 255. */
    const lh_s32_t zone = lh_gl_scalar_ceil_s32(radius);
    const lh_s32_t near = lh_gl_canvas_round_near_end(y0, y1, zone);
    const lh_s32_t far = lh_gl_canvas_round_far_start(y0, y1, zone);

    if (near < far)
    {
        lh_gl_rect_t middle;

        lh_gl_rect_init(lh_addr_of(middle), lh_gl_canvas_round_left(rect), near,
                        lh_gl_canvas_round_right(rect) - lh_gl_canvas_round_left(rect), far - near);
        lh_gl_canvas_sw_fill_rect(context, lh_addr_of(middle), color);
    }
    lh_gl_canvas_sw_round_band(self, rect, radius, y0, near, color);
    lh_gl_canvas_sw_round_band(self, rect, radius, lh_gl_canvas_sw_cut_y0(self, far), y1, color);
    return lh_bool_true;
}
