/**
 * @file sw.c
 * @brief Implementation of `lh/gx/backend/sw.h`.
 */

#include <lh/gx/backend/sw.h>

#include <lh/core/pixel/argb.h>
#include <lh/core/pixel/span.h>

const lh_gx_pixmap_t *
lh_gx_backend_sw_get_target(const lh_gx_backend_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_gx_pixmap_t *
lh_gx_backend_sw_get_target_mut(const lh_gx_backend_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_void
lh_gx_backend_sw_set_target(lh_gx_backend_sw_t *self, lh_gx_pixmap_t *target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

const lh_math_point_t *
lh_gx_backend_sw_get_origin(const lh_gx_backend_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_void
lh_gx_backend_sw_set_origin(lh_gx_backend_sw_t *self, lh_math_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
}

lh_void
lh_gx_backend_sw_init(lh_gx_backend_sw_t *self, lh_gx_pixmap_t *target, lh_math_scalar_t x,
                      lh_math_scalar_t y)
{
    lh_gx_backend_sw_set_target(self, target);
    lh_math_point_t origin;

    lh_math_point_init(lh_addr_of(origin), x, y);
    lh_gx_backend_sw_set_origin(self, origin);
}

lh_bool_t
lh_gx_backend_sw_get_box(const lh_gx_backend_sw_t *self, const lh_gx_rect_t *clip,
                         const lh_gx_rect_t *rect, lh_math_rect_t *out)
{
    const lh_gx_pixmap_t *target = lh_gx_backend_sw_get_target(self);
    const lh_math_point_t *origin = lh_gx_backend_sw_get_origin(self);
    lh_math_rect_t clip_px;

    lh_gx_rect_to_rect(clip, lh_addr_of(clip_px));
    lh_math_rect_t rect_px;

    lh_gx_rect_to_rect(rect, lh_addr_of(rect_px));
    lh_math_rect_t on_screen;

    lh_math_rect_intersection(lh_addr_of(clip_px), lh_addr_of(rect_px), lh_addr_of(on_screen));
    lh_math_rect_t in_target;

    lh_math_rect_offset(lh_addr_of(on_screen), -lh_math_point_get_x(origin),
                        -lh_math_point_get_y(origin), lh_addr_of(in_target));
    lh_math_rect_t bounds;

    lh_math_rect_init(lh_addr_of(bounds), 0, 0,
                      lh_cast_static(lh_math_scalar_t, lh_gx_pixmap_get_width(target)),
                      lh_cast_static(lh_math_scalar_t, lh_gx_pixmap_get_height(target)));
    lh_math_rect_intersection(lh_addr_of(bounds), lh_addr_of(in_target), out);
    return lh_cast_static(lh_bool_t, !lh_math_rect_is_empty(out));
}

lh_void
lh_gx_backend_sw_fill_rect(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_rect_t *rect,
                           lh_gx_color_t color)
{
    const lh_gx_backend_sw_t *self = lh_ptr_rcast(const lh_gx_backend_sw_t, context);
    lh_math_rect_t box;

    lh_return_if(!lh_gx_backend_sw_get_box(self, clip, rect, lh_addr_of(box)));
    lh_gx_pixmap_t *target = lh_gx_backend_sw_get_target_mut(self);
    const lh_gx_pixmap_format_t format = lh_gx_pixmap_get_format(target);
    const lh_math_point_t *at = lh_math_rect_get_origin(lh_addr_of(box));
    const lh_math_size_t *size = lh_math_rect_get_size(lh_addr_of(box));
    const lh_usize_t count = lh_cast_static(lh_usize_t, lh_math_size_get_width(size));
    const lh_u32_t alpha = lh_pixel_argb_get_channel(color, 24);
    const lh_u32_t x = lh_cast_static(lh_u32_t, lh_math_point_get_x(at));

    for (lh_math_scalar_t row = 0; row < lh_math_size_get_height(size); ++row)
    {
        lh_byte_t *line = lh_gx_pixmap_get_address(
            target, x, lh_cast_static(lh_u32_t, lh_math_point_get_y(at) + row));

        if (format == lh_gx_pixmap_format_argb8888)
        {
            lh_pixel_span_blend_argb(lh_ptr_rcast(lh_u32_t, line), count, color, alpha);
        }
        else
        {
            lh_pixel_span_blend_rgb565(
                lh_ptr_rcast(lh_u16_t, line), count, color, alpha,
                lh_cast_static(lh_bool_t, format == lh_gx_pixmap_format_rgb565_swapped));
        }
    }
}

lh_void
lh_gx_backend_sw_fill_mask(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_pixmap_t *mask,
                           lh_gx_point_t at, lh_gx_color_t color)
{
    const lh_gx_backend_sw_t *self = lh_ptr_rcast(const lh_gx_backend_sw_t, context);
    lh_gx_rect_t placed;

    lh_gx_rect_init(lh_addr_of(placed), lh_gx_point_get_x(lh_addr_of(at)),
                    lh_gx_point_get_y(lh_addr_of(at)),
                    lh_cast_static(lh_gx_scalar_t, lh_gx_pixmap_get_width(mask)),
                    lh_cast_static(lh_gx_scalar_t, lh_gx_pixmap_get_height(mask)));
    lh_math_rect_t box;

    lh_return_if(!lh_gx_backend_sw_get_box(self, clip, lh_addr_of(placed), lh_addr_of(box)));
    /* Where the box starts inside the mask: the mask's own top-left corner, in the
       target's coordinates, is its screen position less the origin. */
    lh_math_rect_t mask_px;

    lh_gx_rect_to_rect(lh_addr_of(placed), lh_addr_of(mask_px));
    const lh_math_point_t *origin = lh_gx_backend_sw_get_origin(self);
    const lh_math_point_t *box_at = lh_math_rect_get_origin(lh_addr_of(box));
    const lh_math_point_t *mask_at = lh_math_rect_get_origin(lh_addr_of(mask_px));
    const lh_math_scalar_t dx =
        lh_math_point_get_x(box_at) + lh_math_point_get_x(origin) - lh_math_point_get_x(mask_at);
    const lh_math_scalar_t dy =
        lh_math_point_get_y(box_at) + lh_math_point_get_y(origin) - lh_math_point_get_y(mask_at);
    lh_gx_pixmap_t *target = lh_gx_backend_sw_get_target_mut(self);
    const lh_gx_pixmap_format_t format = lh_gx_pixmap_get_format(target);
    const lh_math_size_t *size = lh_math_rect_get_size(lh_addr_of(box));
    const lh_usize_t count = lh_cast_static(lh_usize_t, lh_math_size_get_width(size));
    const lh_u32_t alpha = lh_pixel_argb_get_channel(color, 24);
    const lh_u32_t x = lh_cast_static(lh_u32_t, lh_math_point_get_x(box_at));

    for (lh_math_scalar_t row = 0; row < lh_math_size_get_height(size); ++row)
    {
        lh_byte_t *line = lh_gx_pixmap_get_address(
            target, x, lh_cast_static(lh_u32_t, lh_math_point_get_y(box_at) + row));
        const lh_byte_t *cover = lh_gx_pixmap_get_address(mask, lh_cast_static(lh_u32_t, dx),
                                                          lh_cast_static(lh_u32_t, dy + row));

        if (format == lh_gx_pixmap_format_argb8888)
        {
            lh_pixel_span_cover_argb(lh_ptr_rcast(lh_u32_t, line), cover, count, color, alpha);
        }
        else
        {
            lh_pixel_span_cover_rgb565(
                lh_ptr_rcast(lh_u16_t, line), cover, count, color, alpha,
                lh_cast_static(lh_bool_t, format == lh_gx_pixmap_format_rgb565_swapped));
        }
    }
}

lh_void
lh_gx_backend_sw_canvas_init(lh_gx_canvas_t *table)
{
    lh_gx_canvas_init(table);
    lh_gx_canvas_set_fill_rect(table, lh_gx_backend_sw_fill_rect);
    lh_gx_canvas_set_fill_mask(table, lh_gx_backend_sw_fill_mask);
}
