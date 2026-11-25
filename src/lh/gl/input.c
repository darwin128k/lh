/**
 * @file input.c
 * @brief Implementation of `lh/gl/input.h`: the field itself: its fields (level 1), init and its
 * class.
 */

#include <lh/gl/input.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>
#include <lh/core/util/str/ptr.h>

const lh_gl_entity_class_t lh_gl_input_class = {lh_gl_input_event, lh_addr_of(lh_gl_label_class)};

lh_gl_label_t *
lh_gl_input_as_label(lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->label);
}

lh_gl_input_t *
lh_gl_entity_as_input(lh_gl_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(
        !lh_gl_entity_class_is(lh_gl_entity_get_class(entity), lh_addr_of(lh_gl_input_class)),
        lh_null);
    return lh_ptr_rcast(lh_gl_input_t, entity);
}

const lh_char_t *
lh_gl_input_get_text(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buffer;
}

lh_u16_t
lh_gl_input_get_caret(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->caret;
}

const lh_gl_color_t *
lh_gl_input_get_caret_color(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->caret_color;
}

lh_void
lh_gl_input_set_caret_color(lh_gl_input_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    self->caret_color = color;
}

lh_gl_input_caret_shape_t
lh_gl_input_get_caret_shape(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->caret_shape;
}

lh_void
lh_gl_input_set_caret_shape(lh_gl_input_t *self, lh_gl_input_caret_shape_t shape)
{
    lh_assert_runtime_ref(self);
    self->caret_shape = shape;
}

lh_bool_t
lh_gl_input_is_focused(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focused;
}

const lh_gl_label_t *
lh_gl_input_get_label(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->label);
}

lh_gl_label_t *
lh_gl_input_get_label_mut(lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->label);
}

lh_void
lh_gl_input_set_label(lh_gl_input_t *self, lh_gl_label_t label)
{
    lh_assert_runtime_ref(self);
    self->label = label;
}

lh_char_t *
lh_gl_input_get_buffer(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buffer;
}

lh_void
lh_gl_input_set_buffer(lh_gl_input_t *self, lh_char_t *buffer)
{
    lh_assert_runtime_ref(self);
    self->buffer = buffer;
}

lh_u16_t
lh_gl_input_get_capacity(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_gl_input_set_capacity(lh_gl_input_t *self, lh_u16_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

lh_u16_t
lh_gl_input_get_used(const lh_gl_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->used;
}

lh_void
lh_gl_input_set_used(lh_gl_input_t *self, lh_u16_t used)
{
    lh_assert_runtime_ref(self);
    self->used = used;
}

lh_void
lh_gl_input_set_focused(lh_gl_input_t *self, lh_bool_t focused)
{
    lh_assert_runtime_ref(self);
    self->focused = focused;
}

lh_void
lh_gl_input_set_caret(lh_gl_input_t *self, lh_u16_t caret)
{
    lh_assert_runtime_ref(self);
    self->caret = caret;
}

lh_gl_entity_t *
lh_gl_input_as_entity(lh_gl_input_t *self)
{
    return lh_gl_label_as_entity(lh_gl_input_get_label_mut(self));
}

lh_void
lh_gl_input_init(lh_gl_input_t *self, lh_gl_rect_t rect, lh_char_t *buffer, lh_u16_t capacity)
{
    /* Emptied before the label sees it: lh_gl_label_init hashes its text, and the
       caller's buffer is usually fresh, unterminated storage. */
    if (!lh_null_eq(buffer))
    {
        lh_str_ptr_clear(buffer);
    }
    lh_gl_label_init(lh_gl_input_as_label(self), rect, buffer);
    lh_gl_input_set_buffer(self, buffer);
    lh_gl_input_set_capacity(self, capacity);
    lh_gl_input_set_used(self, 0U);
    lh_gl_input_set_caret(self, 0U);
    lh_gl_input_set_focused(self, lh_bool_false);
    lh_gl_input_set_caret_color(self, lh_null);
    lh_gl_input_set_caret_shape(self, lh_gl_input_caret_bar);
    /* The label's own text **is** the buffer, so the drawing the label inherited
       shows what was typed without one call being made to say so. */
    lh_gl_entity_set_class_v(lh_gl_input_as_entity(self), lh_addr_of(lh_gl_input_class));
}
