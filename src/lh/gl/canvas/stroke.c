/** @file stroke.c
 * @brief Rounded borders as outer coverage minus inner coverage.
 */
#include <lh/gl/canvas/stroke.h>

#include <lh/gl/canvas/box.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

#define LH_GL_CANVAS_STROKE_RUN 64

lh_void
lh_gl_canvas_stroke_run(lh_gl_canvas_t *self, const struct lh_gl_radius_run *outer,
                        const struct lh_gl_radius_run *inner, const lh_gl_color_t *color,
                        lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_s32_t middle)
{
    while (x0 < x1)
    {
        const lh_s32_t side = x0 < middle ? 0 : 1;
        const lh_s32_t end =
            lh_math_min(lh_math_min(x0 + LH_GL_CANVAS_STROKE_RUN, x1), side == 0 ? middle : x1);
        lh_byte_t outside[LH_GL_CANVAS_STROKE_RUN];
        lh_gl_radius_run_row(lh_addr_of(outer[side]), x0, end, y, outside);
        lh_byte_t inside[LH_GL_CANVAS_STROKE_RUN];
        if (lh_null_ne(inner))
        {
            lh_gl_radius_run_row(lh_addr_of(inner[side]), x0, end, y, inside);
        }
        lh_gl_color_t colors[LH_GL_CANVAS_STROKE_RUN];
        for (lh_s32_t x = x0; x < end; ++x)
        {
            const lh_byte_t coverage = lh_cast_static(
                lh_byte_t,
                lh_math_max(0, outside[x - x0] - (lh_null_eq(inner) ? 0 : inside[x - x0])));
            lh_gl_color_with_coverage(color, coverage, lh_addr_of(colors[x - x0]));
        }
        lh_gl_canvas_fill_colors(self, x0, y, end - x0, 1, colors, lh_gl_axis_horizontal);
        x0 = end;
    }
}

lh_void
lh_gl_canvas_stroke_box(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, const lh_gl_radii_t *radii,
                        lh_gl_scalar_t width, const lh_gl_color_t *color)
{
    lh_return_if(width <= lh_gl_scalar(0) || lh_gl_rect_is_empty(rect));
    lh_gl_rect_t box;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state(self), rect, lh_addr_of(box));
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state(self), lh_addr_of(box), lh_addr_of(cut));
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(cut)));
    lh_gl_radii_t fit;
    lh_gl_radii_clamp(radii, lh_addr_of(box), lh_addr_of(fit));
    struct lh_gl_radius_run outer[4];
    lh_gl_canvas_box_corners_init(lh_addr_of(box), lh_addr_of(fit), outer);
    lh_gl_rect_t hole;
    lh_gl_rect_inset(lh_addr_of(box), width, width, lh_addr_of(hole));
    const lh_bool_t has_hole = !lh_gl_rect_is_empty(lh_addr_of(hole));
    lh_gl_radii_t inner_radii;
    lh_gl_radii_init(
        lh_addr_of(inner_radii),
        lh_math_max(lh_gl_radii_get_top_left(lh_addr_of(fit)) - width, lh_gl_scalar(0)),
        lh_math_max(lh_gl_radii_get_top_right(lh_addr_of(fit)) - width, lh_gl_scalar(0)),
        lh_math_max(lh_gl_radii_get_bottom_right(lh_addr_of(fit)) - width, lh_gl_scalar(0)),
        lh_math_max(lh_gl_radii_get_bottom_left(lh_addr_of(fit)) - width, lh_gl_scalar(0)));
    struct lh_gl_radius_run inner[4];
    if (has_hole)
    {
        lh_gl_canvas_box_corners_init(lh_addr_of(hole), lh_addr_of(inner_radii), inner);
    }
    const lh_gl_scalar_t maximum =
        lh_math_max(lh_math_max(lh_gl_radii_get_top_left(lh_addr_of(inner_radii)),
                                lh_gl_radii_get_top_right(lh_addr_of(inner_radii))),
                    lh_math_max(lh_gl_radii_get_bottom_left(lh_addr_of(inner_radii)),
                                lh_gl_radii_get_bottom_right(lh_addr_of(inner_radii))));
    const lh_s32_t x0 = lh_gl_canvas_round_left(lh_addr_of(cut));
    const lh_s32_t x1 = lh_gl_canvas_round_right(lh_addr_of(cut));
    const lh_s32_t middle_x =
        (lh_gl_canvas_round_left(lh_addr_of(box)) + lh_gl_canvas_round_right(lh_addr_of(box))) / 2;
    const lh_s32_t middle_y =
        (lh_gl_canvas_round_top(lh_addr_of(box)) + lh_gl_canvas_round_bottom(lh_addr_of(box))) / 2;
    /* Straight sides are two rectangles, not two one-pixel sends per row.
       Only the corner bands need per-pixel ring coverage. */
    const lh_gl_scalar_t outer_maximum =
        lh_math_max(lh_math_max(lh_gl_radii_get_top_left(lh_addr_of(fit)),
                                lh_gl_radii_get_top_right(lh_addr_of(fit))),
                    lh_math_max(lh_gl_radii_get_bottom_left(lh_addr_of(fit)),
                                lh_gl_radii_get_bottom_right(lh_addr_of(fit))));
    const lh_s32_t zone = lh_gl_scalar_ceil_s32(lh_math_max(outer_maximum, width));
    const lh_s32_t band0 = lh_math_max(lh_gl_canvas_round_top(lh_addr_of(cut)),
                                       lh_gl_canvas_round_top(lh_addr_of(box)) + zone);
    const lh_s32_t band1 = lh_math_min(lh_gl_canvas_round_bottom(lh_addr_of(cut)),
                                       lh_gl_canvas_round_bottom(lh_addr_of(box)) - zone);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(box));
    const lh_gl_size_t *size = lh_gl_rect_get_size(lh_addr_of(box));
    const lh_bool_t pixel_edges =
        lh_gl_point_get_x(origin) ==
            lh_gl_scalar_of(lh_gl_scalar_floor_s32(lh_gl_point_get_x(origin))) &&
        lh_gl_point_get_y(origin) ==
            lh_gl_scalar_of(lh_gl_scalar_floor_s32(lh_gl_point_get_y(origin))) &&
        lh_gl_size_get_width(size) ==
            lh_gl_scalar_of(lh_gl_scalar_floor_s32(lh_gl_size_get_width(size))) &&
        lh_gl_size_get_height(size) ==
            lh_gl_scalar_of(lh_gl_scalar_floor_s32(lh_gl_size_get_height(size))) &&
        width == lh_gl_scalar_of(lh_gl_scalar_floor_s32(width));
    const lh_bool_t band = has_hole && band1 > band0 && pixel_edges;
    if (band)
    {
        const lh_s32_t left = lh_math_clamp(lh_gl_canvas_round_left(lh_addr_of(hole)), x0, x1);
        const lh_s32_t right = lh_math_clamp(lh_gl_canvas_round_right(lh_addr_of(hole)), left, x1);
        lh_gl_canvas_fill_pixels(self, x0, band0, left - x0, band1 - band0, color);
        lh_gl_canvas_fill_pixels(self, right, band0, x1 - right, band1 - band0, color);
    }
    for (lh_s32_t y = lh_gl_canvas_round_top(lh_addr_of(cut));
         y < lh_gl_canvas_round_bottom(lh_addr_of(cut)); ++y)
    {
        if (band && y == band0)
        {
            y = band1 - 1;
            continue;
        }
        const lh_s32_t pair = y < middle_y ? 0 : 2;
        lh_s32_t inside0 = x1;
        lh_s32_t inside1 = x1;
        if (has_hole && lh_gl_radius_axis_distance(
                            lh_gl_radius_pixel_center(y),
                            lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(hole))),
                            lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(hole))), 0) >= 0)
        {
            if (pixel_edges)
            {
                lh_gl_radius_full_span(lh_addr_of(hole), maximum, y, lh_addr_of(inside0),
                                       lh_addr_of(inside1));
            }
            else
            {
                inside0 = lh_gl_scalar_ceil_s32(
                    lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(hole))) + maximum);
                inside1 = lh_gl_scalar_floor_s32(
                    lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(hole))) +
                    lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(hole))) - maximum);
            }
            inside0 = lh_math_clamp(inside0, x0, x1);
            inside1 = lh_math_clamp(inside1, inside0, x1);
        }
        lh_gl_canvas_stroke_run(self, lh_addr_of(outer[pair]),
                                has_hole ? lh_addr_of(inner[pair]) : lh_null, color, x0, inside0, y,
                                middle_x);
        lh_gl_canvas_stroke_run(self, lh_addr_of(outer[pair]),
                                has_hole ? lh_addr_of(inner[pair]) : lh_null, color, inside1, x1, y,
                                middle_x);
    }
}
