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
    lh_u32_t code;
    lh_u32_t left;
    lh_u32_t at;
    const lh_byte_t first = lh_cast_static(lh_byte_t, **cursor);

    lh_return_if(first == 0U, 0U);
    if (first < 0x80U)
    {
        ++*cursor;
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
        ++*cursor;
        return lh_cast_static(lh_u32_t, first);
    }
    ++*cursor;
    for (at = 0U; at < left; ++at)
    {
        const lh_byte_t next = lh_cast_static(lh_byte_t, **cursor);

        if ((next & 0xC0U) != 0x80U)
        {
            /* Truncated at the end of the string: what has been read is what there
               is, and the cursor stays on the byte that is not a continuation. */
            return code;
        }
        code = (code << 6) | lh_cast_static(lh_u32_t, next & 0x3FU);
        ++*cursor;
    }
    return code;
}

lh_bool_t
lh_ui_text_is_line_end(lh_u32_t code)
{
    return code == 0U || code == lh_cast_static(lh_u32_t, '\n') ? lh_bool_true : lh_bool_false;
}

/* ── Writing ────────────────────────────────────────────────────────────────
 *
 * The engine decodes code points but never had to write one: everything drawn so
 * far was a string the application had already encoded. A text **field** is the
 * first thing that has to put a typed code point into a buffer, and the byte at the
 * caret is written before the rest is shifted right -- so the width of a code point
 * and the bytes of it belong here, next to the reading, rather than in the input.
 */

lh_u32_t
lh_ui_text_encoded_size(lh_u32_t code)
{
    if (code == 0U || code > 0x10FFFFU || (code >= 0xD800U && code <= 0xDFFFU))
    {
        return 0U;
    }
    if (code < 0x80U)
    {
        return 1U;
    }
    if (code < 0x800U)
    {
        return 2U;
    }
    return code < 0x10000U ? 3U : 4U;
}

lh_u32_t
lh_ui_text_lead_size(lh_byte_t lead)
{
    /* Two functions that look like one, and the difference between them is a bug that
       only shows up outside ASCII. ::lh_ui_text_encoded_size answers for a **code
       point**; this one answers for the **first byte** of an encoded character, which
       is what you actually hold when you walk a buffer somebody else filled in.

       Handing a lead byte to the other function works by coincidence and lies:
       `0xD0` (a two-byte lead) is the code point `0xD0`, which is under `0x800`, so
       the two agree -- and every Cyrillic letter agrees with them, which is why a
       suite written with Cyrillic in it stayed green. `0xE0` is a **three**-byte lead,
       and as a code point it is also under `0x800`, so the wrong answer of two comes
       back: a field deleting a character like this removes two of its three bytes and
       leaves the tail of a code point behind as the next character. Same for `0xF0`.
       Our own encoder and our own decoder agreed with each other and both disagreed
       with UTF-8 -- the one thing a round-trip test cannot see. */
    if (lead < 0x80U)
    {
        return 1U;
    }
    if ((lead & 0xE0U) == 0xC0U)
    {
        return 2U;
    }
    if ((lead & 0xF0U) == 0xE0U)
    {
        return 3U;
    }
    if ((lead & 0xF8U) == 0xF0U)
    {
        return 4U;
    }
    /* A continuation byte, or anything that is not a lead byte at all. One is as far
       as it is safe to go: skipping more would step over a character somebody can see,
       and ::lh_ui_text_next_code already advances one byte at a time over these. */
    return 1U;
}

lh_u32_t
lh_ui_text_encode_code(lh_char_t *out, lh_u32_t bytes, lh_u32_t code)
{
    const lh_u32_t size = lh_ui_text_encoded_size(code);

    lh_return_if(lh_null_eq(out), 0U);
    lh_return_if(size == 0U || bytes < size, 0U);

    switch (size)
    {
        case 1U:
            out[0] = lh_cast_static(lh_char_t, code);
            break;
        case 2U:
            out[0] = lh_cast_static(lh_char_t, 0xC0U | (code >> 6));
            out[1] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
            break;
        case 3U:
            out[0] = lh_cast_static(lh_char_t, 0xE0U | (code >> 12));
            out[1] = lh_cast_static(lh_char_t, 0x80U | ((code >> 6) & 0x3FU));
            out[2] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
            break;
        default:
            out[0] = lh_cast_static(lh_char_t, 0xF0U | (code >> 18));
            out[1] = lh_cast_static(lh_char_t, 0x80U | ((code >> 12) & 0x3FU));
            out[2] = lh_cast_static(lh_char_t, 0x80U | ((code >> 6) & 0x3FU));
            out[3] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
            break;
    }
    return size;
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
    /* The cap line down to the baseline, not the line box and not the ink.
       A line box is taller than what is drawn (Roboto 16 px: line 22, ascent 17,
       so its ink starts five rows down) and centring that drew every caption a
       few pixels low. The ink is the other end of the same mistake: it is as
       tall as the tallest and the lowest letter of *this* word, so centring it
       puts the text on a half pixel it cannot win either way — measured on the
       demo's 28-row Hide panel button, 15 rows of ink in 16 rows of room is a
       tie, and truncating it left the text half a pixel high. The cap line is
       the one measure no half pixel is left over in (16 - 12 = 4, even), and it
       is a metric of the font rather than of the word, so a caption sits at the
       same height whatever it says. This is LVGL's LV_TEXT_LEADING_TRIM_CAPITAL_BASELINE
       and CSS's text-box-trim: trim the box to what the letters actually carry. */
    lh_ui_size_t size;
    lh_ui_point_t origin;
    lh_ui_rect_t ink;
    lh_ui_scalar_t lines;

    lh_ui_point_init(lh_addr_of(origin), lh_ui_scalar(0), lh_ui_scalar(0));
    ink = lh_ui_text_get_ink_rect(font, text, origin);
    size = *lh_ui_rect_get_size_as_const(lh_addr_of(ink));
    /* Nothing inked is nothing drawn, and a label with nothing in it collapses
       however tall its font is. */
    lines = lh_cast_static(lh_ui_scalar_t, lh_ui_text_count_lines(text));
    lh_ui_size_set_height(lh_addr_of(size),
                          lh_ui_rect_is_empty(lh_addr_of(ink))
                              ? lh_ui_scalar(0)
                              : (lines - lh_ui_scalar(1)) * lh_ui_scalar(lh_ui_font_get_line_height(font)) +
                                    lh_ui_scalar(lh_ui_font_get_cap_height(font)));
    return size;
}

lh_ui_rect_t
lh_ui_text_get_line_ink_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin)
{
    const lh_char_t *cursor = line;
    lh_ui_scalar_t top = lh_ui_scalar(0);
    lh_ui_scalar_t bottom = lh_ui_scalar(0);
    lh_ui_scalar_t width = lh_ui_scalar(0);
    lh_bool_t inked = lh_bool_false;
    lh_ui_rect_t rect;
    lh_u32_t code;

    for (code = lh_ui_text_next_code(lh_addr_of(cursor)); !lh_ui_text_is_line_end(code);
         code = lh_ui_text_next_code(lh_addr_of(cursor)))
    {
        const lh_ui_scalar_t at = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ink_top(font, code));
        const lh_ui_scalar_t to = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ink_bottom(font, code));

        /* The pen moves for a space too. Measuring the width afterwards walked
           the same letters a second time, and a label asks this on every frame. */
        width += lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_advance(font, code));
        if (to <= at) /* a space, or a glyph the font has no mask for */
        {
            continue;
        }
        if (!inked || at < top)
        {
            top = at;
        }
        if (!inked || to > bottom)
        {
            bottom = to;
        }
        inked = lh_bool_true;
    }
    /* Width stays the pen's: a trailing space takes room whether or not it is
       inked, and that is what every other toolkit measures too. */
    lh_ui_rect_init(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(origin)),
                    lh_ui_point_get_y(lh_addr_of(origin)) + top, width, bottom - top);
    return rect;
}

lh_ui_rect_t
lh_ui_text_get_ink_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin)
{
    const lh_ui_scalar_t line_height = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));
    const lh_ui_scalar_t first = lh_ui_text_get_ink_top(font, text);
    lh_ui_scalar_t down = lh_ui_scalar(0);
    lh_ui_rect_t ink;

    lh_ui_rect_init_empty(lh_addr_of(ink));
    for (; lh_null_ne(text); text = lh_ui_text_get_next_line(text))
    {
        /* Every line box starts that far above @p origin, because @p origin is
           where the ink is and a line box is taller than what it draws. */
        const lh_ui_point_t box = lh_ui_point_offset(lh_addr_of(origin), lh_ui_scalar(0), down - first);
        const lh_ui_rect_t line_ink = lh_ui_text_get_line_ink_rect(font, text, box);

        /* A hull, and the right one here: the lines all start at the same x, so
           the hull is as wide as the widest of them and as tall as the first ink
           to the last. The gap between two lines is inside it and is not drawn
           into, which is what a line box is for anyway. */
        ink = lh_ui_rect_union(lh_addr_of(ink), lh_addr_of(line_ink));
        down += line_height;
    }
    return ink;
}

lh_ui_scalar_t
lh_ui_text_get_ink_top(const lh_ui_font_t *font, const lh_char_t *text)
{
    const lh_ui_scalar_t line_height = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));
    lh_ui_point_t origin_zero;
    lh_ui_scalar_t down = lh_ui_scalar(0);
    lh_ui_scalar_t best = lh_ui_scalar(0);
    lh_bool_t inked = lh_bool_false;

    lh_ui_point_init(lh_addr_of(origin_zero), lh_ui_scalar(0), lh_ui_scalar(0));
    for (; lh_null_ne(text); text = lh_ui_text_get_next_line(text))
    {
        const lh_ui_point_t box = lh_ui_point_offset(lh_addr_of(origin_zero), lh_ui_scalar(0), down);
        const lh_ui_rect_t line_ink = lh_ui_text_get_line_ink_rect(font, text, box);
        const lh_ui_scalar_t at = lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(line_ink)));

        if (lh_ui_rect_is_empty(lh_addr_of(line_ink)))
        {
            continue;
        }
        if (!inked || at < best)
        {
            best = at;
        }
        inked = lh_bool_true;
    }
    return inked ? best : lh_ui_scalar(0);
}

lh_ui_rect_t
lh_ui_text_get_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin)
{
    return lh_ui_text_get_ink_rect(font, text, origin);
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
    lh_ui_point_t at;

    if (lh_ui_font_get_glyph(font, code, lh_addr_of(glyph)))
    {
        /* origin is the line top-left; baseline is ascent down; mask top is relative to it. */
        lh_ui_point_init(lh_addr_of(at), lh_ui_point_get_x(lh_addr_of(origin)),
                         lh_ui_point_get_y(lh_addr_of(origin)) +
                             lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ascent(font)) +
                             lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_top(font, code)));
        lh_ui_canvas_fill_mask(canvas, lh_addr_of(glyph), at, color);
    }
    return lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_advance(font, code));
}

lh_void
lh_ui_text_draw_line(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *line,
                     lh_ui_point_t origin, const lh_ui_color_t *color)
{
    const lh_ui_rect_t box = lh_ui_text_get_line_rect(font, line, origin);
    const lh_char_t *cursor = line;
    lh_u32_t code;

    /* The ink sits inside the line box (Roboto 16: line 22, the highest glyph
       starts at row 2). A clip that holds the whole box holds the ink, and
       measuring the ink first would walk every letter to learn that. A clip
       that only cuts the box still asks the ink: the box is a quarter taller
       than the letters, and the empty part of it is not a reason to draw. A
       clip that misses the box may still catch ink that hangs past it, so that
       case asks the ink too. */
    if (!lh_ui_canvas_covers_rect(canvas, lh_addr_of(box)))
    {
        const lh_ui_rect_t ink = lh_ui_text_get_line_ink_rect(font, line, origin);

        lh_return_if(!lh_ui_canvas_shows_rect(canvas, lh_addr_of(ink)));
    }
    for (code = lh_ui_text_next_code(lh_addr_of(cursor)); !lh_ui_text_is_line_end(code);
         code = lh_ui_text_next_code(lh_addr_of(cursor)))
    {
        origin = lh_ui_point_offset(lh_addr_of(origin), lh_ui_text_draw_code(canvas, font, code, origin, color),
                                    lh_ui_scalar(0));
    }
}

lh_void
lh_ui_text_draw_from(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                     const lh_ui_color_t *color, lh_ui_scalar_t ink_top)
{
    const lh_ui_scalar_t line_height = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));
    lh_ui_point_t box = lh_ui_point_offset(lh_addr_of(origin), lh_ui_scalar(0), -ink_top);

    /* @p origin is where the ink goes, so the first line box starts that far
       above it; each next one is a line height lower. The pixels land where
       ::lh_ui_text_get_ink_rect says, which is the whole point. */
    for (; lh_null_ne(text); text = lh_ui_text_get_next_line(text))
    {
        lh_ui_text_draw_line(canvas, font, text, box, color);
        box = lh_ui_point_offset(lh_addr_of(box), lh_ui_scalar(0), line_height);
    }
}

lh_void
lh_ui_text_draw(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                const lh_ui_color_t *color)
{
    lh_ui_text_draw_from(canvas, font, text, origin, color, lh_ui_text_get_ink_top(font, text));
}
