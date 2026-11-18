/**
 * @file checkbox.c
 * @brief Implementation of `lh/gx/checkbox.h`.
 */

#include <lh/gx/checkbox.h>

#include <lh/core/cast/const.h>
#include <lh/core/util/return.h>
#include <lh/gx/box.h>
#include <lh/gx/class/initializer.h>
#include <lh/gx/text.h>

const lh_gx_class_t lh_gx_checkbox_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, lh_gx_checkbox_input, lh_null);

const lh_gx_entity_t *
lh_gx_checkbox_get_row(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->row);
}

lh_gx_entity_t *
lh_gx_checkbox_get_row_mut(lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->row);
}

const lh_gx_entity_t *
lh_gx_checkbox_get_square(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->square);
}

lh_gx_entity_t *
lh_gx_checkbox_get_square_mut(lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->square);
}

const lh_gx_entity_t *
lh_gx_checkbox_get_tick(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tick);
}

lh_gx_entity_t *
lh_gx_checkbox_get_tick_mut(lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tick);
}

const lh_gx_entity_t *
lh_gx_checkbox_get_caption(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->caption);
}

lh_gx_entity_t *
lh_gx_checkbox_get_caption_mut(lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->caption);
}

const lh_gx_style_t *
lh_gx_checkbox_get_style(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gx_checkbox_set_style(lh_gx_checkbox_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

const lh_gx_style_t *
lh_gx_checkbox_get_tick_style(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tick_style;
}

lh_void
lh_gx_checkbox_set_tick_style(lh_gx_checkbox_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->tick_style = style;
}

lh_gx_checkbox_change_fn *
lh_gx_checkbox_get_on_change(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_change;
}

lh_void
lh_gx_checkbox_set_on_change(lh_gx_checkbox_t *self, lh_gx_checkbox_change_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->on_change = fn;
}

lh_ptr
lh_gx_checkbox_get_context(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_gx_checkbox_set_context(lh_gx_checkbox_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

const lh_gx_tap_t *
lh_gx_checkbox_get_tap(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tap);
}

lh_gx_tap_t *
lh_gx_checkbox_get_tap_mut(lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tap);
}

lh_bool_t
lh_gx_checkbox_is_checked(const lh_gx_checkbox_t *self)
{
    lh_assert_runtime_ref(self);
    return self->checked;
}

lh_void
lh_gx_checkbox_set_checked(lh_gx_checkbox_t *self, lh_bool_t checked)
{
    lh_assert_runtime_ref(self);
    self->checked = checked;
}

lh_gx_checkbox_t *
lh_gx_checkbox_from_entity(lh_gx_entity_t *entity)
{
    return lh_ptr_rcast(lh_gx_checkbox_t, lh_gx_entity_get_content(entity));
}

const lh_char_t *
lh_gx_checkbox_get_text(const lh_gx_checkbox_t *self)
{
    return lh_ptr_rcast(const lh_char_t,
                        lh_gx_entity_get_content(lh_gx_checkbox_get_caption(self)));
}

lh_void
lh_gx_checkbox_change(lh_gx_checkbox_t *self)
{
    lh_gx_checkbox_change_fn *on_change = lh_gx_checkbox_get_on_change(self);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, on_change)));
    on_change(self, lh_gx_checkbox_is_checked(self), lh_gx_checkbox_get_context(self));
}

lh_void
lh_gx_checkbox_check(lh_gx_checkbox_t *self, lh_bool_t checked)
{
    lh_gx_checkbox_set_checked(self, checked);
    lh_gx_entity_set_shown(lh_gx_checkbox_get_tick_mut(self), checked);
}

lh_void
lh_gx_checkbox_lay_out(lh_gx_checkbox_t *self)
{
    const lh_gx_size_t *room =
        lh_gx_rect_get_size(lh_gx_entity_get_place(lh_gx_checkbox_get_row(self)));
    const lh_gx_scalar_t side = lh_gx_size_get_height(room);
    const lh_gx_scalar_t inset = side / 4;
    lh_gx_rect_t place;

    lh_gx_rect_init(lh_addr_of(place), 0, 0, side, side);
    lh_gx_entity_set_place(lh_gx_checkbox_get_square_mut(self), place);
    lh_gx_rect_init(lh_addr_of(place), inset, inset, side - inset - inset, side - inset - inset);
    lh_gx_entity_set_place(lh_gx_checkbox_get_tick_mut(self), place);

    const lh_gx_style_t *style = lh_gx_checkbox_get_style(self);
    const lh_gx_font_t *font = lh_null_ne(style) ? lh_gx_style_get_font(style) : lh_null;
    const lh_char_t *text = lh_gx_checkbox_get_text(self);

    lh_gx_rect_init_empty(lh_addr_of(place));
    if (lh_null_ne(font) && lh_null_ne(text))
    {
        const lh_gx_scalar_t height = lh_gx_scalar_of(lh_gx_font_get_line_height(font));

        lh_gx_rect_init(lh_addr_of(place), side + side / 2, lh_gx_scalar_centre(side, height),
                        lh_gx_scalar_of(lh_gx_text_get_width(font, text)), height);
    }
    lh_gx_entity_set_place(lh_gx_checkbox_get_caption_mut(self), place);
}

lh_void
lh_gx_checkbox_toggle(lh_gx_checkbox_t *self)
{
    lh_gx_checkbox_check(self, lh_cast_static(lh_bool_t, !lh_gx_checkbox_is_checked(self)));
    lh_gx_checkbox_change(self);
}

lh_void
lh_gx_checkbox_set_text(lh_gx_checkbox_t *self, const lh_char_t *text)
{
    lh_gx_entity_set_content(lh_gx_checkbox_get_caption_mut(self),
                             lh_cast_const(lh_char_t *, text));
    lh_gx_checkbox_lay_out(self);
    lh_gx_entity_invalidate(lh_gx_checkbox_get_row(self));
}

lh_void
lh_gx_checkbox_input(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    lh_gx_checkbox_t *self = lh_gx_checkbox_from_entity(entity);

    lh_return_if(lh_gx_tap_take(lh_gx_checkbox_get_tap_mut(self), entity, event) !=
                 lh_gx_tap_step_tap);
    lh_gx_checkbox_toggle(self);
}

lh_void
lh_gx_checkbox_init(lh_gx_checkbox_t *self, const lh_gx_style_t *style,
                    const lh_gx_style_t *tick_style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                    lh_gx_scalar_t width, lh_gx_scalar_t height, const lh_char_t *text,
                    lh_bool_t checked, lh_gx_checkbox_change_fn *on_change, lh_ptr context)
{
    lh_gx_entity_t *row = lh_gx_checkbox_get_row_mut(self);
    lh_gx_entity_t *square = lh_gx_checkbox_get_square_mut(self);
    lh_gx_entity_t *tick = lh_gx_checkbox_get_tick_mut(self);
    lh_gx_entity_t *caption = lh_gx_checkbox_get_caption_mut(self);

    lh_gx_entity_init(row, lh_addr_of(lh_gx_checkbox_class), lh_null, x, y, width, height);
    lh_gx_entity_set_content(row, self);
    lh_gx_entity_init(square, lh_addr_of(lh_gx_box_class), style, 0, 0, 0, 0);
    lh_gx_entity_init(tick, lh_addr_of(lh_gx_box_class), tick_style, 0, 0, 0, 0);
    lh_gx_entity_init(caption, lh_addr_of(lh_gx_text_class), style, 0, 0, 0, 0);
    lh_gx_entity_set_content(caption, lh_cast_const(lh_char_t *, text));
    lh_gx_entity_attach(row, square);
    lh_gx_entity_attach(square, tick);
    lh_gx_entity_attach(row, caption);
    lh_gx_checkbox_set_style(self, style);
    lh_gx_checkbox_set_tick_style(self, tick_style);
    lh_gx_checkbox_set_on_change(self, on_change);
    lh_gx_checkbox_set_context(self, context);
    lh_gx_tap_init(lh_gx_checkbox_get_tap_mut(self));
    lh_gx_checkbox_set_checked(self, checked);
    lh_gx_entity_set_visible(tick, checked);
    lh_gx_checkbox_lay_out(self);
}
