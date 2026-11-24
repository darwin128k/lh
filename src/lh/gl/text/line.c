/**
 * @file line.c
 * @brief Implementation of `lh/gl/text/line.h`.
 */
#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/cast/const.h>

#include <lh/core/math.h>
#include <lh/core/char/map.h>
#include <lh/gl/text.h>
#include <lh/core/str/utf8.h>
#include <lh/core/util/return.h>

lh_s32_t
lh_gl_text_line_get_code_advance(lh_ptr context, lh_u32_t code)
{
    const lh_gl_font_t *font = lh_ptr_rcast(const lh_gl_font_t, context);
    lh_u32_t index = 0U;

    lh_return_if(!lh_gl_font_find_index(font, code, lh_addr_of(index)), 0);
    return lh_cast_static(lh_s32_t, lh_gl_font_get_advance_at(font, index));
}

lh_s32_t
lh_gl_text_line_get_wrap_width(lh_gl_scalar_t width)
{
    const lh_s32_t whole = lh_gl_scalar_floor_s32(width);

    /* Not `max(width, 0)`: GCC 16.1 at -O2 with LTO derives wrong known bits for a
       MAX_EXPR passed through to a callee (IPA bit-CP marks all 32 bits as known
       zero) and folds the breaking away. A width of 0 or less already means "no
       limit" to the core, so only a fraction of a pixel needs lifting to 1. */
    lh_return_if(whole == 0 && width > lh_gl_scalar(0), 1);
    return whole;
}

const lh_char_t *
lh_gl_text_line_get_begin(const lh_gl_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->begin;
}

const lh_char_t *
lh_gl_text_line_get_end(const lh_gl_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->end;
}

const lh_char_t *
lh_gl_text_line_get_next(const lh_gl_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_gl_scalar_t
lh_gl_text_line_get_width(const lh_gl_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_gl_scalar_t
lh_gl_text_line_get_ink_top(const lh_gl_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ink_top;
}

lh_gl_scalar_t
lh_gl_text_line_get_ink_bottom(const lh_gl_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ink_bottom;
}

lh_bool_t
lh_gl_text_line_is_inked(const lh_gl_text_line_t *self)
{
    lh_assert_runtime_ref(self);
    return self->inked;
}

lh_void
lh_gl_text_line_set_begin(lh_gl_text_line_t *self, const lh_char_t *begin)
{
    lh_assert_runtime_ref(self);
    self->begin = begin;
}

lh_void
lh_gl_text_line_set_end(lh_gl_text_line_t *self, const lh_char_t *end)
{
    lh_assert_runtime_ref(self);
    self->end = end;
}

lh_void
lh_gl_text_line_set_next(lh_gl_text_line_t *self, const lh_char_t *next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_void
lh_gl_text_line_set_width(lh_gl_text_line_t *self, lh_gl_scalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_gl_text_line_set_ink_top(lh_gl_text_line_t *self, lh_gl_scalar_t ink_top)
{
    lh_assert_runtime_ref(self);
    self->ink_top = ink_top;
}

lh_void
lh_gl_text_line_set_ink_bottom(lh_gl_text_line_t *self, lh_gl_scalar_t ink_bottom)
{
    lh_assert_runtime_ref(self);
    self->ink_bottom = ink_bottom;
}

lh_void
lh_gl_text_line_set_inked(lh_gl_text_line_t *self, lh_bool_t inked)
{
    lh_assert_runtime_ref(self);
    self->inked = inked;
}

lh_void
lh_gl_text_line_init(lh_gl_text_line_t *self)
{
    lh_gl_text_line_set_begin(self, lh_null);
    lh_gl_text_line_set_end(self, lh_null);
    lh_gl_text_line_set_next(self, lh_null);
    lh_gl_text_line_set_width(self, lh_gl_scalar(0));
    lh_gl_text_line_set_ink_top(self, lh_gl_scalar(0));
    lh_gl_text_line_set_ink_bottom(self, lh_gl_scalar(0));
    lh_gl_text_line_set_inked(self, lh_bool_false);
}

lh_void
lh_gl_text_line_take_ink(lh_gl_text_line_t *self, const lh_gl_font_t *font, lh_u32_t index)
{
    const lh_gl_scalar_t at = lh_cast_static(
        lh_gl_scalar_t, lh_gl_font_get_ascent(font) + lh_gl_font_get_top_at(font, index));
    const lh_gl_scalar_t to =
        at +
        lh_cast_static(lh_gl_scalar_t, lh_gl_mask_get_height(lh_gl_font_get_glyph_at(font, index)));

    lh_return_if(to <= at); /* a space: it moves the pen and draws nothing */
    if (!lh_gl_text_line_is_inked(self) || at < lh_gl_text_line_get_ink_top(self))
    {
        lh_gl_text_line_set_ink_top(self, at);
    }
    if (!lh_gl_text_line_is_inked(self) || to > lh_gl_text_line_get_ink_bottom(self))
    {
        lh_gl_text_line_set_ink_bottom(self, to);
    }
    lh_gl_text_line_set_inked(self, lh_bool_true);
}

lh_void
lh_gl_text_line_get_ink_rect(const lh_gl_text_line_t *self, lh_gl_point_t origin, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), lh_gl_point_get_x(lh_addr_of(origin)),
                    lh_gl_point_get_y(lh_addr_of(origin)) + lh_gl_text_line_get_ink_top(self),
                    lh_gl_text_line_get_width(self),
                    lh_gl_text_line_get_ink_bottom(self) - lh_gl_text_line_get_ink_top(self));
    lh_ptr_deref(out) = rect;
}

lh_gl_scalar_t
lh_gl_text_line_get_shift(const lh_gl_text_line_t *self, lh_gl_scalar_t block,
                          lh_gl_text_align_h_t align)
{
    const lh_gl_scalar_t width = lh_gl_text_line_get_width(self);

    if (align == lh_gl_text_align_h_center)
    {
        return lh_gl_scalar_centre(block, width);
    }
    if (align == lh_gl_text_align_h_right)
    {
        return lh_gl_scalar_slack(block, width);
    }
    return lh_gl_scalar(0);
}

lh_bool_t
lh_gl_text_line_measure(lh_gl_text_line_t *self, const lh_gl_font_t *font, const lh_char_t *text,
                        lh_gl_scalar_t width)
{
    lh_str_wrap_t line;

    lh_gl_text_line_init(self);
    lh_return_if(lh_null_eq(text), lh_bool_false);
    lh_assert_runtime_ref(font);
    LH_ATTRIBUTE_UNUSED(
        lh_str_wrap_measure(lh_addr_of(line), text, lh_gl_text_line_get_wrap_width(width),
                            lh_gl_text_line_get_code_advance, lh_cast_const(lh_gl_font_t *, font)));
    lh_gl_text_line_set_begin(self, lh_str_wrap_get_begin(lh_addr_of(line)));
    lh_gl_text_line_set_end(self, lh_str_wrap_get_end(lh_addr_of(line)));
    lh_gl_text_line_set_next(self, lh_str_wrap_get_next(lh_addr_of(line)));
    lh_gl_text_line_set_width(
        self, lh_cast_static(lh_gl_scalar_t, lh_str_wrap_get_width(lh_addr_of(line))));
    return lh_bool_true;
}

lh_void
lh_gl_text_line_measure_ink(lh_gl_text_line_t *self, const lh_gl_font_t *font)
{
    const lh_char_t *cursor = lh_gl_text_line_get_begin(self);
    const lh_char_t *end = lh_gl_text_line_get_end(self);

    lh_assert_runtime_ref(font);
    lh_gl_text_line_set_ink_top(self, lh_gl_scalar(0));
    lh_gl_text_line_set_ink_bottom(self, lh_gl_scalar(0));
    lh_gl_text_line_set_inked(self, lh_bool_false);
    /* A second walk, over the bytes the layout already settled: no breaking left
       to decide, only which rows the letters ink. Asking it inside the layout made
       every width -- the measure a layout asks most -- pay for ink it never read. */
    while (cursor < end)
    {
        lh_u32_t index = 0U;

        if (lh_gl_font_find_index(font, lh_str_utf8_get_next_code(lh_addr_of(cursor)),
                                  lh_addr_of(index)))
        {
            lh_gl_text_line_take_ink(self, font, index);
        }
    }
}
