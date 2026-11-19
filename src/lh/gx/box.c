/**
 * @file box.c
 * @brief Implementation of `lh/gx/box.h`.
 */

#include <lh/gx/box.h>

#include <lh/core/pixel/argb.h>
#include <lh/core/pixel/round.h>
#include <lh/gx/class/initializer.h>

const lh_gx_class_t lh_gx_box_class =
    lh_gx_class_initializer(lh_gx_box_paint, lh_gx_box_covers, lh_null, lh_null, lh_null, lh_null);

lh_s32_t
lh_gx_box_get_radius_px(lh_gx_scalar_t radius, const lh_math_rect_t *outer)
{
    const lh_math_size_t *size = lh_math_rect_get_size(outer);
    const lh_s32_t half =
        lh_math_min(lh_math_size_get_width(size), lh_math_size_get_height(size)) / 2;

    return lh_math_clamp(lh_cast_static(lh_s32_t, radius), 0,
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
                  lh_s32_t height, const lh_gx_brush_t *brush)
{
    lh_return_if(width <= 0 || height <= 0 || lh_gx_brush_is_clear(brush));
    lh_gx_rect_t rect;

    lh_gx_rect_init(lh_addr_of(rect), lh_cast_static(lh_gx_scalar_t, x),
                    lh_cast_static(lh_gx_scalar_t, y), lh_cast_static(lh_gx_scalar_t, width),
                    lh_cast_static(lh_gx_scalar_t, height));
    lh_gx_paint_fill_rect(paint, lh_addr_of(rect), brush);
}

lh_void
lh_gx_box_init_fill(const lh_gx_style_t *style, const lh_gx_rect_t *place,
                    lh_pixel_gradient_table_t *table, lh_gx_brush_t *out)
{
    const lh_pixel_gradient_t *gradient = lh_gx_style_get_gradient(style);

    if (lh_null_eq(gradient))
    {
        lh_gx_brush_init_color(out, lh_gx_style_get_fill(style));
        return;
    }
    lh_pixel_gradient_table_fill(table, gradient);
    lh_gx_brush_init_gradient(out, gradient, table, lh_ptr_deref(place));
}

lh_void
lh_gx_box_paint_corner(const lh_gx_paint_t *paint, const lh_math_rect_t *outer, lh_s32_t radius,
                       lh_s32_t border, const lh_gx_brush_t *fill, const lh_gx_brush_t *edge,
                       lh_s32_t x, lh_s32_t y)
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
    /* As many rows as fit in the stack's mask, at least one. */
    const lh_s32_t band = lh_math_max(LH_GX_BOX_CORNER_BYTES / radius, 1);
    lh_byte_t fill_cover[LH_GX_BOX_CORNER_BYTES];
    lh_byte_t border_cover[LH_GX_BOX_CORNER_BYTES];

    for (lh_s32_t top = 0; top < radius; top += band)
    {
        const lh_s32_t rows = lh_math_min(band, radius - top);

        for (lh_s32_t row = 0; row < rows; ++row)
        {
            for (lh_s32_t i = 0; i < radius; ++i)
            {
                const lh_byte_t outer_cover =
                    lh_pixel_round_coverage(lo_x, lo_y, hi_x, hi_y, r, x + i, y + top + row);
                /* With no border the inner shape is the outer one. */
                const lh_byte_t inner_cover =
                    border == 0
                        ? outer_cover
                        : lh_pixel_round_coverage(lo_x + inset, lo_y + inset, hi_x - inset,
                                                  hi_y - inset, inner_r, x + i, y + top + row);

                fill_cover[row * radius + i] = inner_cover;
                border_cover[row * radius + i] =
                    lh_cast_static(lh_byte_t, lh_math_max(outer_cover - inner_cover, 0));
            }
        }
        lh_gx_point_t origin;

        lh_gx_point_init(lh_addr_of(origin), lh_cast_static(lh_gx_scalar_t, x),
                         lh_cast_static(lh_gx_scalar_t, y + top));
        lh_memory_bounds_t bits;

        lh_memory_bounds_init(lh_addr_of(bits), fill_cover,
                              lh_cast_static(lh_usize_t, rows * radius));
        lh_gx_pixmap_t mask;

        lh_gx_pixmap_init(lh_addr_of(mask), lh_addr_of(bits), lh_cast_static(lh_u32_t, radius),
                          lh_cast_static(lh_u32_t, rows), lh_gx_pixmap_format_a8);
        lh_gx_paint_fill_mask(paint, lh_addr_of(mask), origin, fill);
        if (border != 0)
        {
            /* The mask holds a copy of its bounds: it is given the border's anew. */
            lh_memory_bounds_init(lh_addr_of(bits), border_cover,
                                  lh_cast_static(lh_usize_t, rows * radius));
            lh_gx_pixmap_set_bits(lh_addr_of(mask), lh_addr_of(bits));
            lh_gx_paint_fill_mask(paint, lh_addr_of(mask), origin, edge);
        }
    }
}

lh_s32_t
lh_gx_box_get_left_px(const lh_gx_style_t *style, const lh_math_rect_t *outer, lh_s32_t y)
{
    const lh_gx_corners_t *radius = lh_gx_style_get_radius(style);
    const lh_s32_t x0 = lh_math_point_get_x(lh_math_rect_get_origin(outer));
    const lh_s32_t y0 = lh_math_point_get_y(lh_math_rect_get_origin(outer));
    const lh_s32_t h = lh_math_size_get_height(lh_math_rect_get_size(outer));
    const lh_s32_t top = lh_gx_box_get_radius_px(lh_gx_corners_get_top_left(radius), outer);
    const lh_s32_t bottom = lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_left(radius), outer);

    lh_return_if(y < y0 + top, x0 + top);
    lh_return_if(y >= y0 + h - bottom, x0 + bottom);
    return x0;
}

lh_s32_t
lh_gx_box_get_right_px(const lh_gx_style_t *style, const lh_math_rect_t *outer, lh_s32_t y)
{
    const lh_gx_corners_t *radius = lh_gx_style_get_radius(style);
    const lh_s32_t x1 = lh_math_point_get_x(lh_math_rect_get_origin(outer)) +
                        lh_math_size_get_width(lh_math_rect_get_size(outer));
    const lh_s32_t y0 = lh_math_point_get_y(lh_math_rect_get_origin(outer));
    const lh_s32_t h = lh_math_size_get_height(lh_math_rect_get_size(outer));
    const lh_s32_t top = lh_gx_box_get_radius_px(lh_gx_corners_get_top_right(radius), outer);
    const lh_s32_t bottom = lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_right(radius), outer);

    lh_return_if(y < y0 + top, x1 - top);
    lh_return_if(y >= y0 + h - bottom, x1 - bottom);
    return x1;
}

lh_s32_t
lh_gx_box_get_strip_end_px(const lh_gx_style_t *style, const lh_math_rect_t *outer, lh_s32_t y)
{
    const lh_gx_corners_t *radius = lh_gx_style_get_radius(style);
    const lh_s32_t y0 = lh_math_point_get_y(lh_math_rect_get_origin(outer));
    const lh_s32_t y1 = y0 + lh_math_size_get_height(lh_math_rect_get_size(outer));
    const lh_s32_t b = lh_gx_box_get_border_px(style, outer);
    /* Where a corner ends or begins, and where the border rows end or begin. */
    const lh_s32_t rows[6] = {
        y0 + lh_gx_box_get_radius_px(lh_gx_corners_get_top_left(radius), outer),
        y0 + lh_gx_box_get_radius_px(lh_gx_corners_get_top_right(radius), outer),
        y1 - lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_right(radius), outer),
        y1 - lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_left(radius), outer),
        y0 + b,
        y1 - b,
    };
    lh_s32_t end = y1;

    for (lh_usize_t i = 0U; i < 6U; ++i)
    {
        if (rows[i] > y)
        {
            end = lh_math_min(end, rows[i]);
        }
    }
    return end;
}

lh_bool_t
lh_gx_box_is_clear_of_corners(const lh_gx_style_t *style, const lh_gx_rect_t *place,
                              const lh_gx_rect_t *area)
{
    const lh_gx_corners_t *radius = lh_gx_style_get_radius(style);
    lh_math_rect_t outer;

    lh_gx_rect_to_rect(place, lh_addr_of(outer));
    const lh_s32_t x0 = lh_math_point_get_x(lh_math_rect_get_origin(lh_addr_of(outer)));
    const lh_s32_t y0 = lh_math_point_get_y(lh_math_rect_get_origin(lh_addr_of(outer)));
    const lh_s32_t w = lh_math_size_get_width(lh_math_rect_get_size(lh_addr_of(outer)));
    const lh_s32_t h = lh_math_size_get_height(lh_math_rect_get_size(lh_addr_of(outer)));
    const lh_s32_t tl =
        lh_gx_box_get_radius_px(lh_gx_corners_get_top_left(radius), lh_addr_of(outer));
    const lh_s32_t tr =
        lh_gx_box_get_radius_px(lh_gx_corners_get_top_right(radius), lh_addr_of(outer));
    const lh_s32_t br =
        lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_right(radius), lh_addr_of(outer));
    const lh_s32_t bl =
        lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_left(radius), lh_addr_of(outer));
    /* The two arms of a plus: the box less its corner columns, less its corner rows. */
    const lh_s32_t left = lh_math_max(tl, bl);
    const lh_s32_t right = lh_math_max(tr, br);
    const lh_s32_t top = lh_math_max(tl, tr);
    const lh_s32_t bottom = lh_math_max(bl, br);
    lh_gx_rect_t arm;

    lh_gx_rect_init(lh_addr_of(arm), lh_cast_static(lh_gx_scalar_t, x0 + left),
                    lh_cast_static(lh_gx_scalar_t, y0),
                    lh_cast_static(lh_gx_scalar_t, w - left - right),
                    lh_cast_static(lh_gx_scalar_t, h));
    lh_gx_rect_t inside;

    lh_gx_rect_intersection(lh_addr_of(arm), area, lh_addr_of(inside));
    lh_return_if(lh_gx_rect_eq(lh_addr_of(inside), area), lh_bool_true);
    lh_gx_rect_init(lh_addr_of(arm), lh_cast_static(lh_gx_scalar_t, x0),
                    lh_cast_static(lh_gx_scalar_t, y0 + top), lh_cast_static(lh_gx_scalar_t, w),
                    lh_cast_static(lh_gx_scalar_t, h - top - bottom));
    lh_gx_rect_intersection(lh_addr_of(arm), area, lh_addr_of(inside));
    return lh_gx_rect_eq(lh_addr_of(inside), area);
}

lh_void
lh_gx_box_paint_strip(const lh_gx_paint_t *paint, const lh_gx_style_t *style,
                      const lh_math_rect_t *outer, const lh_gx_brush_t *fill,
                      const lh_gx_brush_t *edge, lh_s32_t top, lh_s32_t bottom)
{
    const lh_s32_t x0 = lh_math_point_get_x(lh_math_rect_get_origin(outer));
    const lh_s32_t y0 = lh_math_point_get_y(lh_math_rect_get_origin(outer));
    const lh_s32_t w = lh_math_size_get_width(lh_math_rect_get_size(outer));
    const lh_s32_t h = lh_math_size_get_height(lh_math_rect_get_size(outer));
    const lh_s32_t b = lh_gx_box_get_border_px(style, outer);
    const lh_s32_t left = lh_gx_box_get_left_px(style, outer, top);
    const lh_s32_t right = lh_gx_box_get_right_px(style, outer, top);
    const lh_s32_t rows = bottom - top;

    /* A border row is the border from corner to corner. */
    if (top < y0 + b || top >= y0 + h - b)
    {
        lh_gx_box_fill_px(paint, left, top, right - left, rows, edge);
        return;
    }
    /* Any other: the border on each side that is not a corner's, the fill between. */
    const lh_s32_t inner_left = lh_math_max(left, x0 + b);
    const lh_s32_t inner_right = lh_math_min(right, x0 + w - b);

    lh_gx_box_fill_px(paint, left, top, inner_left - left, rows, edge);
    lh_gx_box_fill_px(paint, inner_right, top, right - inner_right, rows, edge);
    lh_gx_box_fill_px(paint, inner_left, top, inner_right - inner_left, rows, fill);
}

lh_bool_t
lh_gx_box_covers(const lh_gx_entity_t *entity, const lh_gx_rect_t *place, const lh_gx_rect_t *area)
{
    const lh_gx_style_t *style = lh_gx_entity_get_style(entity);

    lh_return_if(lh_null_eq(style), lh_bool_false);
    lh_return_if(!lh_gx_style_is_fill_opaque(style), lh_bool_false);
    lh_math_rect_t outer;

    lh_gx_rect_to_rect(place, lh_addr_of(outer));
    lh_return_if(lh_gx_box_get_border_px(style, lh_addr_of(outer)) != 0 &&
                     lh_pixel_argb_get_channel(lh_gx_style_get_border(style), 24) != LH_BYTE_T_MAX,
                 lh_bool_false);
    return lh_gx_box_is_clear_of_corners(style, place, area);
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
    const lh_gx_corners_t *radius = lh_gx_style_get_radius(style);
    const lh_s32_t b = lh_gx_box_get_border_px(style, lh_addr_of(outer));
    const lh_s32_t tl =
        lh_gx_box_get_radius_px(lh_gx_corners_get_top_left(radius), lh_addr_of(outer));
    const lh_s32_t tr =
        lh_gx_box_get_radius_px(lh_gx_corners_get_top_right(radius), lh_addr_of(outer));
    const lh_s32_t br =
        lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_right(radius), lh_addr_of(outer));
    const lh_s32_t bl =
        lh_gx_box_get_radius_px(lh_gx_corners_get_bottom_left(radius), lh_addr_of(outer));

    /* The inside one colour or the gradient over the whole box, its colours worked
       out once for every piece, and the border of one colour. */
    lh_pixel_gradient_table_t table;
    lh_gx_brush_t fill;

    lh_gx_box_init_fill(style, lh_gx_paint_get_place(paint), lh_addr_of(table), lh_addr_of(fill));
    lh_gx_brush_t edge;

    lh_gx_brush_init_color(lh_addr_of(edge), lh_gx_style_get_border(style));
    /* The corners as masks, each of its own radius; a square corner is none. */
    if (tl > 0)
    {
        lh_gx_box_paint_corner(paint, lh_addr_of(outer), tl, b, lh_addr_of(fill), lh_addr_of(edge),
                               x0, y0);
    }
    if (tr > 0)
    {
        lh_gx_box_paint_corner(paint, lh_addr_of(outer), tr, b, lh_addr_of(fill), lh_addr_of(edge),
                               x0 + w - tr, y0);
    }
    if (br > 0)
    {
        lh_gx_box_paint_corner(paint, lh_addr_of(outer), br, b, lh_addr_of(fill), lh_addr_of(edge),
                               x0 + w - br, y0 + h - br);
    }
    if (bl > 0)
    {
        lh_gx_box_paint_corner(paint, lh_addr_of(outer), bl, b, lh_addr_of(fill), lh_addr_of(edge),
                               x0, y0 + h - bl);
    }
    /* The rest as rectangles, a strip of rows at a time: within a strip neither
       the corners beside it nor whether its rows are the border change. */
    for (lh_s32_t top = y0; top < y0 + h;)
    {
        const lh_s32_t bottom = lh_gx_box_get_strip_end_px(style, lh_addr_of(outer), top);

        lh_gx_box_paint_strip(paint, style, lh_addr_of(outer), lh_addr_of(fill), lh_addr_of(edge),
                              top, bottom);
        top = bottom;
    }
}
