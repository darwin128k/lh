/**
 * @file round.c
 * @brief Implementation of `lh/gl/canvas/round.h`.
 */

#include <lh/core/math.h>
#include <lh/gl/canvas.h>
#include <lh/gl/canvas/round.h>
#include <lh/gl/radius.h>
#include <lh/core/util/return.h>

lh_s32_t
lh_gl_canvas_round_left(const lh_gl_rect_t *rect)
{
    return lh_gl_scalar_floor_s32(lh_gl_point_get_x(lh_gl_rect_get_origin(rect)));
}

lh_s32_t
lh_gl_canvas_round_top(const lh_gl_rect_t *rect)
{
    return lh_gl_scalar_floor_s32(lh_gl_point_get_y(lh_gl_rect_get_origin(rect)));
}

lh_s32_t
lh_gl_canvas_round_right(const lh_gl_rect_t *rect)
{
    lh_gl_point_t corner;
    lh_gl_rect_far(rect, lh_addr_of(corner));
    return lh_gl_scalar_ceil_s32(lh_gl_point_get_x(lh_addr_of(corner)));
}

lh_s32_t
lh_gl_canvas_round_bottom(const lh_gl_rect_t *rect)
{
    lh_gl_point_t corner;
    lh_gl_rect_far(rect, lh_addr_of(corner));
    return lh_gl_scalar_ceil_s32(lh_gl_point_get_y(lh_addr_of(corner)));
}

lh_s32_t
lh_gl_canvas_round_near_end(lh_s32_t a, lh_s32_t b, lh_s32_t zone)
{
    return lh_math_min(a + zone, b);
}

lh_s32_t
lh_gl_canvas_round_far_start(lh_s32_t a, lh_s32_t b, lh_s32_t zone)
{
    return lh_math_max(b - zone, lh_gl_canvas_round_near_end(a, b, zone));
}

lh_bool_t
lh_gl_canvas_round_is_corner_row(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_s32_t y)
{
    const lh_s32_t top = lh_gl_canvas_round_top(rect);
    const lh_s32_t bottom = lh_gl_canvas_round_bottom(rect);
    const lh_s32_t zone = lh_gl_scalar_ceil_s32(radius);

    return y < lh_gl_canvas_round_near_end(top, bottom, zone) ||
                   y >= lh_gl_canvas_round_far_start(top, bottom, zone)
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_gl_canvas_round_has_row(const lh_gl_rect_t *rect, lh_s32_t y)
{
    return y >= lh_gl_canvas_round_top(rect) && y < lh_gl_canvas_round_bottom(rect) ? lh_bool_true
                                                                                    : lh_bool_false;
}

lh_void
lh_gl_canvas_round_full_span(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_s32_t y,
                             lh_s32_t *x0, lh_s32_t *x1)
{
    const lh_s32_t left = lh_gl_canvas_round_left(rect);
    const lh_s32_t right = lh_gl_canvas_round_right(rect);

    /* The span wholly inside the rounded rect is the arc's, not a radius-square
       around the corner: lh_gl_radius_full_span finds it with one square root
       for the row. Every pixel outside it has less than full coverage and goes
       through the per-pixel path, which is where the savings are — the square
       used to send the whole corner there, interior included. */
    lh_gl_radius_full_span(rect, radius, y, x0, x1);
    lh_ptr_deref(x0) = lh_math_max(lh_ptr_deref(x0), left);
    lh_ptr_deref(x1) = lh_math_max(lh_math_min(lh_ptr_deref(x1), right), lh_ptr_deref(x0));
}

lh_void
lh_gl_canvas_fill_pixels(struct lh_gl_canvas *self, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                         const lh_gl_color_t *color)
{
    lh_gl_rect_t box;
    lh_return_if(w <= 0 || h <= 0);
    lh_gl_rect_init(lh_addr_of(box), x, y, w, h);
    lh_gl_canvas_fill_target_rect(self, lh_addr_of(box), color);
}

lh_void
lh_gl_canvas_fill_colors(struct lh_gl_canvas *self, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                         const lh_gl_color_t *colors, lh_gl_axis_t axis)
{
    const lh_s32_t count = axis == lh_gl_axis_horizontal ? w : h;

    lh_return_if(w <= 0 || h <= 0 || lh_null_eq(lh_gl_canvas_get_backend(self)));
    lh_gl_rect_t box;
    lh_gl_rect_init(lh_addr_of(box), x, y, w, h);
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state_mut(self), lh_addr_of(box), lh_addr_of(cut));
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(cut)));
    /* The box is drawn whole and cut by whoever clips, as a mask is
       (::lh_gl_canvas_can_fill_mask). */
    if (!lh_null_eq(lh_gl_canvas_backend_get_fill_colors_fn(lh_gl_canvas_get_backend(self))) &&
        (!lh_gl_canvas_is_cutting(self) || lh_gl_canvas_can_send_whole(self, lh_addr_of(box))))
    {
        lh_gl_canvas_add_damage(self, lh_addr_of(cut));
        lh_return_if(lh_gl_canvas_backend_get_fill_colors_fn(lh_gl_canvas_get_backend(self))(
            lh_gl_canvas_get_context(self), x, y, w, h, colors, axis));
    }
    lh_s32_t end;
    for (lh_s32_t at = 0; at < count; at = end)
    {
        for (end = at + 1; end < count && lh_gl_color_equals(colors + at, colors + end); ++end)
        {
        }
        if (lh_gl_color_get_a(colors + at) == 0U)
        {
            continue;
        }
        if (axis == lh_gl_axis_horizontal)
        {
            lh_gl_canvas_fill_pixels(self, x + at, y, end - at, h, colors + at);
        }
        else
        {
            lh_gl_canvas_fill_pixels(self, x, y + at, w, end - at, colors + at);
        }
    }
}

lh_void
lh_gl_canvas_fill_coverage_pixel(struct lh_gl_canvas *self, const lh_gl_rect_t *rect,
                                 lh_gl_scalar_t radius, lh_s32_t x, lh_s32_t y,
                                 const lh_gl_color_t *color)
{
    const lh_byte_t cover = lh_gl_radius_coverage(rect, radius, x, y);
    lh_gl_color_t edge;
    lh_gl_color_with_coverage(color, cover, lh_addr_of(edge));
    lh_return_if(cover == 0U);
    lh_gl_canvas_fill_pixels(self, x, y, 1, 1, lh_addr_of(edge));
}

lh_void
lh_gl_canvas_fill_coverage_span(struct lh_gl_canvas *self, const lh_gl_rect_t *rect,
                                lh_gl_scalar_t radius, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                                const lh_gl_color_t *color)
{
    for (lh_s32_t x = x0; x < x1; ++x)
    {
        lh_gl_canvas_fill_coverage_pixel(self, rect, radius, x, y, color);
    }
}

lh_void
lh_gl_canvas_fill_coverage_row(struct lh_gl_canvas *self, const lh_gl_rect_t *rect,
                               lh_gl_scalar_t radius, lh_s32_t y, const lh_gl_color_t *color)
{
    const lh_s32_t left = lh_gl_canvas_round_left(rect);
    const lh_s32_t right = lh_gl_canvas_round_right(rect);
    const lh_s32_t zone = lh_gl_scalar_ceil_s32(radius);
    lh_gl_canvas_fill_coverage_span(self, rect, radius, left,
                                    lh_gl_canvas_round_near_end(left, right, zone), y, color);
    lh_gl_canvas_fill_coverage_span(
        self, rect, radius, lh_gl_canvas_round_far_start(left, right, zone), right, y, color);
}

lh_void
lh_gl_canvas_fill_round_band(struct lh_gl_canvas *self, const lh_gl_rect_t *rect,
                             lh_gl_scalar_t radius, lh_s32_t y0, lh_s32_t y1,
                             const lh_gl_color_t *color)
{
    const lh_s32_t left = lh_gl_canvas_round_left(rect);
    const lh_s32_t right = lh_gl_canvas_round_right(rect);
    const lh_s32_t zone = lh_gl_scalar_ceil_s32(radius);
    const lh_s32_t near_end = lh_gl_canvas_round_near_end(left, right, zone);
    lh_gl_canvas_fill_pixels(self, near_end, y0,
                             lh_gl_canvas_round_far_start(left, right, zone) - near_end, y1 - y0,
                             color);
    for (lh_s32_t y = y0; y < y1; ++y)
    {
        lh_gl_canvas_fill_coverage_row(self, rect, radius, y, color);
    }
}

lh_void
lh_gl_canvas_fill_round_rect_by_rects(struct lh_gl_canvas *self, const lh_gl_rect_t *rect,
                                      lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    const lh_s32_t top = lh_gl_canvas_round_top(rect);
    const lh_s32_t bottom = lh_gl_canvas_round_bottom(rect);
    const lh_s32_t zone = lh_gl_scalar_ceil_s32(radius);
    const lh_s32_t top_end = lh_gl_canvas_round_near_end(top, bottom, zone);
    const lh_s32_t bottom_start = lh_gl_canvas_round_far_start(top, bottom, zone);
    lh_gl_canvas_fill_round_band(self, rect, radius, top, top_end, color);
    lh_gl_canvas_fill_pixels(self, lh_gl_canvas_round_left(rect), top_end,
                             lh_gl_canvas_round_right(rect) - lh_gl_canvas_round_left(rect),
                             bottom_start - top_end, color);
    lh_gl_canvas_fill_round_band(self, rect, radius, bottom_start, bottom, color);
}
