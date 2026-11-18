/**
 * @file box.c
 * @brief Implementation of `lh/gx/box.h`.
 */

#include <lh/gx/box.h>

#include <lh/core/pixel/argb.h>
#include <lh/core/pixel/round.h>
#include <lh/gx/class/initializer.h>

const lh_gx_class_t lh_gx_box_class =
    lh_gx_class_initializer(lh_gx_box_paint, lh_gx_box_covers, lh_null);

lh_s32_t
lh_gx_box_get_radius_px(const lh_gx_style_t *style, const lh_math_rect_t *outer)
{
    const lh_math_size_t *size = lh_math_rect_get_size(outer);
    const lh_s32_t half =
        lh_math_min(lh_math_size_get_width(size), lh_math_size_get_height(size)) / 2;

    return lh_math_clamp(lh_cast_static(lh_s32_t, lh_gx_style_get_radius(style)), 0,
                         lh_math_min(half, LH_GX_BOX_RADIUS_MAX));
}

lh_s32_t
lh_gx_box_get_border_px(const lh_gx_style_t *style, const lh_math_rect_t *outer)
{
    lh_return_if(lh_pixel_argb_get_channel(lh_gx_style_get_border(style), 24) == 0U, 0);
    const lh_math_size_t *size = lh_math_rect_get_size(outer);
    const lh_s32_t half =
        lh_math_min(lh_math_size_get_width(size), lh_math_size_get_height(size)) / 2;

    return lh_math_clamp(lh_cast_static(lh_s32_t, lh_gx_style_get_border_width(style)), 0, half);
}

lh_void
lh_gx_box_fill_px(const lh_gx_paint_t *paint, lh_s32_t x, lh_s32_t y, lh_s32_t width,
                  lh_s32_t height, lh_gx_color_t color)
{
    lh_return_if(width <= 0 || height <= 0 || lh_pixel_argb_get_channel(color, 24) == 0U);
    lh_gx_rect_t rect;

    lh_gx_rect_init(lh_addr_of(rect), lh_cast_static(lh_gx_scalar_t, x),
                    lh_cast_static(lh_gx_scalar_t, y), lh_cast_static(lh_gx_scalar_t, width),
                    lh_cast_static(lh_gx_scalar_t, height));
    lh_gx_paint_fill_rect(paint, lh_addr_of(rect), color);
}

lh_void
lh_gx_box_paint_corner_row(const lh_gx_paint_t *paint, const lh_math_rect_t *outer, lh_s32_t radius,
                           lh_s32_t border, const lh_gx_style_t *style, lh_s32_t x, lh_s32_t y,
                           lh_s32_t count)
{
    const lh_math_point_t *at = lh_math_rect_get_origin(outer);
    const lh_math_size_t *size = lh_math_rect_get_size(outer);
    const lh_s64_t lo_x =
        lh_cast_static(lh_s64_t, lh_math_point_get_x(at)) * LH_PIXEL_ROUND_SUBPIXEL;
    const lh_s64_t lo_y =
        lh_cast_static(lh_s64_t, lh_math_point_get_y(at)) * LH_PIXEL_ROUND_SUBPIXEL;
    const lh_s64_t hi_x =
        lo_x + lh_cast_static(lh_s64_t, lh_math_size_get_width(size)) * LH_PIXEL_ROUND_SUBPIXEL;
    const lh_s64_t hi_y =
        lo_y + lh_cast_static(lh_s64_t, lh_math_size_get_height(size)) * LH_PIXEL_ROUND_SUBPIXEL;
    const lh_s64_t inset = lh_cast_static(lh_s64_t, border) * LH_PIXEL_ROUND_SUBPIXEL;
    const lh_s64_t r = lh_cast_static(lh_s64_t, radius) * LH_PIXEL_ROUND_SUBPIXEL;
    const lh_s64_t inner_r = lh_math_max(r - inset, 0);
    lh_byte_t fill_cover[LH_GX_BOX_RADIUS_MAX];
    lh_byte_t border_cover[LH_GX_BOX_RADIUS_MAX];

    for (lh_s32_t i = 0; i < count; ++i)
    {
        const lh_byte_t outer_cover = lh_pixel_round_coverage(lo_x, lo_y, hi_x, hi_y, r, x + i, y);
        const lh_byte_t inner_cover = lh_pixel_round_coverage(
            lo_x + inset, lo_y + inset, hi_x - inset, hi_y - inset, inner_r, x + i, y);

        fill_cover[i] = inner_cover;
        border_cover[i] = lh_cast_static(lh_byte_t, lh_math_max(outer_cover - inner_cover, 0));
    }
    lh_gx_point_t origin;

    lh_gx_point_init(lh_addr_of(origin), lh_cast_static(lh_gx_scalar_t, x),
                     lh_cast_static(lh_gx_scalar_t, y));
    lh_memory_bounds_t bits;

    lh_memory_bounds_init(lh_addr_of(bits), fill_cover, lh_cast_static(lh_usize_t, count));
    lh_gx_pixmap_t mask;
    lh_gx_pixmap_init(lh_addr_of(mask), lh_addr_of(bits), lh_cast_static(lh_u32_t, count), 1U,
                      lh_gx_pixmap_format_a8);
    lh_gx_paint_fill_mask(paint, lh_addr_of(mask), origin, lh_gx_style_get_fill(style));
    lh_return_if(border == 0);
    lh_memory_bounds_init(lh_addr_of(bits), border_cover, lh_cast_static(lh_usize_t, count));
    lh_gx_paint_fill_mask(paint, lh_addr_of(mask), origin, lh_gx_style_get_border(style));
}

lh_void
lh_gx_box_paint(const lh_gx_entity_t *entity, const lh_gx_paint_t *paint)
{
    const lh_gx_style_t *style = lh_gx_entity_get_style(entity);

    lh_return_if(lh_null_eq(style));
    lh_math_rect_t outer;

    lh_gx_rect_to_rect(lh_gx_paint_get_place(paint), lh_addr_of(outer));
    const lh_s32_t x0 = lh_math_point_get_x(lh_math_rect_get_origin(lh_addr_of(outer)));
    const lh_s32_t y0 = lh_math_point_get_y(lh_math_rect_get_origin(lh_addr_of(outer)));
    const lh_s32_t w = lh_math_size_get_width(lh_math_rect_get_size(lh_addr_of(outer)));
    const lh_s32_t h = lh_math_size_get_height(lh_math_rect_get_size(lh_addr_of(outer)));

    lh_return_if(w <= 0 || h <= 0);
    const lh_s32_t r = lh_gx_box_get_radius_px(style, lh_addr_of(outer));
    const lh_s32_t b = lh_gx_box_get_border_px(style, lh_addr_of(outer));
    const lh_gx_color_t fill = lh_gx_style_get_fill(style);
    const lh_gx_color_t edge = lh_gx_style_get_border(style);

    /* The corner bands: the corners pixel by pixel, the middle of each row as one
       rectangle — of the border on a border row, of the fill below it. */
    for (lh_s32_t i = 0; i < r; ++i)
    {
        const lh_s32_t rows[2] = {y0 + i, y0 + h - 1 - i};

        for (lh_s32_t k = 0; k < 2; ++k)
        {
            const lh_s32_t y = rows[k];

            lh_gx_box_paint_corner_row(paint, lh_addr_of(outer), r, b, style, x0, y, r);
            lh_gx_box_paint_corner_row(paint, lh_addr_of(outer), r, b, style, x0 + w - r, y, r);
            lh_gx_box_fill_px(paint, x0 + r, y, w - 2 * r, 1, i < b ? edge : fill);
        }
    }
    /* The straight band: border rows across, then the sides and the fill as three
       rectangles. */
    const lh_s32_t top = y0 + r;
    const lh_s32_t bottom = y0 + h - r;
    const lh_s32_t inner_top = lh_math_max(top, y0 + b);
    const lh_s32_t inner_bottom = lh_math_min(bottom, y0 + h - b);

    lh_gx_box_fill_px(paint, x0, top, w, inner_top - top, edge);
    lh_gx_box_fill_px(paint, x0, inner_bottom, w, bottom - inner_bottom, edge);
    lh_gx_box_fill_px(paint, x0, inner_top, b, inner_bottom - inner_top, edge);
    lh_gx_box_fill_px(paint, x0 + w - b, inner_top, b, inner_bottom - inner_top, edge);
    lh_gx_box_fill_px(paint, x0 + b, inner_top, w - 2 * b, inner_bottom - inner_top, fill);
}

lh_bool_t
lh_gx_box_covers(const lh_gx_entity_t *entity, const lh_gx_rect_t *place, const lh_gx_rect_t *area)
{
    const lh_gx_style_t *style = lh_gx_entity_get_style(entity);

    lh_return_if(lh_null_eq(style), lh_bool_false);
    lh_return_if(lh_pixel_argb_get_channel(lh_gx_style_get_fill(style), 24) != LH_BYTE_T_MAX,
                 lh_bool_false);
    lh_math_rect_t outer;

    lh_gx_rect_to_rect(place, lh_addr_of(outer));
    lh_return_if(lh_gx_box_get_border_px(style, lh_addr_of(outer)) != 0 &&
                     lh_pixel_argb_get_channel(lh_gx_style_get_border(style), 24) != LH_BYTE_T_MAX,
                 lh_bool_false);
    const lh_gx_scalar_t r =
        lh_cast_static(lh_gx_scalar_t, lh_gx_box_get_radius_px(style, lh_addr_of(outer)));
    /* Clear of the corners: inside the box less its corner columns, or less its
       corner rows — the two arms of a plus. */
    lh_gx_rect_t arm;

    lh_gx_rect_inset(place, r, 0, lh_addr_of(arm));
    lh_gx_rect_t inside;
    lh_gx_rect_intersection(lh_addr_of(arm), area, lh_addr_of(inside));
    lh_return_if(lh_gx_rect_eq(lh_addr_of(inside), area), lh_bool_true);
    lh_gx_rect_inset(place, 0, r, lh_addr_of(arm));
    lh_gx_rect_intersection(lh_addr_of(arm), area, lh_addr_of(inside));
    return lh_gx_rect_eq(lh_addr_of(inside), area);
}
