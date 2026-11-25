/**
 * @file event.c
 * @brief Implementation of `lh/gl/input.h`: the entity events of a field: the focus, a press and a
 * key.
 */

#include <lh/gl/input.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/str/utf8.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_input_on_focus(const lh_gl_input_t *self, const lh_gl_entity_event_t *event)
{
    lh_gl_input_t *input = lh_ptr_rcast(lh_gl_input_t, self);
    const lh_gl_entity_event_code_t code = lh_gl_entity_event_get_code(event);

    lh_return_if(code != lh_gl_entity_event_focus && code != lh_gl_entity_event_defocus);
    lh_bool_t focused = code == lh_gl_entity_event_focus ? lh_bool_true : lh_bool_false;
    /* No damage: the caret appearing is the application invalidating, the same as
       after a scroll or a state change anywhere else in the tree. */
    lh_gl_input_set_focused(input, focused);
}

lh_void
lh_gl_input_on_focusable(const lh_gl_entity_event_t *event)
{
    /* A field is focusable, and saying so is what makes it work at all: a press puts
       the focus on `lh_gl_entity_find_focusable` of whatever was hit, Tab walks the
       focusables in order, and a key goes to whoever holds the focus. A field that
       said no would be a field that cannot be clicked into, cannot be tabbed to and
       never receives a key -- with every setter in this header working perfectly on a
       widget no person and no key can ever reach. The label underneath says no, which
       is right for a label; this is the one thing a field is and a label is not. */
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_focusable);
    lh_ptr_deref(lh_gl_entity_event_get_focusable(event)) = lh_bool_true;
}

lh_bool_t
lh_gl_input_on_press(const lh_gl_input_t *self, const lh_gl_entity_event_t *event)
{
    lh_gl_input_t *input = lh_ptr_rcast(lh_gl_input_t, self);

    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_press, lh_bool_false);
    lh_return_if(lh_null_eq(lh_gl_input_get_buffer(self)), lh_bool_false);
    const lh_gl_font_t *font = lh_gl_label_get_font(lh_gl_input_get_label(self));
    lh_return_if(lh_null_eq(font), lh_bool_false);
    lh_gl_point_t origin;

    lh_gl_label_get_text_origin(lh_gl_input_get_label(self), lh_addr_of(origin));
    lh_gl_scalar_t at = lh_gl_point_get_x(lh_addr_of(origin));
    lh_gl_point_t press;
    lh_gl_entity_event_get_point(event, lh_addr_of(press));
    lh_gl_scalar_t x = lh_gl_point_get_x(lh_addr_of(press));
    /* The walk is in **advances**, not in repeated widths of the whole prefix: a
       field of 64 characters measured once per character is 4096 glyph lookups for
       one press, and the field is exactly where somebody holds the pointer down. */
    const lh_char_t *end = lh_addr_of(lh_gl_input_get_buffer(self)[lh_gl_input_get_used(self)]);
    const lh_char_t *cursor = lh_gl_input_get_buffer(self);
    lh_u16_t caret = lh_gl_input_get_used(self);
    for (; cursor < end;)
    {
        const lh_char_t *here = cursor;
        const lh_u32_t code = lh_str_utf8_get_next_code(lh_addr_of(cursor));
        const lh_gl_scalar_t next =
            at + lh_cast_static(lh_gl_scalar_t, lh_gl_font_get_advance(font, code));

        /* The boundary **before** the glyph that does not fit: a press past the end
           of the text belongs at the end, and a press in the middle belongs at the
           nearer side of the glyph under it, which is what every text field does.
           Compared against **where the pointer is** -- the first version asked the
           text where it starts, which is a constant, so every press landed at the same
           byte and a click in a field did nothing at all. */
        if (next > x)
        {
            caret = lh_cast_static(lh_u16_t, here - lh_gl_input_get_buffer(self));
            break;
        }
        at = next;
        caret = lh_cast_static(lh_u16_t, cursor - lh_gl_input_get_buffer(self));
    }
    return lh_gl_input_set_caret_v(input, caret);
}

lh_bool_t
lh_gl_input_on_key(const lh_gl_input_t *self, const lh_gl_entity_event_t *event)
{
    const lh_gl_key_input_t *key;
    lh_gl_input_t *input = lh_ptr_rcast(lh_gl_input_t, self);

    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_key, lh_bool_false);
    lh_return_if(lh_gl_input_is_focused(self) == lh_bool_false, lh_bool_false);
    key = lh_gl_entity_event_get_key(event);
    lh_return_if(lh_null_eq(key), lh_bool_false);

    /* A typed character first: it arrives with a code and no key, and a field that
       looked for a key first would answer "not mine" to every letter. */
    lh_u32_t code = lh_gl_key_input_get_code(key);
    if (code != 0U)
    {
        return lh_gl_input_insert(input, code);
    }

    if (lh_gl_key_input_is_down(key, lh_key_backspace))
    {
        return lh_gl_input_backspace(input);
    }
    if (lh_gl_key_input_is_down(key, lh_key_delete))
    {
        return lh_gl_input_delete_forward(input);
    }
    if (lh_gl_key_input_is_down(key, lh_key_left))
    {
        const lh_u16_t at = lh_gl_input_boundary_before(self, lh_gl_input_get_caret(self));

        return lh_gl_input_get_caret(self) == 0U ? lh_bool_false
                                                 : lh_gl_input_set_caret_v(input, at);
    }
    if (lh_gl_key_input_is_down(key, lh_key_right))
    {
        const lh_u32_t size =
            lh_gl_input_get_caret(self) < lh_gl_input_get_used(self)
                ? lh_str_utf8_get_lead_size(lh_cast_static(
                      lh_byte_t, lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)]))
                : 0U;

        return size == 0U
                   ? lh_bool_false
                   : lh_gl_input_set_caret_v(
                         input, lh_cast_static(lh_u16_t, lh_gl_input_get_caret(self) + size));
    }
    if (lh_gl_key_input_is_down(key, lh_key_home))
    {
        lh_gl_input_home(input);
        return lh_bool_true;
    }
    if (lh_gl_key_input_is_down(key, lh_key_end))
    {
        lh_gl_input_end(input);
        return lh_bool_true;
    }
    /* Enter, Tab and Escape are the application's: what a field does with them is a
       question about the form around it, not about the text. */
    return lh_bool_false;
}

lh_void
lh_gl_input_event(const struct lh_gl_entity *self, const lh_gl_entity_event_t *event)
{
    /* The label is the first field: self is the input. */
    const lh_gl_input_t *input = lh_ptr_rcast(const lh_gl_input_t, self);

    /* The label first: it draws the text, and the caret goes **over** it. */
    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_input_class), self, event);

    switch (lh_gl_entity_event_get_code(event))
    {
    case lh_gl_entity_event_focus:
    case lh_gl_entity_event_defocus:
        lh_gl_input_on_focus(input, event);
        break;
    case lh_gl_entity_event_focusable:
        lh_gl_input_on_focusable(event);
        break;
    case lh_gl_entity_event_key:
        LH_ATTRIBUTE_UNUSED(lh_gl_input_on_key(input, event));
        break;
    case lh_gl_entity_event_press:
        LH_ATTRIBUTE_UNUSED(lh_gl_input_on_press(input, event));
        break;
    case lh_gl_entity_event_draw:
        lh_gl_input_on_draw_caret(input, event);
        break;
    default:
        break;
    }
}
