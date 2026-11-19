/**
 * @file button.c
 * @brief Implementation of `lh/gx/button.h`.
 */

#include <lh/gx/button.h>

#include <lh/core/cast/const.h>
#include <lh/core/util/return.h>
#include <lh/gx/box.h>
#include <lh/gx/class/initializer.h>
#include <lh/gx/text.h>

const lh_gx_class_t lh_gx_button_class =
    lh_gx_class_initializer(lh_gx_box_paint, lh_gx_box_covers, lh_null, lh_gx_button_input, lh_null,
                            lh_null, lh_gx_box_get_ink);

const lh_gx_entity_t *
lh_gx_button_get_box(const lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_gx_entity_t *
lh_gx_button_get_box_mut(lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

const lh_gx_entity_t *
lh_gx_button_get_caption(const lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->caption);
}

lh_gx_entity_t *
lh_gx_button_get_caption_mut(lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->caption);
}

const lh_gx_style_t *
lh_gx_button_get_style(const lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gx_button_set_style(lh_gx_button_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

const lh_gx_style_t *
lh_gx_button_get_pressed_style(const lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed_style;
}

lh_void
lh_gx_button_set_pressed_style(lh_gx_button_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->pressed_style = style;
}

lh_gx_button_click_fn *
lh_gx_button_get_on_click(const lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_click;
}

lh_void
lh_gx_button_set_on_click(lh_gx_button_t *self, lh_gx_button_click_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->on_click = fn;
}

lh_ptr
lh_gx_button_get_context(const lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_gx_button_set_context(lh_gx_button_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

const lh_gx_tap_t *
lh_gx_button_get_tap(const lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tap);
}

lh_gx_tap_t *
lh_gx_button_get_tap_mut(lh_gx_button_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tap);
}

lh_gx_button_t *
lh_gx_button_from_entity(lh_gx_entity_t *entity)
{
    return lh_ptr_rcast(lh_gx_button_t, lh_gx_entity_get_content(entity));
}

lh_bool_t
lh_gx_button_is_pressed(const lh_gx_button_t *self)
{
    return lh_gx_tap_is_pressed(lh_gx_button_get_tap(self));
}

const lh_char_t *
lh_gx_button_get_text(const lh_gx_button_t *self)
{
    return lh_ptr_rcast(const lh_char_t, lh_gx_entity_get_content(lh_gx_button_get_caption(self)));
}

lh_void
lh_gx_button_click(lh_gx_button_t *self)
{
    lh_gx_button_click_fn *on_click = lh_gx_button_get_on_click(self);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, on_click)));
    on_click(self, lh_gx_button_get_context(self));
}

lh_void
lh_gx_button_set_look(lh_gx_button_t *self, const lh_gx_style_t *style)
{
    lh_gx_entity_set_style(lh_gx_button_get_box_mut(self), style);
    lh_gx_entity_set_style(lh_gx_button_get_caption_mut(self), style);
    lh_gx_entity_invalidate(lh_gx_button_get_box(self));
}

lh_void
lh_gx_button_center_caption(lh_gx_button_t *self)
{
    lh_gx_entity_t *caption = lh_gx_button_get_caption_mut(self);
    const lh_gx_style_t *style = lh_gx_entity_get_style(caption);
    const lh_gx_font_t *font = lh_null_ne(style) ? lh_gx_style_get_font(style) : lh_null;
    const lh_char_t *text = lh_gx_button_get_text(self);
    lh_gx_rect_t place;

    lh_gx_rect_init_empty(lh_addr_of(place));
    if (lh_null_ne(font) && lh_null_ne(text))
    {
        const lh_gx_size_t *room =
            lh_gx_rect_get_size(lh_gx_entity_get_place(lh_gx_button_get_box(self)));
        const lh_gx_scalar_t width = lh_gx_scalar_of(lh_gx_text_get_width(font, text));
        const lh_gx_scalar_t height = lh_gx_scalar_of(lh_gx_font_get_line_height(font));

        lh_gx_rect_init(lh_addr_of(place), lh_gx_scalar_centre(lh_gx_size_get_width(room), width),
                        lh_gx_scalar_centre(lh_gx_size_get_height(room), height), width, height);
    }
    lh_gx_entity_set_place(caption, place);
}

lh_void
lh_gx_button_press(lh_gx_button_t *self)
{
    const lh_gx_style_t *pressed_style = lh_gx_button_get_pressed_style(self);

    lh_return_if(lh_null_eq(pressed_style));
    lh_gx_button_set_look(self, pressed_style);
}

lh_void
lh_gx_button_release(lh_gx_button_t *self, lh_bool_t click)
{
    if (lh_null_ne(lh_gx_button_get_pressed_style(self)))
    {
        lh_gx_button_set_look(self, lh_gx_button_get_style(self));
    }
    lh_return_if(!click);
    lh_gx_button_click(self);
}

lh_void
lh_gx_button_set_text(lh_gx_button_t *self, const lh_char_t *text)
{
    lh_gx_entity_set_content(lh_gx_button_get_caption_mut(self), lh_cast_const(lh_char_t *, text));
    lh_gx_button_center_caption(self);
    lh_gx_entity_invalidate(lh_gx_button_get_box(self));
}

lh_void
lh_gx_button_input(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    lh_gx_button_t *self = lh_gx_button_from_entity(entity);

    switch (lh_gx_tap_take(lh_gx_button_get_tap_mut(self), entity, event))
    {
    case lh_gx_tap_step_down:
        lh_gx_button_press(self);
        break;
    case lh_gx_tap_step_up:
        lh_gx_button_release(self, lh_bool_false);
        break;
    case lh_gx_tap_step_tap:
        lh_gx_button_release(self, lh_bool_true);
        break;
    case lh_gx_tap_step_none:
    default:
        break;
    }
}

lh_void
lh_gx_button_init(lh_gx_button_t *self, const lh_gx_style_t *style, lh_gx_scalar_t x,
                  lh_gx_scalar_t y, lh_gx_scalar_t width, lh_gx_scalar_t height,
                  const lh_char_t *text, lh_gx_button_click_fn *on_click, lh_ptr context)
{
    lh_gx_entity_t *box = lh_gx_button_get_box_mut(self);
    lh_gx_entity_t *caption = lh_gx_button_get_caption_mut(self);

    lh_gx_entity_init(box, lh_addr_of(lh_gx_button_class), style, x, y, width, height);
    lh_gx_entity_set_content(box, self);
    lh_gx_entity_init(caption, lh_addr_of(lh_gx_text_class), style, 0, 0, 0, 0);
    lh_gx_entity_set_content(caption, lh_cast_const(lh_char_t *, text));
    lh_gx_entity_attach(box, caption);
    lh_gx_button_set_style(self, style);
    lh_gx_button_set_pressed_style(self, lh_null);
    lh_gx_button_set_on_click(self, on_click);
    lh_gx_button_set_context(self, context);
    lh_gx_tap_init(lh_gx_button_get_tap_mut(self));
    lh_gx_button_center_caption(self);
}
