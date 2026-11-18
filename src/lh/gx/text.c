/**
 * @file text.c
 * @brief Implementation of `lh/gx/text.h`.
 */

#include <lh/gx/text.h>

#include <lh/core/char/map.h>
#include <lh/core/math.h>
#include <lh/core/str/utf8.h>
#include <lh/core/util/return.h>
#include <lh/gx/class/initializer.h>

const lh_gx_class_t lh_gx_text_class = lh_gx_class_initializer(lh_gx_text_paint, lh_null, lh_null);

lh_s32_t
lh_gx_text_get_width(const lh_gx_font_t *font, const lh_char_t *text)
{
    lh_s32_t width = 0;
    const lh_char_t *cursor = text;

    for (lh_u32_t code = lh_str_utf8_get_next_code(lh_addr_of(cursor));
         code != 0U && code != lh_cast_static(lh_u32_t, lh_char_map_lf);
         code = lh_str_utf8_get_next_code(lh_addr_of(cursor)))
    {
        lh_gx_glyph_t glyph;

        if (lh_gx_font_get_glyph(font, code, lh_addr_of(glyph)))
        {
            width += lh_gx_glyph_get_advance(lh_addr_of(glyph));
        }
    }
    return width;
}

lh_void
lh_gx_text_paint_glyph(const lh_gx_paint_t *paint, const lh_gx_glyph_t *glyph, lh_s32_t x,
                       lh_s32_t baseline, lh_gx_color_t color)
{
    const lh_pixel_mask_t *mask = lh_gx_glyph_get_mask(glyph);
    const lh_s32_t width = lh_math_min(lh_pixel_mask_get_width(mask), LH_GX_TEXT_GLYPH_MAX);
    const lh_s32_t height = lh_pixel_mask_get_height(mask);

    lh_return_if(width <= 0 || height <= 0);
    const lh_s32_t top = baseline + lh_gx_glyph_get_top(glyph);
    lh_byte_t cover[LH_GX_TEXT_GLYPH_MAX];
    lh_memory_bounds_t bits;

    lh_memory_bounds_init(lh_addr_of(bits), cover, lh_cast_static(lh_usize_t, width));
    lh_gx_pixmap_t row;

    lh_gx_pixmap_init(lh_addr_of(row), lh_addr_of(bits), lh_cast_static(lh_u32_t, width), 1U,
                      lh_gx_pixmap_format_a8);
    for (lh_s32_t y = 0; y < height; ++y)
    {
        for (lh_s32_t i = 0; i < width; ++i)
        {
            cover[i] = lh_pixel_mask_get_coverage(mask, i, y);
        }
        lh_gx_point_t at;

        lh_gx_point_init(lh_addr_of(at), lh_cast_static(lh_gx_scalar_t, x),
                         lh_cast_static(lh_gx_scalar_t, top + y));
        lh_gx_paint_fill_mask(paint, lh_addr_of(row), at, color);
    }
}

lh_void
lh_gx_text_paint(const lh_gx_entity_t *entity, const lh_gx_paint_t *paint)
{
    const lh_gx_style_t *style = lh_gx_entity_get_style(entity);

    lh_return_if(lh_null_eq(style));
    const lh_gx_font_t *font = lh_gx_style_get_font(style);
    const lh_char_t *cursor = lh_ptr_rcast(const lh_char_t, lh_gx_entity_get_content(entity));

    lh_return_if(lh_null_eq(font) || lh_null_eq(cursor));
    lh_math_rect_t place;

    lh_gx_rect_to_rect(lh_gx_paint_get_place(paint), lh_addr_of(place));
    const lh_s32_t baseline = lh_math_point_get_y(lh_math_rect_get_origin(lh_addr_of(place))) +
                              lh_gx_font_get_ascent(font);
    const lh_gx_color_t color = lh_gx_style_get_fill(style);
    lh_s32_t pen = lh_math_point_get_x(lh_math_rect_get_origin(lh_addr_of(place)));

    for (lh_u32_t code = lh_str_utf8_get_next_code(lh_addr_of(cursor));
         code != 0U && code != lh_cast_static(lh_u32_t, lh_char_map_lf);
         code = lh_str_utf8_get_next_code(lh_addr_of(cursor)))
    {
        lh_gx_glyph_t glyph;

        if (lh_gx_font_get_glyph(font, code, lh_addr_of(glyph)))
        {
            lh_gx_text_paint_glyph(paint, lh_addr_of(glyph), pen, baseline, color);
            pen += lh_gx_glyph_get_advance(lh_addr_of(glyph));
        }
    }
}
