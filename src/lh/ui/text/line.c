/**
 * @file line.c
 * @brief Implementation of `lh/ui/text/line.h`.
 */

#include <lh/math.h>
#include <lh/ui/text.h>
#include <lh/util/return.h>

lh_void
lh_ui_text_line_init(lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    self->begin = lh_null;
    self->end = lh_null;
    self->next = lh_null;
    self->width = lh_ui_scalar(0);
    self->ink_top = lh_ui_scalar(0);
    self->ink_bottom = lh_ui_scalar(0);
    self->inked = lh_bool_false;
}

lh_void
lh_ui_text_line_take_ink(lh_ui_text_line_t *self, const lh_ui_font_t *font, lh_u32_t index)
{
    const lh_ui_scalar_t at = lh_cast_static(
        lh_ui_scalar_t, lh_ui_font_get_ascent(font) + lh_ui_font_get_top_at(font, index));
    const lh_ui_scalar_t to =
        at +
        lh_cast_static(lh_ui_scalar_t, lh_ui_mask_get_height(lh_ui_font_get_glyph_at(font, index)));

    lh_return_if(to <= at); /* a space: it moves the pen and draws nothing */
    if (!self->inked || at < self->ink_top)
    {
        self->ink_top = at;
    }
    if (!self->inked || to > self->ink_bottom)
    {
        self->ink_bottom = to;
    }
    self->inked = lh_bool_true;
}

const lh_char_t *
lh_ui_text_line_next_of(const lh_char_t *end, lh_ui_scalar_t width)
{
    const lh_char_t *cursor = end;
    const lh_u32_t code = lh_ui_text_next_code(lh_addr_of(cursor));

    lh_return_if(code == 0U, lh_null);
    lh_return_if(lh_ui_text_is_line_end(code), cursor);
    if (width > lh_ui_scalar(0) && code == lh_cast_static(lh_u32_t, ' '))
    {
        while (lh_ptr_deref(cursor) == ' ')
        {
            ++cursor;
        }
        lh_return_if(lh_ptr_deref(cursor) == '\0', lh_null);
        return cursor;
    }
    return end;
}

lh_bool_t
lh_ui_text_line_measure(lh_ui_text_line_t *self, const lh_ui_font_t *font, const lh_char_t *text,
                        lh_ui_scalar_t width)
{
    lh_ui_text_line_t at_space;
    lh_bool_t has_space = lh_bool_false;
    const lh_char_t *cursor = text;

    lh_ui_text_line_init(self);
    lh_return_if(lh_null_eq(text), lh_bool_false);
    lh_assert_runtime_ref(font);
    self->begin = text;
    for (;;)
    {
        const lh_char_t *before = cursor;
        const lh_u32_t code = lh_ui_text_next_code(lh_addr_of(cursor));
        lh_u32_t index = 0U;
        const lh_bool_t found = lh_ui_font_find_index(font, code, lh_addr_of(index));
        const lh_ui_scalar_t advance =
            found ? lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_advance_at(font, index))
                  : lh_ui_scalar(0);

        if (lh_ui_text_is_line_end(code))
        {
            self->end = before;
            break;
        }
        /* A space may hang past the edge: it is where the line breaks, not ink. */
        if (width > lh_ui_scalar(0) && code != lh_cast_static(lh_u32_t, ' ') &&
            self->width + advance > width)
        {
            if (has_space)
            {
                lh_ptr_deref(self) = at_space;
                break;
            }
            if (before == text)
            {
                /* A first letter wider than the line still takes it. */
                self->width += advance;
                self->end = cursor;
                break;
            }
            self->end = before;
            break;
        }
        if (code == lh_cast_static(lh_u32_t, ' '))
        {
            at_space = lh_ptr_deref(self);
            at_space.end = before;
            has_space = lh_bool_true;
        }
        self->width += advance;
    }
    self->next = lh_ui_text_line_next_of(self->end, width);
    return lh_bool_true;
}

const lh_char_t *
lh_ui_text_line_get_begin(const lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->begin;
}

const lh_char_t *
lh_ui_text_line_get_end(const lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->end;
}

lh_void
lh_ui_text_line_measure_ink(lh_ui_text_line_t *self, const lh_ui_font_t *font)
{
    const lh_char_t *cursor = lh_ui_text_line_get_begin(self);
    const lh_char_t *end = lh_ui_text_line_get_end(self);

    lh_assert_runtime_ref(font);
    self->ink_top = lh_ui_scalar(0);
    self->ink_bottom = lh_ui_scalar(0);
    self->inked = lh_bool_false;
    /* A second walk, over the bytes the layout already settled: no breaking left
       to decide, only which rows the letters ink. Asking it inside the layout made
       every width -- the measure a layout asks most -- pay for ink it never read. */
    while (cursor < end)
    {
        lh_u32_t index = 0U;

        if (lh_ui_font_find_index(font, lh_ui_text_next_code(lh_addr_of(cursor)),
                                  lh_addr_of(index)))
        {
            lh_ui_text_line_take_ink(self, font, index);
        }
    }
}

const lh_char_t *
lh_ui_text_line_get_next(const lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_ui_scalar_t
lh_ui_text_line_get_width(const lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_ui_scalar_t
lh_ui_text_line_get_ink_top(const lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ink_top;
}

lh_ui_scalar_t
lh_ui_text_line_get_ink_bottom(const lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ink_bottom;
}

lh_bool_t
lh_ui_text_line_is_inked(const lh_ui_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->inked;
}

lh_void
lh_ui_text_line_get_ink_rect(const lh_ui_text_line_t *self, lh_ui_point_t origin, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(origin)),
                    lh_ui_point_get_y(lh_addr_of(origin)) + lh_ui_text_line_get_ink_top(self),
                    lh_ui_text_line_get_width(self),
                    lh_ui_text_line_get_ink_bottom(self) - lh_ui_text_line_get_ink_top(self));
    lh_ptr_deref(out) = rect;
}

lh_ui_scalar_t
lh_ui_text_line_get_shift(const lh_ui_text_line_t *self, lh_ui_scalar_t block,
                          lh_ui_text_align_h_t align)
{
    const lh_ui_scalar_t width = lh_ui_text_line_get_width(self);

    if (align == lh_ui_text_align_h_center)
    {
        return lh_ui_scalar_centre(block, width);
    }
    if (align == lh_ui_text_align_h_right)
    {
        return lh_ui_scalar_slack(block, width);
    }
    return lh_ui_scalar(0);
}
