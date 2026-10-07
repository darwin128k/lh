/**
 * @file pixmap.c
 * @brief Implementation of `lh/ui/pixmap.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/byte/limits.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/pixmap.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_pixmap_init(lh_ui_pixmap_t *self, lh_u32_t *pixels, lh_s32_t width, lh_s32_t height, lh_s32_t stride)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(width >= 0 && height >= 0 && stride >= width, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_null_ne(pixels) || width == 0 || height == 0, lh_runtime_error_code_invalid_argument);
    self->pixels = pixels;
    self->width = width;
    self->height = height;
    self->stride = stride;
}

lh_void
lh_ui_pixmap_init_empty(lh_ui_pixmap_t *self)
{
    lh_ui_pixmap_init(self, lh_null, 0, 0, 0);
}

lh_s32_t
lh_ui_pixmap_get_width(const lh_ui_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_s32_t
lh_ui_pixmap_get_height(const lh_ui_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_ui_rect_t
lh_ui_pixmap_get_bounds(const lh_ui_pixmap_t *self)
{
    lh_ui_rect_t bounds;

    lh_assert_runtime_ref(self);
    lh_ui_rect_init(lh_addr_of(bounds), 0, 0, self->width, self->height);
    return bounds;
}

lh_u32_t *
lh_ui_pixmap_get_row(const lh_ui_pixmap_t *self, lh_s32_t y)
{
    lh_assert_runtime_ref(self);
    return self->pixels + y * self->stride;
}

lh_ui_color_t
lh_ui_pixmap_get_pixel(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_ui_color_t color;

    lh_ui_color_init_argb(lh_addr_of(color), lh_ui_pixmap_get_row(self, y)[x]);
    return color;
}

lh_void
lh_ui_pixmap_set_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_ui_pixmap_get_row(self, y)[x] = lh_ui_color_get_argb(color);
}

lh_void
lh_ui_pixmap_blend_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color)
{
    const lh_ui_color_t dst = lh_ui_pixmap_get_pixel(self, x, y);
    const lh_ui_color_t out = lh_ui_color_over(lh_addr_of(dst), color);

    lh_ui_pixmap_set_pixel(self, x, y, lh_addr_of(out));
}

lh_void
lh_ui_pixmap_cover_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color,
                         lh_byte_t coverage)
{
    const lh_ui_color_t edge = lh_ui_color_with_coverage(color, coverage);

    lh_return_if(coverage == 0U);
    lh_ui_pixmap_blend_pixel(self, x, y, lh_addr_of(edge));
}

lh_void
lh_ui_pixmap_store_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t argb)
{
    lh_u32_t *row = lh_ui_pixmap_get_row(self, y);

    for (; x0 < x1; ++x0)
    {
        row[x0] = argb;
    }
}

lh_void
lh_ui_pixmap_blend_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    for (; x0 < x1; ++x0)
    {
        lh_ui_pixmap_blend_pixel(self, x0, y, color);
    }
}

lh_void
lh_ui_pixmap_fill_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    if (lh_ui_color_get_a(color) == LH_BYTE_T_MAX)
    {
        lh_ui_pixmap_store_span(self, x0, x1, y, lh_ui_color_get_argb(color));
        return;
    }
    lh_ui_pixmap_blend_span(self, x0, x1, y, color);
}

lh_void
lh_ui_pixmap_fill_box(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                      const lh_ui_color_t *color)
{
    for (; y0 < y1; ++y0)
    {
        lh_ui_pixmap_fill_span(self, x0, x1, y0, color);
    }
}

lh_void
lh_ui_pixmap_clear(lh_ui_pixmap_t *self, const lh_ui_color_t *color)
{
    const lh_u32_t argb = lh_ui_color_get_argb(color);
    lh_s32_t y;

    lh_assert_runtime_ref(self);
    for (y = 0; y < self->height; ++y)
    {
        lh_ui_pixmap_store_span(self, 0, self->width, y, argb);
    }
}
