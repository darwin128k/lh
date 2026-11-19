/**
 * @file paint.c
 * @brief Implementation of `lh/gx/paint.h`.
 */

#include <lh/gx/paint.h>

#include <lh/core/pixel/argb.h>

const lh_gx_canvas_t *
lh_gx_paint_get_canvas(const lh_gx_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas;
}

lh_void
lh_gx_paint_set_canvas(lh_gx_paint_t *self, const lh_gx_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    self->canvas = canvas;
}

lh_ptr
lh_gx_paint_get_canvas_context(const lh_gx_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas_context;
}

lh_void
lh_gx_paint_set_canvas_context(lh_gx_paint_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->canvas_context = context;
}

const lh_gx_rect_t *
lh_gx_paint_get_clip(const lh_gx_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

lh_void
lh_gx_paint_set_clip(lh_gx_paint_t *self, lh_gx_rect_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
}

const lh_gx_rect_t *
lh_gx_paint_get_place(const lh_gx_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->place);
}

lh_void
lh_gx_paint_set_place(lh_gx_paint_t *self, lh_gx_rect_t place)
{
    lh_assert_runtime_ref(self);
    self->place = place;
}

lh_void
lh_gx_paint_init(lh_gx_paint_t *self, const lh_gx_canvas_t *canvas, lh_ptr context,
                 lh_gx_rect_t clip, lh_gx_rect_t place)
{
    lh_gx_paint_set_canvas(self, canvas);
    lh_gx_paint_set_canvas_context(self, context);
    lh_gx_paint_set_clip(self, clip);
    lh_gx_paint_set_place(self, place);
}

lh_void
lh_gx_paint_fill_rect(const lh_gx_paint_t *self, const lh_gx_rect_t *rect,
                      const lh_gx_brush_t *brush)
{
    lh_gx_canvas_get_fill_rect(lh_gx_paint_get_canvas(self))(
        lh_gx_paint_get_canvas_context(self), lh_gx_paint_get_clip(self), rect, brush);
}

lh_void
lh_gx_paint_fill_shadow(const lh_gx_paint_t *self, const lh_gx_rect_t *box, lh_gx_scalar_t radius,
                        const lh_gx_shadow_t *shadow)
{
    lh_gx_canvas_fill_shadow_fn *fill_shadow =
        lh_gx_canvas_get_fill_shadow(lh_gx_paint_get_canvas(self));

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, fill_shadow)));
    fill_shadow(lh_gx_paint_get_canvas_context(self), lh_gx_paint_get_clip(self), box, radius,
                shadow);
}

lh_void
lh_gx_paint_fill_rect_color(const lh_gx_paint_t *self, const lh_gx_rect_t *rect,
                            lh_gx_color_t color)
{
    lh_gx_brush_t brush;

    lh_gx_brush_init_color(lh_addr_of(brush), color);
    lh_gx_paint_fill_rect(self, rect, lh_addr_of(brush));
}

lh_void
lh_gx_paint_fill_mask_by_rects(const lh_gx_paint_t *self, const lh_gx_pixmap_t *mask,
                               lh_gx_point_t at, const lh_gx_brush_t *brush)
{
    const lh_gx_color_t color = lh_gx_brush_get_color(brush);
    const lh_u32_t alpha = lh_pixel_argb_get_channel(color, 24);
    const lh_u32_t rgb = lh_bit_and(color, 0x00FFFFFFU);

    for (lh_u32_t y = 0U; y < lh_gx_pixmap_get_height(mask); ++y)
    {
        const lh_byte_t *cover = lh_gx_pixmap_get_address(mask, 0U, y);
        const lh_s32_t row = lh_cast_static(lh_s32_t, lh_gx_point_get_y(lh_addr_of(at))) +
                             lh_cast_static(lh_s32_t, y);

        for (lh_u32_t x = 0U; x < lh_gx_pixmap_get_width(mask); ++x)
        {
            const lh_s32_t column = lh_cast_static(lh_s32_t, lh_gx_point_get_x(lh_addr_of(at))) +
                                    lh_cast_static(lh_s32_t, x);
            const lh_u32_t a = lh_pixel_channel_div255_round(cover[x] * alpha);

            if (a != 0U)
            {
                lh_gx_rect_t pixel;

                lh_gx_rect_init(lh_addr_of(pixel), lh_cast_static(lh_gx_scalar_t, column),
                                lh_cast_static(lh_gx_scalar_t, row), 1, 1);
                lh_gx_paint_fill_rect_color(self, lh_addr_of(pixel),
                                            lh_bit_or(lh_bit_shl(a, 24), rgb));
            }
        }
    }
}

lh_void
lh_gx_paint_fill_mask(const lh_gx_paint_t *self, const lh_gx_pixmap_t *mask, lh_gx_point_t at,
                      const lh_gx_brush_t *brush)
{
    lh_gx_canvas_fill_mask_fn *fill_mask = lh_gx_canvas_get_fill_mask(lh_gx_paint_get_canvas(self));

    if (lh_null_eq(lh_ptr_rcast(lh_void, fill_mask)))
    {
        lh_gx_paint_fill_mask_by_rects(self, mask, at, brush);
        return;
    }
    fill_mask(lh_gx_paint_get_canvas_context(self), lh_gx_paint_get_clip(self), mask, at, brush);
}

lh_void
lh_gx_paint_fill_mask_color(const lh_gx_paint_t *self, const lh_gx_pixmap_t *mask, lh_gx_point_t at,
                            lh_gx_color_t color)
{
    lh_gx_brush_t brush;

    lh_gx_brush_init_color(lh_addr_of(brush), color);
    lh_gx_paint_fill_mask(self, mask, at, lh_addr_of(brush));
}
