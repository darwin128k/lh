/**
 * @file caret.c
 * @brief Implementation of `lh/gl/input.h`: the caret: where it may stand, moving it, and drawing
 * it.
 */

#include <lh/gl/input.h>

#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/char/map.h>
#include <lh/core/str/utf8.h>
#include <lh/core/util/return.h>

/* Boundaries.
 *
 * Every caret position in this file is a byte offset that is **on** a code point,
 * and these two are what keeps it there. A field that steps back over one byte of a
 * two-byte character puts the caret inside it; the width of half a character is a
 * half-pixel of nothing, and the next character typed lands between two bytes that
 * together are not a character.
 */
lh_u16_t
lh_gl_input_boundary_before(const lh_gl_input_t *self, lh_u16_t caret)
{
    lh_u16_t at = caret;

    while (at > 0U && (lh_gl_input_get_buffer(self)[at - 1U] & 0xC0) == 0x80)
    {
        --at;
    }
    return at > 0U ? lh_cast_static(lh_u16_t, at - 1U) : lh_cast_static(lh_u16_t, 0U);
}

lh_bool_t
lh_gl_input_is_boundary(const lh_gl_input_t *self, lh_u16_t at)
{
    lh_return_if(at > lh_gl_input_get_used(self), lh_bool_false);
    /* The end of the text is the end of the last character, and the byte there is the
       terminator: neither a lead byte nor a continuation, so it needs no case of its
       own. */
    return at == 0U || (lh_gl_input_get_buffer(self)[at] & 0xC0) != 0x80 ? lh_bool_true
                                                                         : lh_bool_false;
}

lh_void
lh_gl_input_home(lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    self->caret = 0U;
}

lh_void
lh_gl_input_end(lh_gl_input_t *self)
{
    lh_gl_input_set_caret(self, lh_gl_input_get_used(self));
}

lh_gl_scalar_t
lh_gl_input_get_line_top(const lh_gl_input_t *self, const lh_gl_font_t *font)
{
    const lh_gl_entity_t *entity =
        lh_gl_container_get_entity(lh_gl_label_get_container(lh_gl_input_get_label(self)));
    const lh_gl_rect_t *box = lh_gl_entity_get_rect(entity);
    const lh_gl_scalar_t ascent = lh_cast_static(lh_gl_scalar_t, lh_gl_font_get_ascent(font));
    /* A run with no height (".", a space) stands on nothing, and the label says so with
       a negative baseline: that takes the same answer as an empty field. */
    const lh_gl_scalar_t own = lh_gl_input_get_used(self) > 0U
                                   ? lh_gl_label_get_baseline(lh_gl_input_get_label(self))
                                   : lh_gl_scalar(-1);
    lh_gl_scalar_t baseline;

    if (own >= lh_gl_scalar(0))
    {
        baseline = lh_gl_point_get_y(lh_gl_rect_get_origin(box)) + own;
    }
    else
    {
        const lh_gl_style_t *style = lh_gl_entity_get_style(entity);
        lh_gl_insets_t padding;
        lh_gl_entity_get_padding(entity, lh_addr_of(padding));
        const lh_gl_scalar_t cap = lh_cast_static(lh_gl_scalar_t, lh_gl_font_get_cap_height(font));
        lh_gl_size_t size;
        lh_gl_point_t origin;

        lh_gl_size_init(lh_addr_of(size), 0, cap);
        lh_gl_text_align_get_origin(box, lh_addr_of(padding), size, lh_gl_text_align_h_left,
                                    lh_null_eq(style) ? lh_gl_text_align_v_top
                                                      : lh_gl_style_get_align_v(style),
                                    lh_addr_of(origin));
        baseline = lh_gl_point_get_y(lh_addr_of(origin)) + cap;
    }
    return baseline - ascent;
}

lh_bool_t
lh_gl_input_set_caret_v(lh_gl_input_t *self, lh_u16_t caret)
{
    lh_return_if(lh_null_eq(lh_gl_input_get_buffer(self)), lh_bool_false);
    /* A refusal leaves the caret where it was. A caller with a byte that is not
       there has a number wrong, and moving the caret to the nearest boundary would
       hand it a different wrong number that looks right. */
    lh_return_if(!lh_gl_input_is_boundary(self, caret), lh_bool_false);
    lh_gl_input_set_caret(self, caret);
    return lh_bool_true;
}

lh_void
lh_gl_input_get_caret_rect(const lh_gl_input_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_scalar_t width = lh_gl_scalar(1);
    lh_gl_rect_t caret;

    lh_gl_rect_init_empty(lh_addr_of(caret));
    if (lh_null_eq(lh_gl_input_get_buffer(self)))
    {
        lh_ptr_deref(out) = caret;
        return;
    }
    const lh_gl_font_t *font = lh_gl_label_get_font(lh_gl_input_get_label(self));
    if (lh_null_eq(font))
    {
        lh_ptr_deref(out) = caret;
        return;
    }
    lh_gl_point_t origin;

    lh_gl_label_get_text_origin(lh_gl_input_get_label(self), lh_addr_of(origin));
    /* The width of the text **before** the caret, measured by cutting the buffer at
       the caret and putting the byte back. A prefix is a string only while it is cut
       off, and the input owns what is in its buffer -- which is the whole reason it
       has one instead of the pointer a label keeps. */
    lh_char_t saved = lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)];

    lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)] = lh_char_map_nul;
    lh_gl_scalar_t at = lh_gl_point_get_x(lh_addr_of(origin)) +
                        lh_gl_text_get_width(font, lh_gl_input_get_buffer(self));
    lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)] = saved;

    if (lh_gl_input_get_caret_shape(self) == lh_gl_input_caret_block)
    {
        /* The character under the caret, or a '0' past the end: a block as wide as
           nothing is a bar, and the end of the text is where a block sits most. */
        const lh_char_t *next =
            lh_addr_of(lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)]);
        const lh_u32_t code = lh_gl_input_get_caret(self) < lh_gl_input_get_used(self)
                                  ? lh_str_utf8_get_next_code(lh_addr_of(next))
                                  : lh_char_map_digit_0;

        width = lh_cast_static(lh_gl_scalar_t, lh_gl_font_get_advance(font, code));
        if (width <= lh_gl_scalar(0))
        {
            width =
                lh_cast_static(lh_gl_scalar_t, lh_gl_font_get_advance(font, lh_char_map_digit_0));
        }
        if (width <= lh_gl_scalar(0))
        {
            width = lh_gl_scalar(1);
        }
    }
    /* The font's line, standing on the text's baseline: the same height whatever the
       text is, and where the line is rather than where the ink happens to be. One
       column for a bar, not rounded away: a caret on a half pixel is a grey line half
       as visible as the one it was. */
    lh_gl_rect_init(lh_addr_of(caret), at, lh_gl_input_get_line_top(self, font), width,
                    lh_cast_static(lh_gl_scalar_t, lh_gl_font_get_line_height(font)));
    lh_ptr_deref(out) = caret;
}

lh_void
lh_gl_input_on_draw_caret(const lh_gl_input_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_draw);
    /* A caret that is not focused is not drawn. That is the whole of "focused" as
       far as this component is concerned, and it is why the field needs no clock. */
    lh_return_if(lh_gl_input_is_focused(self) == lh_bool_false);
    const lh_gl_color_t *color = lh_gl_input_get_caret_color(self);
    lh_return_if(lh_null_eq(color));
    lh_gl_rect_t caret;
    lh_gl_input_get_caret_rect(self, lh_addr_of(caret));
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(caret)));
    lh_gl_canvas_t *canvas = lh_gl_entity_event_get_canvas(event);
    lh_return_if(lh_null_eq(canvas));
    lh_gl_canvas_fill_rect(canvas, lh_addr_of(caret), color);
    if (lh_gl_input_get_caret_shape(self) == lh_gl_input_caret_block &&
        lh_gl_input_get_caret(self) < lh_gl_input_get_used(self))
    {
        /* The block covers the character it stands on, so the character is drawn
           again on top of it in the colour that is behind the text -- the field's
           fill, or white for a field without one. Same font, same line top, same
           column: the glyph lands exactly where the label put it. */
        const lh_gl_font_t *font = lh_gl_label_get_font(lh_gl_input_get_label(self));
        const lh_gl_style_t *style = lh_gl_entity_get_style(
            lh_gl_container_get_entity(lh_gl_label_get_container(lh_gl_input_get_label(self))));
        const lh_gl_color_t *under =
            lh_null_eq(style) ? lh_null : lh_gl_paint_get_color_v(lh_gl_style_get_fill(style));
        const lh_char_t *at = lh_addr_of(lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)]);
        lh_gl_color_t white;

        lh_gl_color_init_argb(lh_addr_of(white), 0xFFFFFFFFU);
        LH_ATTRIBUTE_UNUSED(
            lh_gl_text_draw_code(canvas, font, lh_str_utf8_get_next_code(lh_addr_of(at)),
                                 lh_ptr_deref(lh_gl_rect_get_origin(lh_addr_of(caret))),
                                 lh_null_eq(under) ? lh_addr_of(white) : under));
    }
}
