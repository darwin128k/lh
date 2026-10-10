/**
 * @file input.c
 * @brief Implementation of `lh/ui/input.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/container.h>
#include <lh/ui/entity.h>
#include <lh/ui/input.h>
#include <lh/ui/key.h>
#include <lh/ui/label.h>
#include <lh/ui/text.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* ── Boundaries ──────────────────────────────────────────────────────────────
 *
 * Every caret position in this file is a byte offset that is **on** a code point,
 * and these two are what keeps it there. A field that steps back over one byte of a
 * two-byte character puts the caret inside it; the width of half a character is a
 * half-pixel of nothing, and the next character typed lands between two bytes that
 * together are not a character.
 *
 * Both are file-local: they are arithmetic on one field's bytes and nothing else
 * will ever want them. Every `static` in `lh/ui` is a function like these two and
 * none of them is state.
 */

/** @brief The start of the code point before byte @p caret, or 0 at the start. */
static lh_u16_t
lh_ui_input_boundary_before(const lh_ui_input_t *self, lh_u16_t caret)
{
    lh_u16_t at = caret;

    while (at > 0U && (self->buffer[at - 1U] & 0xC0) == 0x80)
    {
        --at;
    }
    return at > 0U ? lh_cast_static(lh_u16_t, at - 1U) : lh_cast_static(lh_u16_t, 0U);
}

/**
 * @brief Is byte @p at the start of a code point of @p self?
 *
 * The byte that decides it is the one **at** @p, not the one before it: a position is
 * the start of a character when the byte it stands on is a lead byte rather than a
 * continuation. Asking about the byte before asks the same question about the
 * character *ending* there, which is a different character -- so every position in a
 * field of ASCII said yes (no byte in ASCII is a continuation), which is why a caret
 * could sit between the two halves of a Cyrillic letter and every test with plain
 * letters stayed green over it.
 */
static lh_bool_t
lh_ui_input_is_boundary(const lh_ui_input_t *self, lh_u16_t at)
{
    lh_return_if(at > self->used, lh_bool_false);
    /* The end of the text is the end of the last character, and the byte there is the
       terminator: neither a lead byte nor a continuation, so it needs no case of its
       own. */
    return at == 0U || (self->buffer[at] & 0xC0) != 0x80 ? lh_bool_true : lh_bool_false;
}

/* ── Events ───────────────────────────────────────────────────────────────── */

lh_void
lh_ui_input_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    /* The label is the first field: self is the input. */
    const lh_ui_input_t *input = lh_ptr_rcast(const lh_ui_input_t, self);

    /* The label first: it draws the text, and the caret goes **over** it. */
    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_input_class), self, event);

    switch (lh_ui_entity_event_get_code(event))
    {
        case lh_ui_entity_event_focus:
        case lh_ui_entity_event_defocus:
            lh_ui_input_on_focus(input, event);
            break;
        case lh_ui_entity_event_focusable:
            lh_ui_input_on_focusable(event);
            break;
        case lh_ui_entity_event_key:
            (void)lh_ui_input_on_key(input, event);
            break;
        case lh_ui_entity_event_press:
            (void)lh_ui_input_on_press(input, event);
            break;
        case lh_ui_entity_event_draw:
            lh_ui_input_on_draw_caret(input, event);
            break;
        default:
            break;
    }
}

lh_void
lh_ui_input_on_focus(const lh_ui_input_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_input_t *input = lh_ptr_rcast(lh_ui_input_t, self);
    const lh_ui_entity_event_code_t code = lh_ui_entity_event_get_code(event);
    lh_bool_t focused;

    lh_return_if(code != lh_ui_entity_event_focus && code != lh_ui_entity_event_defocus);
    focused = code == lh_ui_entity_event_focus ? lh_bool_true : lh_bool_false;
    /* No damage: the caret appearing is the application invalidating, the same as
       after a scroll or a state change anywhere else in the tree. */
    input->focused = focused;
}

lh_void
lh_ui_input_on_focusable(const lh_ui_entity_event_t *event)
{
    /* A field is focusable, and saying so is what makes it work at all: a press puts
       the focus on `lh_ui_entity_find_focusable` of whatever was hit, Tab walks the
       focusables in order, and a key goes to whoever holds the focus. A field that
       said no would be a field that cannot be clicked into, cannot be tabbed to and
       never receives a key -- with every setter in this header working perfectly on a
       widget no person and no key can ever reach. The label underneath says no, which
       is right for a label; this is the one thing a field is and a label is not. */
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_focusable);
    *lh_ui_entity_event_get_focusable(event) = lh_bool_true;
}

lh_bool_t
lh_ui_input_on_key(const lh_ui_input_t *self, const lh_ui_entity_event_t *event)
{
    const lh_ui_key_input_t *key;
    lh_ui_input_t *input = lh_ptr_rcast(lh_ui_input_t, self);
    lh_u32_t code;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_key, lh_bool_false);
    lh_return_if(self->focused == lh_bool_false, lh_bool_false);
    key = lh_ui_entity_event_get_key(event);
    lh_return_if(lh_null_eq(key), lh_bool_false);

    /* A typed character first: it arrives with a code and no key, and a field that
       looked for a key first would answer "not mine" to every letter. */
    code = lh_ui_key_input_get_code(key);
    if (code != 0U)
    {
        return lh_ui_input_insert(input, code);
    }

    if (lh_ui_key_input_is_down(key, lh_key_backspace))
    {
        return lh_ui_input_backspace(input);
    }
    if (lh_ui_key_input_is_down(key, lh_key_delete))
    {
        return lh_ui_input_delete_forward(input);
    }
    if (lh_ui_key_input_is_down(key, lh_key_left))
    {
        const lh_u16_t at = lh_ui_input_boundary_before(self, self->caret);

        return self->caret == 0U ? lh_bool_false : lh_ui_input_set_caret(input, at);
    }
    if (lh_ui_key_input_is_down(key, lh_key_right))
    {
        const lh_u32_t size = self->caret < self->used
                                  ? lh_ui_text_lead_size(lh_cast_static(lh_byte_t, self->buffer[self->caret]))
                                  : 0U;

        return size == 0U ? lh_bool_false
                          : lh_ui_input_set_caret(input,
                                                  lh_cast_static(lh_u16_t, self->caret + size));
    }
    if (lh_ui_key_input_is_down(key, lh_key_home))
    {
        lh_ui_input_home(input);
        return lh_bool_true;
    }
    if (lh_ui_key_input_is_down(key, lh_key_end))
    {
        lh_ui_input_end(input);
        return lh_bool_true;
    }
    /* Enter, Tab and Escape are the application's: what a field does with them is a
       question about the form around it, not about the text. */
    return lh_bool_false;
}

lh_bool_t
lh_ui_input_on_press(const lh_ui_input_t *self, const lh_ui_entity_event_t *event)
{
    const lh_ui_font_t *font;
    const lh_char_t *cursor;
    const lh_char_t *end;
    lh_ui_input_t *input = lh_ptr_rcast(lh_ui_input_t, self);
    lh_ui_point_t origin;
    lh_ui_point_t press;
    lh_ui_scalar_t at;
    lh_ui_scalar_t x;
    lh_u16_t caret;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_press, lh_bool_false);
    lh_return_if(lh_null_eq(self->buffer), lh_bool_false);
    font = lh_ui_label_get_font(lh_addr_of(self->label));
    lh_return_if(lh_null_eq(font), lh_bool_false);

    origin = lh_ui_label_get_text_origin(lh_addr_of(self->label));
    at = lh_ui_point_get_x(lh_addr_of(origin));
    press = lh_ui_entity_event_get_point(event);
    x = lh_ui_point_get_x(lh_addr_of(press));
    /* The walk is in **advances**, not in repeated widths of the whole prefix: a
       field of 64 characters measured once per character is 4096 glyph lookups for
       one press, and the field is exactly where somebody holds the pointer down. */
    end = &self->buffer[self->used];
    cursor = self->buffer;
    caret = self->used;
    for (; cursor < end;)
    {
        const lh_char_t *here = cursor;
        const lh_u32_t code = lh_ui_text_next_code(lh_addr_of(cursor));
        const lh_ui_scalar_t next =
            at + lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_advance(font, code));

        /* The boundary **before** the glyph that does not fit: a press past the end
           of the text belongs at the end, and a press in the middle belongs at the
           nearer side of the glyph under it, which is what every text field does.
           Compared against **where the pointer is** -- the first version asked the
           text where it starts, which is a constant, so every press landed at the same
           byte and a click in a field did nothing at all. */
        if (next > x)
        {
            caret = lh_cast_static(lh_u16_t, here - self->buffer);
            break;
        }
        at = next;
        caret = lh_cast_static(lh_u16_t, cursor - self->buffer);
    }
    return lh_ui_input_set_caret(input, caret);
}

lh_void
lh_ui_input_on_draw_caret(const lh_ui_input_t *self, const lh_ui_entity_event_t *event)
{
    const lh_ui_color_t *color;
    lh_ui_canvas_t *canvas;
    lh_ui_rect_t caret;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    /* A caret that is not focused is not drawn. That is the whole of "focused" as
       far as this component is concerned, and it is why the field needs no clock. */
    lh_return_if(self->focused == lh_bool_false);
    color = lh_ui_input_get_caret_color(self);
    lh_return_if(lh_null_eq(color));
    caret = lh_ui_input_get_caret_rect(self);
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(caret)));
    canvas = lh_ui_entity_event_get_canvas(event);
    lh_return_if(lh_null_eq(canvas));
    lh_ui_canvas_fill_rect(canvas, lh_addr_of(caret), color);
}

/* ── Lifetime and fields ───────────────────────────────────────────────────── */

lh_void
lh_ui_input_init(lh_ui_input_t *self, lh_ui_rect_t rect, lh_char_t *buffer, lh_u16_t capacity)
{
    lh_assert_runtime_ref(self);
    lh_ui_label_init(lh_addr_of(self->label), rect, buffer);
    self->buffer = buffer;
    self->capacity = capacity;
    self->used = 0U;
    self->caret = 0U;
    self->focused = lh_bool_false;
    self->caret_color = lh_null;
    /* The label's own text **is** the buffer, so the drawing the label inherited
       shows what was typed without one call being made to say so. */
    lh_ui_entity_set_class(lh_ui_input_as_entity(self), lh_addr_of(lh_ui_input_class));
    if (!lh_null_eq(buffer))
    {
        buffer[0] = '\0';
    }
}

lh_ui_entity_t *
lh_ui_input_as_entity(lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_label_as_entity(lh_addr_of(self->label));
}

lh_ui_label_t *
lh_ui_input_as_label(lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->label);
}

lh_ui_input_t *
lh_ui_entity_as_input(lh_ui_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(!lh_ui_entity_class_is(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_input_class)),
                 lh_null);
    return lh_ptr_rcast(lh_ui_input_t, entity);
}

/* ── Text ──────────────────────────────────────────────────────────────────── */

const lh_char_t *
lh_ui_input_get_text(const lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buffer;
}

lh_bool_t
lh_ui_input_set_text(lh_ui_input_t *self, const lh_char_t *text)
{
    lh_u16_t used = 0U;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->buffer), lh_bool_false);
    /* `lh_return_ifn` is "return when the condition is FALSE", which is the opposite of
       what a guard wants; a null text is a caller with no text, and it is refused
       rather than read as "empty". */
    lh_return_if(lh_null_eq(text), lh_bool_false);

    while (used < self->capacity && text[used] != '\0')
    {
        self->buffer[used] = text[used];
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
            const lh_u32_t size = lh_ui_text_lead_size(lh_cast_static(lh_byte_t, self->buffer[at]));

            if (size == 0U || lh_cast_static(lh_u32_t, at) + size > lh_cast_static(lh_u32_t, used))
            {
                used = at;
                break;
            }
            at = lh_cast_static(lh_u16_t, at + size);
        }
    }
    self->used = used;
    self->buffer[used] = '\0';
    self->caret = used;
    return lh_bool_true;
}

lh_void
lh_ui_input_clear(lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->buffer));
    self->used = 0U;
    self->caret = 0U;
    self->buffer[0] = '\0';
}

lh_u16_t
lh_ui_input_get_length(const lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->used;
}

/* ── Editing ───────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_input_insert(lh_ui_input_t *self, lh_u32_t code)
{
    const lh_u32_t size = lh_ui_text_encoded_size(code);
    lh_u16_t at;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->buffer), lh_bool_false);
    /* Nothing is written in either of the two refusals. A character that does not
       fit leaves the text exactly as it was: a field with one byte of room must not
       end in half of the character the person just typed. */
    lh_return_if(size == 0U, lh_bool_false);
    lh_return_if(self->used + size > self->capacity, lh_bool_false);

    for (at = self->used; at > self->caret; --at)
    {
        self->buffer[at] = self->buffer[at - 1U];
    }
    lh_ui_text_encode_code(&self->buffer[self->caret], lh_cast_static(lh_u32_t, self->capacity - self->caret),
                           code);
    self->used = lh_cast_static(lh_u16_t, self->used + size);
    self->caret = lh_cast_static(lh_u16_t, self->caret + size);
    self->buffer[self->used] = '\0';
    return lh_bool_true;
}

lh_bool_t
lh_ui_input_backspace(lh_ui_input_t *self)
{
    const lh_u16_t from = lh_ui_input_boundary_before(self, self->caret);
    lh_u16_t at;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->buffer), lh_bool_false);
    lh_return_if(self->caret == 0U, lh_bool_false);

    for (at = from; at < self->used - 1U; ++at)
    {
        self->buffer[at] = self->buffer[at + 1U];
    }
    self->used = lh_cast_static(lh_u16_t, self->used - (self->caret - from));
    self->caret = from;
    self->buffer[self->used] = '\0';
    return lh_bool_true;
}

lh_bool_t
lh_ui_input_delete_forward(lh_ui_input_t *self)
{
    const lh_u32_t size = lh_ui_text_lead_size(lh_cast_static(lh_byte_t, self->buffer[self->caret]));
    lh_u16_t at;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->buffer), lh_bool_false);
    lh_return_if(self->caret >= self->used, lh_bool_false);
    lh_return_if(size == 0U, lh_bool_false);
    /* The character has to be **whole**. A buffer whose last code point was cut short
       -- the one shape a caller can produce by writing bytes straight into it -- has a
       lead byte that claims four bytes with one there, and `used - size` below is then
       a very large number, and the loop that shifts the tail walks off the end of the
       buffer writing zeroes after it. A refusal leaves the broken text as it is, which
       is something an application can see and repair; walking off the end is not. */
    lh_return_if(size > lh_cast_static(lh_u32_t, self->used - self->caret), lh_bool_false);

    for (at = self->caret; at < self->used - size; ++at)
    {
        self->buffer[at] = self->buffer[at + size];
    }
    self->used = lh_cast_static(lh_u16_t, self->used - size);
    self->buffer[self->used] = '\0';
    return lh_bool_true;
}

/* ── Caret ─────────────────────────────────────────────────────────────────── */

lh_u16_t
lh_ui_input_get_caret(const lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->caret;
}

lh_bool_t
lh_ui_input_set_caret(lh_ui_input_t *self, lh_u16_t caret)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->buffer), lh_bool_false);
    /* A refusal leaves the caret where it was. A caller with a byte that is not
       there has a number wrong, and moving the caret to the nearest boundary would
       hand it a different wrong number that looks right. */
    lh_return_if(!lh_ui_input_is_boundary(self, caret), lh_bool_false);
    self->caret = caret;
    return lh_bool_true;
}

lh_void
lh_ui_input_home(lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    self->caret = 0U;
}

lh_void
lh_ui_input_end(lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    self->caret = self->used;
}

lh_ui_rect_t
lh_ui_input_get_caret_rect(const lh_ui_input_t *self)
{
    lh_ui_rect_t caret;
    const lh_ui_font_t *font;
    lh_ui_point_t origin;
    lh_ui_rect_t ink;
    lh_ui_scalar_t at;
    lh_ui_scalar_t y;
    lh_ui_scalar_t height;
    lh_char_t saved;

    lh_ui_rect_init_empty(lh_addr_of(caret));
    lh_return_if(lh_null_eq(self->buffer), caret);
    font = lh_ui_label_get_font(lh_addr_of(self->label));
    lh_return_if(lh_null_eq(font), caret);

    origin = lh_ui_label_get_text_origin(lh_addr_of(self->label));
    /* The width of the text **before** the caret, measured by cutting the buffer at
       the caret and putting the byte back. A prefix is a string only while it is cut
       off, and the input owns what is in its buffer -- which is the whole reason it
       has one instead of the pointer a label keeps. */
    saved = self->buffer[self->caret];
    self->buffer[self->caret] = '\0';
    at = lh_ui_point_get_x(lh_addr_of(origin)) + lh_ui_text_get_width(font, self->buffer);
    self->buffer[self->caret] = saved;

    /* An empty field has no ink to measure, and **a caret whose height comes from
       the text is no caret at all** in a field with no text -- which is the state a
       field is in for exactly as long as it takes to notice. The line box stands in
       for it, which is the one thing that is always there. */
    ink = lh_ui_text_get_line_ink_rect(font, self->buffer, origin);
    if (self->used == 0U)
    {
        y = lh_ui_point_get_y(lh_addr_of(origin));
        height = lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_line_height(font));
    }
    else
    {
        y = lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(ink)));
        height = lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(ink)));
    }
    /* One column, and the caret is not rounded away: a caret on a half pixel is a
       grey line that is half as visible as the one it was. */
    lh_ui_rect_init(lh_addr_of(caret), at, y, lh_ui_scalar(1), height);
    return caret;
}

const lh_ui_color_t *
lh_ui_input_get_caret_color(const lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->caret_color;
}

lh_void
lh_ui_input_set_caret_color(lh_ui_input_t *self, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    self->caret_color = color;
}

lh_bool_t
lh_ui_input_is_focused(const lh_ui_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focused;
}

/* ── Class ─────────────────────────────────────────────────────────────────── */

const lh_ui_entity_class_t lh_ui_input_class = {
    lh_ui_input_event, lh_addr_of(lh_ui_label_class)};