/**
 * @file round.c
 * @brief Implementation of `lh/ui/canvas/round.h`.
 */

#include <lh/math.h>
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
    const lh_ui_point_t corner = lh_ui_rect_far(rect);
    return lh_ui_scalar_ceil_s32(lh_ui_point_get_x(lh_addr_of(corner)));
}

lh_s32_t
lh_ui_canvas_round_bottom(const lh_ui_rect_t *rect)
{
    const lh_ui_point_t corner = lh_ui_rect_far(rect);
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
lh_ui_canvas_fill_coverage_pixel(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                 lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color)
{
    const lh_byte_t cover = lh_ui_radius_coverage(rect, radius, x, y);
    const lh_ui_color_t edge = lh_ui_color_with_coverage(color, cover);
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
