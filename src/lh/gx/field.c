/**
 * @file field.c
 * @brief Implementation of `lh/gx/field.h`.
 */

#include <lh/gx/field.h>

#include <lh/core/memory/std.h>
#include <lh/core/str/utf8.h>
#include <lh/core/util/return.h>
#include <lh/core/util/str/ptr.h>
#include <lh/gx/box.h>
#include <lh/gx/class/initializer.h>
#include <lh/gx/text.h>

const lh_gx_class_t lh_gx_field_class =
    lh_gx_class_initializer(lh_gx_box_paint, lh_gx_box_covers, lh_null, lh_gx_field_input,
                            lh_gx_field_key, lh_null, lh_gx_box_get_ink);

const lh_gx_entity_t *
lh_gx_field_get_box(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_gx_entity_t *
lh_gx_field_get_box_mut(lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

const lh_gx_entity_t *
lh_gx_field_get_view(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->view);
}

lh_gx_entity_t *
lh_gx_field_get_view_mut(lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->view);
}

const lh_gx_entity_t *
lh_gx_field_get_text_entity(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_gx_entity_t *
lh_gx_field_get_text_entity_mut(lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

const lh_gx_entity_t *
lh_gx_field_get_caret(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->caret);
}

lh_gx_entity_t *
lh_gx_field_get_caret_mut(lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->caret);
}

const lh_gx_style_t *
lh_gx_field_get_style(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gx_field_set_style(lh_gx_field_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

const lh_gx_style_t *
lh_gx_field_get_focus_style(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focus_style;
}

lh_void
lh_gx_field_set_focus_style(lh_gx_field_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->focus_style = style;
}

const lh_gx_style_t *
lh_gx_field_get_caret_style(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->caret_style;
}

lh_void
lh_gx_field_set_caret_style(lh_gx_field_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->caret_style = style;
}

const lh_char_t *
lh_gx_field_get_text(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buffer;
}

lh_char_t *
lh_gx_field_get_buffer_mut(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buffer;
}

lh_void
lh_gx_field_set_buffer(lh_gx_field_t *self, lh_char_t *buffer)
{
    lh_assert_runtime_ref(self);
    self->buffer = buffer;
}

lh_usize_t
lh_gx_field_get_capacity(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_gx_field_set_capacity(lh_gx_field_t *self, lh_usize_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

lh_usize_t
lh_gx_field_get_length(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->length;
}

lh_void
lh_gx_field_set_length(lh_gx_field_t *self, lh_usize_t length)
{
    lh_assert_runtime_ref(self);
    self->length = length;
}

lh_usize_t
lh_gx_field_get_cursor(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cursor;
}

lh_void
lh_gx_field_set_cursor(lh_gx_field_t *self, lh_usize_t cursor)
{
    lh_assert_runtime_ref(self);
    self->cursor = cursor;
}

lh_s32_t
lh_gx_field_get_scroll(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scroll;
}

lh_void
lh_gx_field_set_scroll(lh_gx_field_t *self, lh_s32_t scroll)
{
    lh_assert_runtime_ref(self);
    self->scroll = scroll;
}

lh_gx_field_fn *
lh_gx_field_get_on_change(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_change;
}

lh_void
lh_gx_field_set_on_change(lh_gx_field_t *self, lh_gx_field_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->on_change = fn;
}

lh_gx_field_fn *
lh_gx_field_get_on_submit(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_submit;
}

lh_void
lh_gx_field_set_on_submit(lh_gx_field_t *self, lh_gx_field_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->on_submit = fn;
}

lh_ptr
lh_gx_field_get_context(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_gx_field_set_context(lh_gx_field_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_bool_t
lh_gx_field_is_focused(const lh_gx_field_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focused;
}

lh_void
lh_gx_field_set_focused(lh_gx_field_t *self, lh_bool_t focused)
{
    lh_assert_runtime_ref(self);
    self->focused = focused;
}

lh_gx_field_t *
lh_gx_field_from_entity(lh_gx_entity_t *entity)
{
    return lh_ptr_rcast(lh_gx_field_t, lh_gx_entity_get_content(entity));
}

const lh_gx_font_t *
lh_gx_field_get_font(const lh_gx_field_t *self)
{
    const lh_gx_style_t *style = lh_gx_field_get_style(self);

    return lh_null_ne(style) ? lh_gx_style_get_font(style) : lh_null;
}

lh_void
lh_gx_field_call(lh_gx_field_t *self, lh_gx_field_fn *fn)
{
    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, fn)));
    fn(self, lh_gx_field_get_context(self));
}

lh_void
lh_gx_field_show_focus(lh_gx_field_t *self, lh_bool_t focused)
{
    const lh_gx_style_t *focus_style = lh_gx_field_get_focus_style(self);

    lh_gx_field_set_focused(self, focused);
    lh_gx_entity_set_style(lh_gx_field_get_box_mut(self), focused && lh_null_ne(focus_style)
                                                              ? focus_style
                                                              : lh_gx_field_get_style(self));
    lh_gx_entity_set_shown(lh_gx_field_get_caret_mut(self), focused);
    lh_gx_entity_invalidate(lh_gx_field_get_box(self));
}

lh_s32_t
lh_gx_field_get_caret_x(const lh_gx_field_t *self)
{
    const lh_gx_font_t *font = lh_gx_field_get_font(self);

    lh_return_if(lh_null_eq(font), 0);
    return lh_gx_text_get_width_n(font, lh_gx_field_get_text(self), lh_gx_field_get_cursor(self));
}

lh_usize_t
lh_gx_field_get_cursor_at(const lh_gx_field_t *self, const lh_gx_point_t *at)
{
    const lh_gx_font_t *font = lh_gx_field_get_font(self);
    const lh_char_t *text = lh_gx_field_get_text(self);
    const lh_usize_t length = lh_gx_field_get_length(self);

    lh_return_if(lh_null_eq(font), length);
    lh_gx_rect_t view;

    lh_gx_entity_get_screen_rect(lh_gx_field_get_view(self), lh_addr_of(view));
    /* Twice the distance into the text, so that half an advance is whole. */
    const lh_s64_t along =
        2 * (lh_cast_static(lh_s64_t, lh_gx_point_get_x(at)) -
             lh_cast_static(lh_s64_t, lh_gx_point_get_x(lh_gx_rect_get_origin(lh_addr_of(view)))) +
             lh_gx_field_get_scroll(self));
    lh_s64_t pen = 0;
    lh_usize_t offset = 0U;

    while (offset < length)
    {
        const lh_char_t *cursor = text + offset;
        const lh_s32_t advance =
            lh_gx_text_get_code_width(font, lh_str_utf8_get_next_code(lh_addr_of(cursor)));

        lh_return_if(along < 2 * pen + advance, offset);
        pen += advance;
        offset = lh_cast_static(lh_usize_t, lh_ptr_udiff(cursor, text));
    }
    return length;
}

lh_void
lh_gx_field_keep_caret_shown(lh_gx_field_t *self)
{
    const lh_gx_font_t *font = lh_gx_field_get_font(self);
    const lh_s32_t room =
        lh_cast_static(lh_s32_t, lh_gx_size_get_width(lh_gx_rect_get_size(
                                     lh_gx_entity_get_place(lh_gx_field_get_view(self)))));
    const lh_s32_t whole =
        (lh_null_ne(font) ? lh_gx_text_get_width(font, lh_gx_field_get_text(self)) : 0) +
        LH_GX_FIELD_CARET_WIDTH;
    const lh_s32_t caret = lh_gx_field_get_caret_x(self);
    lh_s32_t scroll = lh_gx_field_get_scroll(self);

    if (caret - scroll > room - LH_GX_FIELD_CARET_WIDTH)
    {
        scroll = caret - room + LH_GX_FIELD_CARET_WIDTH;
    }
    if (caret < scroll)
    {
        scroll = caret;
    }
    /* No room left empty past the end of the text once it is shorter again. */
    lh_gx_field_set_scroll(self, lh_math_clamp(scroll, 0, lh_math_max(whole - room, 0)));
}

lh_void
lh_gx_field_lay_out(lh_gx_field_t *self)
{
    const lh_gx_size_t *size =
        lh_gx_rect_get_size(lh_gx_entity_get_place(lh_gx_field_get_box(self)));
    const lh_gx_scalar_t width = lh_gx_size_get_width(size);
    const lh_gx_scalar_t height = lh_gx_size_get_height(size);
    const lh_gx_font_t *font = lh_gx_field_get_font(self);
    const lh_gx_scalar_t line =
        lh_null_ne(font) ? lh_gx_scalar_of(lh_gx_font_get_line_height(font)) : height;
    const lh_gx_scalar_t top = lh_gx_scalar_centre(height, line);
    const lh_gx_scalar_t scroll = lh_gx_scalar_of(lh_gx_field_get_scroll(self));
    lh_gx_rect_t place;

    lh_gx_rect_init(lh_addr_of(place), LH_GX_FIELD_PADDING, 0,
                    lh_math_max(width - 2 * LH_GX_FIELD_PADDING, 0), height);
    lh_gx_entity_move(lh_gx_field_get_view_mut(self), place);
    lh_gx_rect_init(lh_addr_of(place), -scroll, top,
                    lh_null_ne(font)
                        ? lh_gx_scalar_of(lh_gx_text_get_width(font, lh_gx_field_get_text(self)))
                        : 0,
                    line);
    lh_gx_entity_move(lh_gx_field_get_text_entity_mut(self), place);
    lh_gx_rect_init(lh_addr_of(place), lh_gx_scalar_of(lh_gx_field_get_caret_x(self)) - scroll, top,
                    LH_GX_FIELD_CARET_WIDTH, line);
    lh_gx_entity_move(lh_gx_field_get_caret_mut(self), place);
}

lh_void
lh_gx_field_refresh(lh_gx_field_t *self)
{
    lh_gx_field_keep_caret_shown(self);
    lh_gx_field_lay_out(self);
    lh_gx_entity_invalidate(lh_gx_field_get_view(self));
}

lh_void
lh_gx_field_place_cursor(lh_gx_field_t *self, lh_usize_t cursor)
{
    lh_gx_field_set_cursor(self, lh_math_min(cursor, lh_gx_field_get_length(self)));
    lh_gx_field_refresh(self);
}

lh_bool_t
lh_gx_field_insert(lh_gx_field_t *self, lh_u32_t code)
{
    const lh_u32_t size = lh_str_utf8_get_encoded_size(code);
    const lh_usize_t length = lh_gx_field_get_length(self);
    const lh_usize_t cursor = lh_gx_field_get_cursor(self);
    lh_char_t *buffer = lh_gx_field_get_buffer_mut(self);

    lh_return_if(size == 0U || length + size + 1U > lh_gx_field_get_capacity(self), lh_bool_false);
    /* The tail, its `0` included, moves right to make room. */
    lh_memory_std_move(buffer + cursor + size, buffer + cursor, length - cursor + 1U);
    lh_str_utf8_encode(buffer + cursor, size, code);
    lh_gx_field_set_length(self, length + size);
    lh_gx_field_place_cursor(self, cursor + size);
    return lh_bool_true;
}

lh_void
lh_gx_field_erase(lh_gx_field_t *self, lh_usize_t from, lh_usize_t to)
{
    const lh_usize_t length = lh_gx_field_get_length(self);
    lh_char_t *buffer = lh_gx_field_get_buffer_mut(self);

    /* The tail, its `0` included, moves left over the gap. */
    lh_memory_std_move(buffer + from, buffer + to, length - to + 1U);
    lh_gx_field_set_length(self, length - (to - from));
    lh_gx_field_place_cursor(self, from);
}

lh_void
lh_gx_field_step(lh_gx_field_t *self, lh_bool_t forward)
{
    const lh_usize_t cursor = lh_gx_field_get_cursor(self);
    const lh_usize_t length = lh_gx_field_get_length(self);
    const lh_char_t *text = lh_gx_field_get_text(self);

    if (forward)
    {
        lh_return_if(cursor >= length);
        lh_gx_field_place_cursor(
            self, cursor + lh_str_utf8_get_lead_size(lh_cast_static(lh_byte_t, text[cursor])));
        return;
    }
    lh_return_if(cursor == 0U);
    lh_gx_field_place_cursor(self, cursor - lh_str_utf8_get_prev_size(text, cursor));
}

lh_void
lh_gx_field_set_text(lh_gx_field_t *self, const lh_char_t *text)
{
    lh_char_t *buffer = lh_gx_field_get_buffer_mut(self);
    lh_usize_t size = lh_math_min(lh_str_ptr_len(text), lh_gx_field_get_capacity(self) - 1U);

    /* Whole characters only: a cut inside one backs off to its start. */
    while (size > 0U && lh_str_utf8_is_continuation(lh_cast_static(lh_byte_t, text[size])))
    {
        --size;
    }
    lh_memory_std_copy(buffer, text, size);
    buffer[size] = 0;
    lh_gx_field_set_length(self, size);
    lh_gx_field_set_scroll(self, 0);
    lh_gx_field_place_cursor(self, size);
}

lh_void
lh_gx_field_input(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    lh_return_if(lh_gx_event_get_kind(event) != lh_gx_event_kind_press);
    lh_gx_field_t *self = lh_gx_field_from_entity(entity);

    lh_gx_field_place_cursor(self, lh_gx_field_get_cursor_at(self, lh_gx_event_get_at(event)));
}

lh_void
lh_gx_field_init(lh_gx_field_t *self, const lh_gx_style_t *style, const lh_gx_style_t *focus_style,
                 const lh_gx_style_t *caret_style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                 lh_gx_scalar_t width, lh_gx_scalar_t height, lh_char_t *buffer,
                 lh_usize_t capacity, lh_gx_field_fn *on_change, lh_gx_field_fn *on_submit,
                 lh_ptr context)
{
    lh_gx_entity_t *box = lh_gx_field_get_box_mut(self);
    lh_gx_entity_t *view = lh_gx_field_get_view_mut(self);
    lh_gx_entity_t *text = lh_gx_field_get_text_entity_mut(self);
    lh_gx_entity_t *caret = lh_gx_field_get_caret_mut(self);

    lh_gx_entity_init(box, lh_addr_of(lh_gx_field_class), style, x, y, width, height);
    lh_gx_entity_set_content(box, self);
    lh_gx_entity_init(view, lh_null, lh_null, 0, 0, 0, 0);
    lh_gx_entity_init(text, lh_addr_of(lh_gx_text_class), style, 0, 0, 0, 0);
    lh_gx_entity_set_content(text, buffer);
    lh_gx_entity_init(caret, lh_addr_of(lh_gx_box_class), caret_style, 0, 0, 0, 0);
    lh_gx_entity_set_visible(caret, lh_bool_false);
    lh_gx_entity_attach(box, view);
    lh_gx_entity_attach(view, text);
    lh_gx_entity_attach(view, caret);
    lh_gx_field_set_style(self, style);
    lh_gx_field_set_focus_style(self, focus_style);
    lh_gx_field_set_caret_style(self, caret_style);
    lh_gx_field_set_buffer(self, buffer);
    lh_gx_field_set_capacity(self, capacity);
    lh_gx_field_set_length(self, lh_str_ptr_len(buffer));
    lh_gx_field_set_scroll(self, 0);
    lh_gx_field_set_on_change(self, on_change);
    lh_gx_field_set_on_submit(self, on_submit);
    lh_gx_field_set_context(self, context);
    lh_gx_field_set_focused(self, lh_bool_false);
    lh_gx_field_lay_out(self);
    lh_gx_field_place_cursor(self, lh_gx_field_get_length(self));
}

lh_bool_t
lh_gx_field_erase_before(lh_gx_field_t *self)
{
    const lh_usize_t cursor = lh_gx_field_get_cursor(self);

    lh_return_if(cursor == 0U, lh_bool_false);
    lh_gx_field_erase(self, cursor - lh_str_utf8_get_prev_size(lh_gx_field_get_text(self), cursor),
                      cursor);
    return lh_bool_true;
}

lh_bool_t
lh_gx_field_erase_after(lh_gx_field_t *self)
{
    const lh_usize_t cursor = lh_gx_field_get_cursor(self);
    const lh_usize_t length = lh_gx_field_get_length(self);

    lh_return_if(cursor >= length, lh_bool_false);
    const lh_usize_t size =
        lh_str_utf8_get_lead_size(lh_cast_static(lh_byte_t, lh_gx_field_get_text(self)[cursor]));

    lh_gx_field_erase(self, cursor, lh_math_min(cursor + size, length));
    return lh_bool_true;
}

lh_void
lh_gx_field_press_key(lh_gx_field_t *self, lh_gx_key_t key)
{
    switch (key)
    {
    case lh_gx_key_left:
        lh_gx_field_step(self, lh_bool_false);
        break;
    case lh_gx_key_right:
        lh_gx_field_step(self, lh_bool_true);
        break;
    case lh_gx_key_home:
        lh_gx_field_place_cursor(self, 0U);
        break;
    case lh_gx_key_end:
        lh_gx_field_place_cursor(self, lh_gx_field_get_length(self));
        break;
    case lh_gx_key_backspace:
        if (lh_gx_field_erase_before(self))
        {
            lh_gx_field_call(self, lh_gx_field_get_on_change(self));
        }
        break;
    case lh_gx_key_delete:
        if (lh_gx_field_erase_after(self))
        {
            lh_gx_field_call(self, lh_gx_field_get_on_change(self));
        }
        break;
    case lh_gx_key_enter:
        lh_gx_field_call(self, lh_gx_field_get_on_submit(self));
        break;
    default:
        break;
    }
}

lh_void
lh_gx_field_key(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    lh_gx_field_t *self = lh_gx_field_from_entity(entity);

    switch (lh_gx_event_get_kind(event))
    {
    case lh_gx_event_kind_focus:
        lh_gx_field_show_focus(self, lh_bool_true);
        break;
    case lh_gx_event_kind_blur:
        lh_gx_field_show_focus(self, lh_bool_false);
        break;
    case lh_gx_event_kind_text:
        if (lh_gx_field_insert(self, lh_gx_event_get_code(event)))
        {
            lh_gx_field_call(self, lh_gx_field_get_on_change(self));
        }
        break;
    case lh_gx_event_kind_key:
        lh_gx_field_press_key(self, lh_gx_event_get_key(event));
        break;
    default:
        break;
    }
}
