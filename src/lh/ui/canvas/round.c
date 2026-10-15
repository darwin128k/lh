/**
 * @file round.c
 * @brief Implementation of `lh/ui/canvas/round.h`.
 */

#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/radius.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_s32_t
lh_ui_canvas_round_left(const lh_ui_rect_t *rect)
{
    return lh_ui_scalar_floor_s32(lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(rect)));
}

lh_s32_t
lh_ui_canvas_round_top(const lh_ui_rect_t *rect)
{
    return lh_ui_scalar_floor_s32(lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(rect)));
}

lh_s32_t
lh_ui_canvas_round_right(const lh_ui_rect_t *rect)
{
    lh_ui_point_t corner;
    lh_ui_rect_far(rect, lh_addr_of(corner));
    return lh_ui_scalar_ceil_s32(lh_ui_point_get_x(lh_addr_of(corner)));
}

lh_s32_t
lh_ui_canvas_round_bottom(const lh_ui_rect_t *rect)
{
    lh_ui_point_t corner;
    lh_ui_rect_far(rect, lh_addr_of(corner));
    return lh_ui_scalar_ceil_s32(lh_ui_point_get_y(lh_addr_of(corner)));
}

lh_s32_t
lh_ui_canvas_round_near_end(lh_s32_t a, lh_s32_t b, lh_s32_t zone)
{
    return lh_math_min(a + zone, b);
}

lh_s32_t
lh_ui_canvas_round_far_start(lh_s32_t a, lh_s32_t b, lh_s32_t zone)
{
    return lh_math_max(b - zone, lh_ui_canvas_round_near_end(a, b, zone));
}

lh_bool_t
lh_ui_canvas_round_is_corner_row(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y)
{
    const lh_s32_t top = lh_ui_canvas_round_top(rect);
    const lh_s32_t bottom = lh_ui_canvas_round_bottom(rect);
    const lh_s32_t zone = lh_ui_scalar_ceil_s32(radius);

    return y < lh_ui_canvas_round_near_end(top, bottom, zone) || y >= lh_ui_canvas_round_far_start(top, bottom, zone)
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_ui_canvas_round_has_row(const lh_ui_rect_t *rect, lh_s32_t y)
{
    return y >= lh_ui_canvas_round_top(rect) && y < lh_ui_canvas_round_bottom(rect) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_canvas_round_full_span(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y, lh_s32_t *x0, lh_s32_t *x1)
{
    const lh_s32_t left = lh_ui_canvas_round_left(rect);
    const lh_s32_t right = lh_ui_canvas_round_right(rect);

    /* The span wholly inside the rounded rect is the arc's, not a radius-square
       around the corner: lh_ui_radius_full_span finds it with one square root
       for the row. Every pixel outside it has less than full coverage and goes
       through the per-pixel path, which is where the savings are — the square
       used to send the whole corner there, interior included. */
    lh_ui_radius_full_span(rect, radius, y, x0, x1);
    *x0 = lh_math_max(*x0, left);
    *x1 = lh_math_max(lh_math_min(*x1, right), *x0);
}

lh_void
lh_ui_canvas_fill_pixels(struct lh_ui_canvas *self, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                         const lh_ui_color_t *color)
{
    lh_ui_rect_t box;
    lh_return_if(w <= 0 || h <= 0);
    lh_ui_rect_init(lh_addr_of(box), x, y, w, h);
    lh_ui_canvas_fill_target_rect(self, lh_addr_of(box), color);
}

lh_void
lh_ui_canvas_fill_colors(struct lh_ui_canvas *self, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                         const lh_ui_color_t *colors, lh_ui_axis_t axis)
{
    const lh_s32_t count = axis == lh_ui_axis_horizontal ? w : h;
    lh_ui_rect_t box;
    lh_ui_rect_t cut;
    lh_s32_t at;
    lh_s32_t end;

    lh_return_if(w <= 0 || h <= 0 || lh_null_eq(self->backend));
    lh_ui_rect_init(lh_addr_of(box), x, y, w, h);
    lh_ui_canvas_state_cut(lh_addr_of(self->state), lh_addr_of(box), lh_addr_of(cut));
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    /* The box is drawn whole and cut by whoever clips, as a mask is
       (::lh_ui_canvas_can_fill_mask). */
    if (!lh_null_eq(self->backend->fill_colors) &&
        (!lh_ui_canvas_is_cutting(self) || lh_ui_canvas_can_send_whole(self, lh_addr_of(box))))
    {
        lh_ui_canvas_add_damage(self, lh_addr_of(cut));
        lh_return_if(self->backend->fill_colors(self->context, x, y, w, h, colors, axis));
    }
    for (at = 0; at < count; at = end)
    {
        for (end = at + 1; end < count && lh_ui_color_equals(colors + at, colors + end); ++end)
        {
        }
        if (lh_ui_color_get_a(colors + at) == 0U)
        {
            continue;
        }
        if (axis == lh_ui_axis_horizontal)
        {
            lh_ui_canvas_fill_pixels(self, x + at, y, end - at, h, colors + at);
        }
        else
        {
            lh_ui_canvas_fill_pixels(self, x, y + at, w, end - at, colors + at);
        }
    }
}

lh_void
lh_ui_canvas_fill_coverage_pixel(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                 lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color)
{
    const lh_byte_t cover = lh_ui_radius_coverage(rect, radius, x, y);
    lh_ui_color_t edge;
    lh_ui_color_with_coverage(color, cover, lh_addr_of(edge));
    lh_return_if(cover == 0U);
    lh_ui_canvas_fill_pixels(self, x, y, 1, 1, lh_addr_of(edge));
}

lh_void
lh_ui_canvas_fill_coverage_span(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_s32_t x;
    for (x = x0; x < x1; ++x)
    {
        lh_ui_canvas_fill_coverage_pixel(self, rect, radius, x, y, color);
    }
}

lh_void
lh_ui_canvas_fill_coverage_row(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                               lh_s32_t y, const lh_ui_color_t *color)
{
    const lh_s32_t left = lh_ui_canvas_round_left(rect);
    const lh_s32_t right = lh_ui_canvas_round_right(rect);
    const lh_s32_t zone = lh_ui_scalar_ceil_s32(radius);
    lh_ui_canvas_fill_coverage_span(self, rect, radius, left, lh_ui_canvas_round_near_end(left, right, zone), y, color);
    lh_ui_canvas_fill_coverage_span(self, rect, radius, lh_ui_canvas_round_far_start(left, right, zone), right, y, color);
}

lh_void
lh_ui_canvas_fill_round_band(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                             lh_s32_t y0, lh_s32_t y1, const lh_ui_color_t *color)
{
    const lh_s32_t left = lh_ui_canvas_round_left(rect);
    const lh_s32_t right = lh_ui_canvas_round_right(rect);
    const lh_s32_t zone = lh_ui_scalar_ceil_s32(radius);
    const lh_s32_t near_end = lh_ui_canvas_round_near_end(left, right, zone);
    lh_s32_t y;
    lh_ui_canvas_fill_pixels(self, near_end, y0, lh_ui_canvas_round_far_start(left, right, zone) - near_end, y1 - y0,
                             color);
    for (y = y0; y < y1; ++y)
    {
        lh_ui_canvas_fill_coverage_row(self, rect, radius, y, color);
    }
}

lh_void
lh_ui_canvas_fill_round_rect_by_rects(struct lh_ui_canvas *self, const lh_ui_rect_t *rect,
                                      lh_ui_scalar_t radius, const lh_ui_color_t *color)
{
    const lh_s32_t top = lh_ui_canvas_round_top(rect);
    const lh_s32_t bottom = lh_ui_canvas_round_bottom(rect);
    const lh_s32_t zone = lh_ui_scalar_ceil_s32(radius);
    const lh_s32_t top_end = lh_ui_canvas_round_near_end(top, bottom, zone);
    const lh_s32_t bottom_start = lh_ui_canvas_round_far_start(top, bottom, zone);
    lh_ui_canvas_fill_round_band(self, rect, radius, top, top_end, color);
    lh_ui_canvas_fill_pixels(self, lh_ui_canvas_round_left(rect), top_end,
                             lh_ui_canvas_round_right(rect) - lh_ui_canvas_round_left(rect), bottom_start - top_end,
                             color);
    lh_ui_canvas_fill_round_band(self, rect, radius, bottom_start, bottom, color);
}
