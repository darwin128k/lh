/**
 * @file text.c
 * @brief Implementation of `lh/ui/text.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/text.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* ── Reading ─────────────────────────────────────────────────────────────── */

lh_u32_t
lh_ui_text_next_code(const lh_char_t **cursor)
{
    const lh_u32_t code = lh_cast_static(lh_u32_t, lh_cast_static(lh_byte_t, **cursor));

    lh_return_if(code == 0U, 0U);
    ++*cursor;
    return code;
}

lh_bool_t
lh_ui_text_is_line_end(lh_u32_t code)
{
    return code == 0U || code == lh_cast_static(lh_u32_t, '\n') ? lh_bool_true : lh_bool_false;
}

const lh_char_t *
lh_ui_text_get_line_end(const lh_char_t *line)
{
    const lh_char_t *cursor = line;
    const lh_char_t *end = line;

    while (!lh_ui_text_is_line_end(lh_ui_text_next_code(lh_addr_of(cursor))))
    {
        end = cursor;
    }
    return end;
}

const lh_char_t *
lh_ui_text_get_next_line(const lh_char_t *line)
{
    const lh_char_t *cursor = lh_ui_text_get_line_end(line);

    return lh_ui_text_next_code(lh_addr_of(cursor)) == 0U ? lh_null : cursor;
}

lh_u32_t
lh_ui_text_count_lines(const lh_char_t *text)
{
    lh_u32_t count = 0U;

    for (; lh_null_ne(text); text = lh_ui_text_get_next_line(text))
    {
        ++count;
    }
    return count;
}

/* ── Measuring ───────────────────────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_text_get_line_width(const lh_ui_font_t *font, const lh_char_t *line)
{
    const lh_char_t *cursor = line;
    lh_ui_scalar_t width = lh_ui_scalar(0);
    lh_u32_t code;

    for (code = lh_ui_text_next_code(lh_addr_of(cursor)); !lh_ui_text_is_line_end(code);
         code = lh_ui_text_next_code(lh_addr_of(cursor)))
    {
        width += lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_advance(font, code));
    }
    return width;
}

lh_ui_scalar_t
lh_ui_text_get_width(const lh_ui_font_t *font, const lh_char_t *text)
{
    lh_ui_scalar_t width = lh_ui_scalar(0);
    lh_ui_scalar_t line_width;

    for (; lh_null_ne(text); text = lh_ui_text_get_next_line(text))
    {
        line_width = lh_ui_text_get_line_width(font, text);
        width = lh_math_max(width, line_width);
    }
    return width;
}

lh_ui_size_t
lh_ui_text_get_size(const lh_ui_font_t *font, const lh_char_t *text)
{
    lh_ui_size_t size;

    lh_ui_size_init(lh_addr_of(size), lh_ui_text_get_width(font, text),
                    lh_cast_static(lh_ui_scalar_t, lh_ui_text_count_lines(text)) *
                        lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font)));
    return size;
}

lh_ui_rect_t
lh_ui_text_get_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init_origin_size(lh_addr_of(rect), origin, lh_ui_text_get_size(font, text));
    return rect;
}

lh_ui_rect_t
lh_ui_text_get_line_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(origin)), lh_ui_point_get_y(lh_addr_of(origin)),
                    lh_ui_text_get_line_width(font, line),
                    lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font)));
    return rect;
}

/* ── Drawing ─────────────────────────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_text_draw_code(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, lh_u32_t code, lh_ui_point_t origin,
                     const lh_ui_color_t *color)
{
    lh_ui_mask_t glyph;

    if (lh_ui_font_get_glyph(font, code, lh_addr_of(glyph)))
    {
        lh_ui_canvas_fill_mask(canvas, lh_addr_of(glyph), origin, color);
    }
    return lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_advance(font, code));
}

lh_void
lh_ui_text_draw_line(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *line,
                     lh_ui_point_t origin, const lh_ui_color_t *color)
{
    const lh_ui_rect_t rect = lh_ui_text_get_line_rect(font, line, origin);
    const lh_char_t *cursor = line;
    lh_u32_t code;

    lh_return_if(!lh_ui_canvas_shows_rect(canvas, lh_addr_of(rect)));
    for (code = lh_ui_text_next_code(lh_addr_of(cursor)); !lh_ui_text_is_line_end(code);
         code = lh_ui_text_next_code(lh_addr_of(cursor)))
    {
        origin = lh_ui_point_offset(lh_addr_of(origin), lh_ui_text_draw_code(canvas, font, code, origin, color),
                                    lh_ui_scalar(0));
    }
}

lh_void
lh_ui_text_draw(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                const lh_ui_color_t *color)
{
    const lh_ui_scalar_t line_height = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));

    for (; lh_null_ne(text); text = lh_ui_text_get_next_line(text))
    {
        lh_ui_text_draw_line(canvas, font, text, origin, color);
        origin = lh_ui_point_offset(lh_addr_of(origin), lh_ui_scalar(0), line_height);
    }
}
