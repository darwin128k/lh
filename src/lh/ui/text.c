/**
 * @file text.c
 * @brief Implementation of `lh/ui/text.h`.
 */

#include <lh/math.h>
#include <lh/char/map.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/text.h>
#include <lh/util/return.h>

/* -- Reading --------------------------------------------------------------- */

lh_u32_t
lh_ui_text_next_code(const lh_char_t **cursor)
{
    lh_u32_t code;
    lh_u32_t left;
    lh_u32_t at;
    const lh_byte_t first = lh_cast_static(lh_byte_t, lh_ptr_deref(lh_ptr_deref(cursor)));

    lh_return_if(first == 0U, 0U);
    if (first < 0x80U)
    {
        ++lh_ptr_deref(cursor);
        return lh_cast_static(lh_u32_t, first);
    }
    /* UTF-8, and **this is the place to change it**: the lead byte says how many bytes
       the code takes, and the ones after it each add six bits. It used to read one
       byte and hand that byte back as if it were the code, which was true of every
       string an application could put in a label and false of every string a person
       can type -- and a **field** is the first thing in the engine that writes one. */
    if ((first & 0xE0U) == 0xC0U)
    {
        code = lh_cast_static(lh_u32_t, first & 0x1FU);
        left = 1U;
    }
    else if ((first & 0xF0U) == 0xE0U)
    {
        code = lh_cast_static(lh_u32_t, first & 0x0FU);
        left = 2U;
    }
    else if ((first & 0xF8U) == 0xF0U)
    {
        code = lh_cast_static(lh_u32_t, first & 0x07U);
        left = 3U;
    }
    else
    {
        /* A continuation byte where a lead byte should be: a broken string, and one
           byte is as far as it is safe to go. Swallowing it would make the measuring
           below walk past a character nobody can see. */
        ++lh_ptr_deref(cursor);
        return lh_cast_static(lh_u32_t, first);
    }
    ++lh_ptr_deref(cursor);
    for (at = 0U; at < left; ++at)
    {
        const lh_byte_t next = lh_cast_static(lh_byte_t, lh_ptr_deref(lh_ptr_deref(cursor)));

        if ((next & 0xC0U) != 0x80U)
        {
            /* Truncated at the end of the string: what has been read is what there
               is, and the cursor stays on the byte that is not a continuation. */
            return code;
        }
        code = (code << 6) | lh_cast_static(lh_u32_t, next & 0x3FU);
        ++lh_ptr_deref(cursor);
    }
    return code;
}

lh_bool_t
lh_ui_text_is_line_end(lh_u32_t code)
{
    return code == 0U || code == lh_cast_static(lh_u32_t, lh_char_map_lf) ? lh_bool_true : lh_bool_false;
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

/* -- Lines a width breaks -------------------------------------------------- */

lh_ui_scalar_t
lh_ui_text_get_line_width(const lh_ui_font_t *font, const lh_char_t *line)
{
    lh_ui_text_line_t laid;

    (void)lh_ui_text_line_measure(lh_addr_of(laid), font, line, lh_ui_scalar(0));
    return lh_ui_text_line_get_width(lh_addr_of(laid));
}

lh_ui_scalar_t
lh_ui_text_get_wrapped_width(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_scalar_t width)
{
    lh_ui_text_block_t block;

    lh_ui_text_block_measure_layout(lh_addr_of(block), font, text, width);
    return lh_ui_text_block_get_width(lh_addr_of(block));
}

lh_ui_scalar_t
lh_ui_text_get_width(const lh_ui_font_t *font, const lh_char_t *text)
{
    return lh_ui_text_get_wrapped_width(font, text, lh_ui_scalar(0));
}

lh_u32_t
lh_ui_text_count_lines_wrapped(const lh_ui_font_t *font, const lh_char_t *text,
                               lh_ui_scalar_t width)
{
    lh_ui_text_block_t block;

    lh_ui_text_block_measure_layout(lh_addr_of(block), font, text, width);
    return lh_ui_text_block_get_lines(lh_addr_of(block));
}

/* -- Ink ------------------------------------------------------------------- */

lh_void
lh_ui_text_get_line_ink_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin,
                             lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_text_line_t laid;

    (void)lh_ui_text_line_measure(lh_addr_of(laid), font, line, lh_ui_scalar(0));
    lh_ui_text_line_measure_ink(lh_addr_of(laid), font);
    lh_ui_text_line_get_ink_rect(lh_addr_of(laid), origin, out);
}

lh_ui_scalar_t
lh_ui_text_get_ink_top_wrapped(const lh_ui_font_t *font, const lh_char_t *text,
                               lh_ui_scalar_t width)
{
    lh_ui_text_block_t block;

    lh_ui_text_block_measure(lh_addr_of(block), font, text, width);
    return lh_ui_text_block_get_ink_top(lh_addr_of(block));
}

lh_ui_scalar_t
lh_ui_text_get_ink_top(const lh_ui_font_t *font, const lh_char_t *text)
{
    return lh_ui_text_get_ink_top_wrapped(font, text, lh_ui_scalar(0));
}

lh_void
lh_ui_text_get_ink_rect_wrapped(const lh_ui_font_t *font, const lh_char_t *text,
                                lh_ui_point_t origin, lh_ui_scalar_t width,
                                lh_ui_text_align_h_t align, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_ui_scalar_t line_height =
        lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));
    lh_ui_text_block_t block;
    lh_ui_text_line_t laid;
    lh_ui_scalar_t down;
    lh_ui_rect_t ink;

    lh_ui_text_block_measure(lh_addr_of(block), font, text, width);
    /* Left-aligned, the block already holds the hull: one walk. Centred or right,
       every line moves by the room the widest one leaves it, which is only known
       once every line has been measured, so the lines are laid out a second time. */
    if (align == lh_ui_text_align_h_left)
    {
        lh_ui_text_block_get_ink_rect(lh_addr_of(block), origin, out);
        return;
    }
    down = lh_math_neg(lh_ui_text_block_get_ink_top(lh_addr_of(block)));
    lh_ui_rect_init_empty(lh_addr_of(ink));
    for (; lh_ui_text_line_measure(lh_addr_of(laid), font, text, width);
         text = lh_ui_text_line_get_next(lh_addr_of(laid)))
    {
        /* Every line box starts that far above @p origin, because @p origin is
           where the ink is and a line box is taller than what it draws. */
        lh_ui_point_t box;
        lh_ui_point_offset(lh_addr_of(origin),
                           lh_ui_text_line_get_shift(lh_addr_of(laid),
                                                     lh_ui_text_block_get_width(lh_addr_of(block)),
                                                     align),
                           down, lh_addr_of(box));
        lh_ui_rect_t line_ink;

        lh_ui_text_line_measure_ink(lh_addr_of(laid), font);
        lh_ui_text_line_get_ink_rect(lh_addr_of(laid), box, lh_addr_of(line_ink));

        /* A hull: as wide as the widest line and as tall as the first ink to the
           last. The gap between two lines is inside it and is not drawn into, which
           is what a line box is for anyway. */
        lh_ui_rect_union(lh_addr_of(ink), lh_addr_of(line_ink), lh_addr_of(ink));
        down += line_height;
    }
    lh_ptr_deref(out) = ink;
}

lh_void
lh_ui_text_get_ink_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                        lh_ui_rect_t *out)
{
    lh_ui_text_get_ink_rect_wrapped(font, text, origin, lh_ui_scalar(0), lh_ui_text_align_h_left,
                                    out);
}

lh_void
lh_ui_text_get_size_wrapped(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_scalar_t width,
                            lh_ui_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_text_block_t block;

    lh_ui_text_block_measure(lh_addr_of(block), font, text, width);
    lh_ui_text_block_get_size(lh_addr_of(block), font, out);
}

lh_void
lh_ui_text_get_size(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_size_t *out)
{
    lh_ui_text_get_size_wrapped(font, text, lh_ui_scalar(0), out);
}

lh_void
lh_ui_text_get_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                    lh_ui_rect_t *out)
{
    lh_ui_text_get_ink_rect(font, text, origin, out);
}

lh_void
lh_ui_text_get_line_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin,
                         lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(origin)),
                    lh_ui_point_get_y(lh_addr_of(origin)), lh_ui_text_get_line_width(font, line),
                    lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font)));
    lh_ptr_deref(out) = rect;
}

/* -- Drawing --------------------------------------------------------------- */

lh_ui_scalar_t
lh_ui_text_draw_glyph_at(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, lh_u32_t index,
                         lh_ui_point_t origin, const lh_ui_color_t *color)
{
    lh_ui_point_t at;

    /* origin is the line top-left; baseline is ascent down; mask top is relative to it. */
    lh_ui_point_init(lh_addr_of(at), lh_ui_point_get_x(lh_addr_of(origin)),
                     lh_ui_point_get_y(lh_addr_of(origin)) +
                         lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ascent(font)) +
                         lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_top_at(font, index)));
    lh_ui_canvas_fill_mask(canvas, lh_ui_font_get_glyph_at(font, index), at, color);
    return lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_advance_at(font, index));
}

lh_ui_scalar_t
lh_ui_text_draw_code(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, lh_u32_t code,
                     lh_ui_point_t origin, const lh_ui_color_t *color)
{
    lh_u32_t index = 0U;

    lh_return_if(!lh_ui_font_find_index(font, code, lh_addr_of(index)), lh_ui_scalar(0));
    return lh_ui_text_draw_glyph_at(canvas, font, index, origin, color);
}

lh_void
lh_ui_text_draw_laid_line(lh_ui_canvas_t *canvas, const lh_ui_font_t *font,
                          const lh_ui_text_line_t *line, lh_ui_point_t origin,
                          const lh_ui_color_t *color)
{
    const lh_char_t *cursor = lh_ui_text_line_get_begin(line);
    const lh_char_t *end = lh_ui_text_line_get_end(line);
    lh_ui_rect_t box;

    lh_ui_rect_init(lh_addr_of(box), lh_ui_point_get_x(lh_addr_of(origin)),
                    lh_ui_point_get_y(lh_addr_of(origin)), lh_ui_text_line_get_width(line),
                    lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font)));
    /* The ink sits inside the line box (Roboto 16: line 22, the highest glyph
       starts at row 2). A clip that holds the whole box holds the ink, and
       measuring the ink first would walk every letter to learn that. A clip
       that only cuts the box still asks the ink: the box is a quarter taller
       than the letters, and the empty part of it is not a reason to draw. A
       clip that misses the box may still catch ink that hangs past it, so that
       case asks the ink too. */
    if (!lh_ui_canvas_covers_rect(canvas, lh_addr_of(box)))
    {
        lh_ui_text_line_t inked = lh_ptr_deref(line);
        lh_ui_rect_t ink;

        lh_ui_text_line_measure_ink(lh_addr_of(inked), font);
        lh_ui_text_line_get_ink_rect(lh_addr_of(inked), origin, lh_addr_of(ink));
        lh_return_if(!lh_ui_canvas_shows_rect(canvas, lh_addr_of(ink)));
    }
    while (cursor < end)
    {
        const lh_u32_t code = lh_ui_text_next_code(lh_addr_of(cursor));

        lh_ui_point_offset(lh_addr_of(origin),
                           lh_ui_text_draw_code(canvas, font, code, origin, color), lh_ui_scalar(0),
                           lh_addr_of(origin));
    }
}

lh_void
lh_ui_text_draw_line(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *line,
                     lh_ui_point_t origin, const lh_ui_color_t *color)
{
    lh_ui_text_line_t laid;

    lh_return_if(!lh_ui_text_line_measure(lh_addr_of(laid), font, line, lh_ui_scalar(0)));
    lh_ui_text_draw_laid_line(canvas, font, lh_addr_of(laid), origin, color);
}

lh_void
lh_ui_text_draw_wrapped_from(lh_ui_canvas_t *canvas, const lh_ui_font_t *font,
                             const lh_char_t *text, lh_ui_point_t origin,
                             const lh_ui_color_t *color, lh_ui_scalar_t ink_top,
                             lh_ui_scalar_t width, lh_ui_text_align_h_t align)
{
    const lh_ui_scalar_t line_height =
        lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));
    const lh_ui_scalar_t block = align == lh_ui_text_align_h_left
                                     ? lh_ui_scalar(0)
                                     : lh_ui_text_get_wrapped_width(font, text, width);
    lh_ui_point_t box;
    lh_ui_point_offset(lh_addr_of(origin), lh_ui_scalar(0), lh_math_neg(ink_top), lh_addr_of(box));
    lh_ui_text_line_t laid;

    /* @p origin is where the ink goes, so the first line box starts that far
       above it; each next one is a line height lower. The pixels land where
       ::lh_ui_text_get_ink_rect_wrapped says, which is the whole point. */
    for (; lh_ui_text_line_measure(lh_addr_of(laid), font, text, width);
         text = lh_ui_text_line_get_next(lh_addr_of(laid)))
    {
        const lh_ui_scalar_t shift = lh_ui_text_line_get_shift(lh_addr_of(laid), block, align);

        lh_ui_point_t point1;
        lh_ui_point_offset(lh_addr_of(box), shift, lh_ui_scalar(0), lh_addr_of(point1));
        lh_ui_text_draw_laid_line(canvas, font, lh_addr_of(laid), point1, color);
        lh_ui_point_offset(lh_addr_of(box), lh_ui_scalar(0), line_height, lh_addr_of(box));
    }
}

lh_void
lh_ui_text_draw_from(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text,
                     lh_ui_point_t origin, const lh_ui_color_t *color, lh_ui_scalar_t ink_top)
{
    lh_ui_text_draw_wrapped_from(canvas, font, text, origin, color, ink_top, lh_ui_scalar(0),
                                 lh_ui_text_align_h_left);
}

lh_void
lh_ui_text_draw(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text,
                lh_ui_point_t origin, const lh_ui_color_t *color)
{
    lh_ui_text_draw_from(canvas, font, text, origin, color, lh_ui_text_get_ink_top(font, text));
}

lh_void
lh_ui_text_draw_wrapped(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text,
                        lh_ui_point_t origin, const lh_ui_color_t *color, lh_ui_scalar_t width,
                        lh_ui_text_align_h_t align)
{
    lh_ui_text_block_t block;

    lh_ui_text_block_measure(lh_addr_of(block), font, text, width);
    lh_ui_text_draw_wrapped_from(canvas, font, text, origin, color,
                                 lh_ui_text_block_get_ink_top(lh_addr_of(block)), width, align);
}
