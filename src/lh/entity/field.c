#include <lh/entity/field.h>
#include <lh/assert.h>
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

lh_void
lh_entity_field_insert(lh_entity_field_t *self, lh_uint_t code)
{
    lh_int_t i;
    if (lh_ptr_is_null(self->text) || self->capacity < 2)
    {
        return;
    }
    if (code == LH_ENTITY_KEY_LEFT)
    {
        if (self->cursor > 0)
        {
            self->cursor -= 1;
        }
        return;
    }
    if (code == LH_ENTITY_KEY_RIGHT)
    {
        if (self->cursor < self->length)
        {
            self->cursor += 1;
        }
        return;
    }
    if (code == LH_ENTITY_KEY_BACKSPACE)
    {
        if (self->cursor <= 0)
        {
            return;
        }
        for (i = self->cursor - 1; i < self->length; ++i)
        {
            self->text[i] = self->text[i + 1];
        }
        self->cursor -= 1;
        self->length -= 1;
        return;
    }
    if (code == LH_ENTITY_KEY_DELETE)
    {
        if (self->cursor >= self->length)
        {
            return;
        }
        for (i = self->cursor; i < self->length; ++i)
        {
            self->text[i] = self->text[i + 1];
        }
        self->length -= 1;
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
    if (code < 32U || code > 126U)
    {
        return;
    }
    if (self->length + 1 >= self->capacity)
    {
        return;
    }
    for (i = self->length; i > self->cursor; --i)
    {
        self->text[i] = self->text[i - 1];
    }
    self->text[self->cursor] = (lh_char_t)code;
    self->cursor += 1;
    self->length += 1;
    self->text[self->length] = 0;
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
