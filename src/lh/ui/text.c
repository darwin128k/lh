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
    lh_bool_t inked = lh_bool_false;
    lh_ui_rect_t rect;
    lh_u32_t code;

    for (code = lh_ui_text_next_code(lh_addr_of(cursor)); !lh_ui_text_is_line_end(code);
         code = lh_ui_text_next_code(lh_addr_of(cursor)))
    {
        const lh_ui_scalar_t at = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ink_top(font, code));
        const lh_ui_scalar_t to = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ink_bottom(font, code));

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
                    lh_ui_point_get_y(lh_addr_of(origin)) + top, lh_ui_text_get_line_width(font, line),
                    bottom - top);
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
    const lh_ui_rect_t rect = lh_ui_text_get_line_ink_rect(font, line, origin);
    const lh_char_t *cursor = line;
    lh_u32_t code;

    /* On the ink and not on the line box: the box is a quarter taller than what
       gets drawn, and skipping a line because the clip shows the empty part of
       it would drop text that is on the screen. */

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
    lh_ui_point_t box = lh_ui_point_offset(
        lh_addr_of(origin), lh_ui_scalar(0), -lh_ui_text_get_ink_top(font, text));

    /* @p origin is where the ink goes, so the first line box starts that far
       above it; each next one is a line height lower. The pixels land where
       ::lh_ui_text_get_ink_rect says, which is the whole point. */
    for (; lh_null_ne(text); text = lh_ui_text_get_next_line(text))
    {
        lh_ui_text_draw_line(canvas, font, text, box, color);
        box = lh_ui_point_offset(lh_addr_of(box), lh_ui_scalar(0), line_height);
    }
}
