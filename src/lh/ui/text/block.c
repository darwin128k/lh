/**
 * @file block.c
 * @brief Implementation of `lh/ui/text/block.h`.
 */

#include <lh/ui/text/block.h>

#include <lh/math.h>
#include <lh/ui/text/line.h>

lh_void
lh_ui_text_block_init(lh_ui_text_block_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_rect_init_empty(lh_addr_of(self->ink));
    self->width = lh_ui_scalar(0);
    self->ink_top = lh_ui_scalar(0);
    self->lines = 0U;
}

lh_void
lh_ui_text_block_measure_layout(lh_ui_text_block_t *self, const lh_ui_font_t *font,
                                const lh_char_t *text, lh_ui_scalar_t width)
{
    lh_ui_text_line_t laid;

    lh_ui_text_block_init(self);
    for (; lh_ui_text_line_measure(lh_addr_of(laid), font, text, width);
         text = lh_ui_text_line_get_next(lh_addr_of(laid)))
    {
        self->width = lh_math_max(self->width, lh_ui_text_line_get_width(lh_addr_of(laid)));
        ++self->lines;
    }
}

lh_void
lh_ui_text_block_measure(lh_ui_text_block_t *self, const lh_ui_font_t *font, const lh_char_t *text,
                         lh_ui_scalar_t width)
{
    const lh_ui_scalar_t line_height =
        lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));
    lh_ui_text_line_t laid;
    lh_ui_point_t box;
    lh_bool_t inked = lh_bool_false;

    lh_ui_text_block_init(self);
    lh_ui_point_init(lh_addr_of(box), lh_ui_scalar(0), lh_ui_scalar(0));
    for (; lh_ui_text_line_measure(lh_addr_of(laid), font, text, width);
         text = lh_ui_text_line_get_next(lh_addr_of(laid)))
    {
        lh_ui_rect_t line_ink;
        lh_ui_scalar_t at;

        lh_ui_text_line_measure_ink(lh_addr_of(laid), font);
        lh_ui_text_line_get_ink_rect(lh_addr_of(laid), box, lh_addr_of(line_ink));
        at = lh_ui_text_line_get_ink_top(lh_addr_of(laid));

        /* The first ink is asked of each line from the top of its own box: it is
           what an alignment takes back. */
        if (!lh_ui_rect_is_empty(lh_addr_of(line_ink)) && (!inked || at < self->ink_top))
        {
            self->ink_top = at;
            inked = lh_bool_true;
        }
        self->width = lh_math_max(self->width, lh_ui_text_line_get_width(lh_addr_of(laid)));
        lh_ui_rect_union(lh_addr_of(self->ink), lh_addr_of(line_ink), lh_addr_of(self->ink));
        ++self->lines;
        lh_ui_point_offset(lh_addr_of(box), lh_ui_scalar(0), line_height, lh_addr_of(box));
    }
}

lh_u32_t
lh_ui_text_block_get_lines(const lh_ui_text_block_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lines;
}

lh_ui_scalar_t
lh_ui_text_block_get_width(const lh_ui_text_block_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_ui_scalar_t
lh_ui_text_block_get_ink_top(const lh_ui_text_block_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ink_top;
}

lh_void
lh_ui_text_block_get_ink_rect(const lh_ui_text_block_t *self, lh_ui_point_t origin,
                              lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(self);
    /* The union skips empty rects, so moving the hull is the hull of the moved
       lines -- including when nothing is inked and the hull is the last line's box. */
    lh_ui_rect_offset(lh_addr_of(self->ink), lh_ui_point_get_x(lh_addr_of(origin)),
                      lh_ui_point_get_y(lh_addr_of(origin)) - self->ink_top, out);
}

lh_void
lh_ui_text_block_get_size(const lh_ui_text_block_t *self, const lh_ui_font_t *font,
                          lh_ui_size_t *out)
{
    lh_assert_runtime_ref(out);
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
    lh_ui_point_t origin;
    lh_ui_rect_t ink;
    lh_ui_size_t size;

    lh_ui_point_init(lh_addr_of(origin), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_text_block_get_ink_rect(self, origin, lh_addr_of(ink));
    size = lh_ptr_deref(lh_ui_rect_get_size(lh_addr_of(ink)));
    /* Nothing inked is nothing drawn, and a label with nothing in it collapses
       however tall its font is. */
    lh_ui_size_set_height(
        lh_addr_of(size),
        lh_ui_rect_is_empty(lh_addr_of(ink))
            ? lh_ui_scalar(0)
            : (lh_cast_static(lh_ui_scalar_t, lh_ui_text_block_get_lines(self)) - lh_ui_scalar(1)) *
                      lh_ui_scalar_of(lh_ui_font_get_line_height(font)) +
                  lh_ui_scalar_of(lh_ui_font_get_cap_height(font)));
    lh_ptr_deref(out) = size;
}
