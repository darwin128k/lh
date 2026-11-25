/**
 * @file edit.c
 * @brief Implementation of `lh/gl/input.h`: editing the text: replacing it, clearing it, inserting
 * and deleting.
 */

#include <lh/gl/input.h>

#include <lh/core/assert.h>
#include <lh/core/char/map.h>
#include <lh/core/str/utf8.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_input_set_text(lh_gl_input_t *self, const lh_char_t *text)
{
    lh_u16_t used = 0U;

    lh_return_if(lh_null_eq(lh_gl_input_get_buffer(self)), lh_bool_false);
    /* `lh_return_ifn` is "return when the condition is FALSE", which is the opposite of
       what a guard wants; a null text is a caller with no text, and it is refused
       rather than read as "empty". */
    lh_return_if(lh_null_eq(text), lh_bool_false);

    while (used < lh_gl_input_get_capacity(self) && text[used] != lh_char_map_nul)
    {
        lh_gl_input_get_buffer(self)[used] = text[used];
        ++used;
    }
    /* A cut lands on a code point boundary, and the only way to be sure of that is to
       **read the buffer forward** and stop where a code point does not fit. Walking
       backwards over trailing continuation bytes is the obvious way to write this and
       it is wrong: in "AB" followed by a two-byte character, cut at three, the last
       byte is a *lead* byte and the loop stops there -- leaving `AB` and half a
       character, which is not a string at all. Half a character is not a character,
       and the next read over it walks off the end of the text. */
    {
        lh_u16_t at = 0U;

        while (at < used)
        {
            const lh_u32_t size = lh_str_utf8_get_lead_size(
                lh_cast_static(lh_byte_t, lh_gl_input_get_buffer(self)[at]));

            if (size == 0U || lh_cast_static(lh_u32_t, at) + size > lh_cast_static(lh_u32_t, used))
            {
                used = at;
                break;
            }
            at = lh_cast_static(lh_u16_t, at + size);
        }
    }
    lh_gl_input_set_used(self, used);
    lh_gl_input_get_buffer(self)[used] = lh_char_map_nul;
    lh_gl_input_set_caret(self, used);
    lh_gl_label_drop_metrics(lh_gl_input_as_label(self));
    return lh_bool_true;
}

lh_void
lh_gl_input_clear(lh_gl_input_t *self)
{
    lh_return_if(lh_null_eq(lh_gl_input_get_buffer(self)));
    lh_gl_input_set_used(self, 0U);
    lh_gl_input_set_caret(self, 0U);
    lh_gl_input_get_buffer(self)[0] = lh_char_map_nul;
    lh_gl_label_drop_metrics(lh_gl_input_as_label(self));
}

lh_u16_t
lh_gl_input_get_length(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->used;
}

lh_bool_t
lh_gl_input_insert(lh_gl_input_t *self, lh_u32_t code)
{
    const lh_u32_t size = lh_str_utf8_get_encoded_size(code);

    lh_return_if(lh_null_eq(lh_gl_input_get_buffer(self)), lh_bool_false);
    /* Nothing is written in either of the two refusals. A character that does not
       fit leaves the text exactly as it was: a field with one byte of room must not
       end in half of the character the person just typed. */
    lh_return_if(size == 0U, lh_bool_false);
    lh_return_if(lh_gl_input_get_used(self) + size > lh_gl_input_get_capacity(self), lh_bool_false);

    for (lh_u16_t at = lh_gl_input_get_used(self); at > lh_gl_input_get_caret(self); --at)
    {
        lh_gl_input_get_buffer(self)[at] = lh_gl_input_get_buffer(self)[at - 1U];
    }
    lh_str_utf8_encode(
        lh_addr_of(lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)]),
        lh_cast_static(lh_u32_t, lh_gl_input_get_capacity(self) - lh_gl_input_get_caret(self)),
        code);
    lh_gl_input_set_used(self, lh_cast_static(lh_u16_t, lh_gl_input_get_used(self) + size));
    lh_gl_input_set_caret(self, lh_cast_static(lh_u16_t, lh_gl_input_get_caret(self) + size));
    lh_gl_input_get_buffer(self)[lh_gl_input_get_used(self)] = lh_char_map_nul;
    lh_gl_label_drop_metrics(lh_gl_input_as_label(self));
    return lh_bool_true;
}

lh_bool_t
lh_gl_input_backspace(lh_gl_input_t *self)
{
    const lh_u16_t from = lh_gl_input_boundary_before(self, lh_gl_input_get_caret(self));

    lh_return_if(lh_null_eq(lh_gl_input_get_buffer(self)), lh_bool_false);
    lh_return_if(lh_gl_input_get_caret(self) == 0U, lh_bool_false);

    for (lh_u16_t at = from; at < lh_gl_input_get_used(self) - 1U; ++at)
    {
        lh_gl_input_get_buffer(self)[at] = lh_gl_input_get_buffer(self)[at + 1U];
    }
    lh_gl_input_set_used(self, lh_cast_static(lh_u16_t, lh_gl_input_get_used(self) -
                                                            (lh_gl_input_get_caret(self) - from)));
    lh_gl_input_set_caret(self, from);
    lh_gl_input_get_buffer(self)[lh_gl_input_get_used(self)] = lh_char_map_nul;
    lh_gl_label_drop_metrics(lh_gl_input_as_label(self));
    return lh_bool_true;
}

lh_bool_t
lh_gl_input_delete_forward(lh_gl_input_t *self)
{
    const lh_u32_t size = lh_str_utf8_get_lead_size(
        lh_cast_static(lh_byte_t, lh_gl_input_get_buffer(self)[lh_gl_input_get_caret(self)]));

    lh_return_if(lh_null_eq(lh_gl_input_get_buffer(self)), lh_bool_false);
    lh_return_if(lh_gl_input_get_caret(self) >= lh_gl_input_get_used(self), lh_bool_false);
    lh_return_if(size == 0U, lh_bool_false);
    /* The character has to be **whole**. A buffer whose last code point was cut short
       -- the one shape a caller can produce by writing bytes straight into it -- has a
       lead byte that claims four bytes with one there, and `used - size` below is then
       a very large number, and the loop that shifts the tail walks off the end of the
       buffer writing zeroes after it. A refusal leaves the broken text as it is, which
       is something an application can see and repair; walking off the end is not. */
    lh_return_if(
        size > lh_cast_static(lh_u32_t, lh_gl_input_get_used(self) - lh_gl_input_get_caret(self)),
        lh_bool_false);

    for (lh_u16_t at = lh_gl_input_get_caret(self); at < lh_gl_input_get_used(self) - size; ++at)
    {
        lh_gl_input_get_buffer(self)[at] = lh_gl_input_get_buffer(self)[at + size];
    }
    lh_gl_input_set_used(self, lh_cast_static(lh_u16_t, lh_gl_input_get_used(self) - size));
    lh_gl_input_get_buffer(self)[lh_gl_input_get_used(self)] = lh_char_map_nul;
    lh_gl_label_drop_metrics(lh_gl_input_as_label(self));
    return lh_bool_true;
}
