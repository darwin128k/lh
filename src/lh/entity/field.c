#include <lh/entity/field.h>
#include <lh/assert.h>
#include <lh/byte.h>
#include <lh/cast/static.h>
#include <lh/entity/key.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/font.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_field_construct(lh_entity_t *self)
{
    lh_entity_field_t *const field = lh_ptr_rcast(lh_entity_field_t, self);
    field->font = lh_ui_font_get_default();
    field->lines = 1;
}

lh_bool_t
lh_entity_field_continues(lh_char_t ch)
{
    return (lh_cast_static(lh_byte_t, ch) & 0xC0U) == 0x80U ? lh_bool_true : lh_bool_false;
}

lh_int_t
lh_entity_field_before(const lh_char_t *text, lh_int_t cursor)
{
    if (cursor <= 0)
    {
        return 0;
    }
    cursor -= 1;
    while (cursor > 0 && lh_entity_field_continues(text[cursor]))
    {
        cursor -= 1;
    }
    return cursor;
}

lh_int_t
lh_entity_field_after(const lh_char_t *text, lh_int_t cursor, lh_int_t length)
{
    if (cursor >= length)
    {
        return length;
    }
    cursor += 1;
    while (cursor < length && lh_entity_field_continues(text[cursor]))
    {
        cursor += 1;
    }
    return cursor;
}

lh_int_t
lh_entity_field_utf8(lh_uint_t code, lh_char_t *out)
{
    if (code < 0x80U)
    {
        out[0] = lh_cast_static(lh_char_t, code);
        return 1;
    }
    if (code < 0x800U)
    {
        out[0] = lh_cast_static(lh_char_t, 0xC0U | (code >> 6));
        out[1] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
        return 2;
    }
    if (code < 0x10000U)
    {
        out[0] = lh_cast_static(lh_char_t, 0xE0U | (code >> 12));
        out[1] = lh_cast_static(lh_char_t, 0x80U | ((code >> 6) & 0x3FU));
        out[2] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
        return 3;
    }
    if (code > 0x10FFFFU)
    {
        return 0;
    }
    out[0] = lh_cast_static(lh_char_t, 0xF0U | (code >> 18));
    out[1] = lh_cast_static(lh_char_t, 0x80U | ((code >> 12) & 0x3FU));
    out[2] = lh_cast_static(lh_char_t, 0x80U | ((code >> 6) & 0x3FU));
    out[3] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
    return 4;
}

lh_void
lh_entity_field_insert(lh_entity_field_t *self, lh_uint_t code)
{
    lh_char_t bytes[4];
    lh_int_t n;
    lh_int_t i;
    lh_int_t from;
    if (lh_ptr_is_null(self->text) || self->capacity < 2)
    {
        return;
    }
    if (code == LH_ENTITY_KEY_LEFT)
    {
        self->cursor = lh_entity_field_before(self->text, self->cursor);
        return;
    }
    if (code == LH_ENTITY_KEY_RIGHT)
    {
        self->cursor = lh_entity_field_after(self->text, self->cursor, self->length);
        return;
    }
    if (code == LH_ENTITY_KEY_BACKSPACE)
    {
        from = lh_entity_field_before(self->text, self->cursor);
        if (from == self->cursor)
        {
            return;
        }
        for (i = self->cursor; i <= self->length; ++i)
        {
            self->text[from + (i - self->cursor)] = self->text[i];
        }
        self->length -= self->cursor - from;
        self->cursor = from;
        return;
    }
    if (code == LH_ENTITY_KEY_DELETE)
    {
        from = lh_entity_field_after(self->text, self->cursor, self->length);
        if (from == self->cursor)
        {
            return;
        }
        for (i = from; i <= self->length; ++i)
        {
            self->text[self->cursor + (i - from)] = self->text[i];
        }
        self->length -= from - self->cursor;
        return;
    }
    if (code == LH_ENTITY_KEY_ENTER)
    {
        if (self->lines <= 1)
        {
            return;
        }
        code = (lh_uint_t)'\n';
    }
    if (code < 32U && code != (lh_uint_t)'\n')
    {
        return;
    }
    n = lh_entity_field_utf8(code, bytes);
    if (n <= 0 || self->length + n >= self->capacity)
    {
        return;
    }
    for (i = self->length; i >= self->cursor; --i)
    {
        self->text[i + n] = self->text[i];
    }
    for (i = 0; i < n; ++i)
    {
        self->text[self->cursor + i] = bytes[i];
    }
    self->cursor += n;
    self->length += n;
}

lh_void
lh_entity_field_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_field_t *const field = lh_ptr_rcast(lh_entity_field_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    const lh_ui_style_t *style;
    lh_ui_canvas_t *canvas;
    lh_math_rect_t bounds;
    lh_entity_screen_t *screen;
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        screen = lh_entity_cast(lh_entity_get_root(self), lh_addr_of(lh_entity_screen_class));
        if (lh_ptr_is_set(screen))
        {
            lh_entity_screen_set_focus(screen, self);
        }
        return;
    }
    if (code == LH_ENTITY_EVENT_KEY)
    {
        const lh_uint_t *const key =
            lh_ptr_rcast(const lh_uint_t, lh_entity_event_get_param(event));
        if (lh_ptr_is_set(key))
        {
            lh_entity_field_insert(field, *key);
            lh_entity_invalidate(self);
        }
        return;
    }
    if (code != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    canvas = lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event));
    if (lh_ptr_is_null(style) || lh_ptr_is_null(field->font))
    {
        return;
    }
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_ui_canvas_fill_rect(canvas, bounds, lh_ui_style_get_bg_color(style));
    if (lh_ptr_is_set(field->text))
    {
        lh_ui_font_draw(field->font, canvas, lh_math_rect_get_x(lh_addr_of(bounds)) + 4,
                        lh_math_rect_get_y(lh_addr_of(bounds)) + 4, field->text,
                        lh_ui_style_get_text_color(style));
    }
    screen = lh_entity_cast(lh_entity_get_root(self), lh_addr_of(lh_entity_screen_class));
    if (lh_ptr_is_set(screen) && lh_entity_screen_get_focus(screen) == self &&
        lh_ptr_is_set(field->text))
    {
        lh_char_t mark[128];
        lh_int_t n = field->cursor;
        lh_int_t i;
        lh_math_vec2_t measured;
        if (n > 127)
        {
            n = 127;
        }
        for (i = 0; i < n; ++i)
        {
            mark[i] = field->text[i] == '\n' ? ' ' : field->text[i];
        }
        mark[n] = 0;
        measured = lh_ui_font_measure(field->font, mark);
        lh_ui_canvas_fill_rect(
            canvas,
            lh_math_rect_make(lh_math_rect_get_x(lh_addr_of(bounds)) + 4 +
                                  (lh_int_t)lh_math_vec2_get_x(lh_addr_of(measured)),
                              lh_math_rect_get_y(lh_addr_of(bounds)) + 4, 1,
                              lh_ui_font_get_glyph_height(field->font)),
            lh_ui_style_get_text_color(style));
    }
}

const lh_entity_class_t lh_entity_field_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_field_t),
                                lh_entity_field_construct, lh_null, lh_entity_field_on_event);

lh_void
lh_entity_field_set_buffer(lh_entity_field_t *self, lh_char_t *text, lh_int_t capacity)
{
    lh_assert_runtime_ref(self);
    self->text = text;
    self->capacity = capacity;
    self->length = 0;
    self->cursor = 0;
    if (lh_ptr_is_set(text) && capacity > 0)
    {
        text[0] = 0;
    }
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_field_set_text(lh_entity_field_t *self, const lh_char_t *text)
{
    lh_int_t i;
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(self->text) || self->capacity < 2)
    {
        return;
    }
    i = 0;
    if (lh_ptr_is_set(text))
    {
        while (text[i] != 0 && i + 1 < self->capacity)
        {
            self->text[i] = text[i];
            i += 1;
        }
    }
    self->text[i] = 0;
    self->length = i;
    self->cursor = i;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

const lh_char_t *
lh_entity_field_get_text(const lh_entity_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_entity_field_set_lines(lh_entity_field_t *self, lh_int_t lines)
{
    lh_assert_runtime_ref(self);
    self->lines = lines < 1 ? 1 : lines;
}

lh_void
lh_entity_field_set_font(lh_entity_field_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
