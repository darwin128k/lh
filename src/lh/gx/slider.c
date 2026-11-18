/**
 * @file slider.c
 * @brief Implementation of `lh/gx/slider.h`.
 */

#include <lh/gx/slider.h>

#include <lh/core/util/return.h>
#include <lh/gx/box.h>
#include <lh/gx/class/initializer.h>

const lh_gx_class_t lh_gx_slider_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, lh_gx_slider_input, lh_null);

const lh_gx_entity_t *
lh_gx_slider_get_row(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->row);
}

lh_gx_entity_t *
lh_gx_slider_get_row_mut(lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->row);
}

const lh_gx_entity_t *
lh_gx_slider_get_rail(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rail);
}

lh_gx_entity_t *
lh_gx_slider_get_rail_mut(lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rail);
}

const lh_gx_entity_t *
lh_gx_slider_get_fill(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_gx_entity_t *
lh_gx_slider_get_fill_mut(lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

const lh_gx_entity_t *
lh_gx_slider_get_knob(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->knob);
}

lh_gx_entity_t *
lh_gx_slider_get_knob_mut(lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->knob);
}

const lh_gx_style_t *
lh_gx_slider_get_style(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gx_slider_set_style(lh_gx_slider_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

const lh_gx_style_t *
lh_gx_slider_get_accent_style(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->accent_style;
}

lh_void
lh_gx_slider_set_accent_style(lh_gx_slider_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->accent_style = style;
}

const lh_gx_style_t *
lh_gx_slider_get_knob_style(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->knob_style;
}

lh_void
lh_gx_slider_set_knob_style(lh_gx_slider_t *self, const lh_gx_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->knob_style = style;
}

lh_s32_t
lh_gx_slider_get_min(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->min;
}

lh_void
lh_gx_slider_set_min(lh_gx_slider_t *self, lh_s32_t min)
{
    lh_assert_runtime_ref(self);
    self->min = min;
}

lh_s32_t
lh_gx_slider_get_max(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->max;
}

lh_void
lh_gx_slider_set_max(lh_gx_slider_t *self, lh_s32_t max)
{
    lh_assert_runtime_ref(self);
    self->max = max;
}

lh_s32_t
lh_gx_slider_get_value(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->value;
}

lh_void
lh_gx_slider_set_value(lh_gx_slider_t *self, lh_s32_t value)
{
    lh_assert_runtime_ref(self);
    self->value = value;
}

lh_s32_t
lh_gx_slider_get_start(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_gx_slider_set_start(lh_gx_slider_t *self, lh_s32_t start)
{
    lh_assert_runtime_ref(self);
    self->start = start;
}

lh_gx_slider_change_fn *
lh_gx_slider_get_on_change(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_change;
}

lh_void
lh_gx_slider_set_on_change(lh_gx_slider_t *self, lh_gx_slider_change_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->on_change = fn;
}

lh_ptr
lh_gx_slider_get_context(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_gx_slider_set_context(lh_gx_slider_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

const lh_gx_drag_t *
lh_gx_slider_get_drag(const lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->drag);
}

lh_gx_drag_t *
lh_gx_slider_get_drag_mut(lh_gx_slider_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->drag);
}

lh_gx_slider_t *
lh_gx_slider_from_entity(lh_gx_entity_t *entity)
{
    return lh_ptr_rcast(lh_gx_slider_t, lh_gx_entity_get_content(entity));
}

lh_s64_t
lh_gx_slider_get_travel(const lh_gx_slider_t *self)
{
    const lh_gx_size_t *size =
        lh_gx_rect_get_size(lh_gx_entity_get_place(lh_gx_slider_get_row(self)));
    const lh_s64_t width = lh_cast_static(lh_s64_t, lh_gx_size_get_width(size));
    const lh_s64_t height = lh_cast_static(lh_s64_t, lh_gx_size_get_height(size));

    return lh_math_max(width - height, 0);
}

lh_void
lh_gx_slider_change(lh_gx_slider_t *self)
{
    lh_gx_slider_change_fn *on_change = lh_gx_slider_get_on_change(self);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, on_change)));
    on_change(self, lh_gx_slider_get_value(self), lh_gx_slider_get_context(self));
}

lh_s64_t
lh_gx_slider_get_offset(const lh_gx_slider_t *self)
{
    const lh_s64_t travel = lh_gx_slider_get_travel(self);
    const lh_s64_t min = lh_gx_slider_get_min(self);
    const lh_s64_t range = lh_gx_slider_get_max(self) - min;

    lh_return_if(range <= 0 || travel <= 0, 0);
    return lh_math_div_round(travel * (lh_gx_slider_get_value(self) - min), range);
}

lh_s32_t
lh_gx_slider_get_value_at(const lh_gx_slider_t *self, const lh_gx_point_t *at)
{
    const lh_s64_t travel = lh_gx_slider_get_travel(self);
    const lh_s32_t min = lh_gx_slider_get_min(self);

    lh_return_if(travel <= 0, min);
    lh_gx_rect_t place;

    lh_gx_entity_get_screen_rect(lh_gx_slider_get_row(self), lh_addr_of(place));
    const lh_s64_t left =
        lh_cast_static(lh_s64_t, lh_gx_point_get_x(lh_gx_rect_get_origin(lh_addr_of(place))));
    const lh_s64_t half =
        lh_cast_static(lh_s64_t, lh_gx_size_get_height(lh_gx_rect_get_size(lh_addr_of(place)))) / 2;
    /* Held on the rail before the division, which rounds a non-negative number. */
    const lh_s64_t along =
        lh_math_clamp(lh_cast_static(lh_s64_t, lh_gx_point_get_x(at)) - left - half, 0, travel);
    const lh_s64_t range = lh_cast_static(lh_s64_t, lh_gx_slider_get_max(self)) - min;

    return lh_cast_static(lh_s32_t, min + lh_math_div_round(along * range, travel));
}

lh_void
lh_gx_slider_lay_out(lh_gx_slider_t *self)
{
    const lh_gx_size_t *size =
        lh_gx_rect_get_size(lh_gx_entity_get_place(lh_gx_slider_get_row(self)));
    const lh_gx_scalar_t side = lh_gx_size_get_height(size);
    const lh_gx_scalar_t travel = lh_gx_scalar_of(lh_gx_slider_get_travel(self));
    const lh_gx_scalar_t offset = lh_gx_scalar_of(lh_gx_slider_get_offset(self));
    const lh_gx_scalar_t thick = side / 4;
    const lh_gx_scalar_t top = lh_gx_scalar_centre(side, thick);
    lh_gx_rect_t place;

    lh_gx_rect_init(lh_addr_of(place), side / 2, top, travel, thick);
    lh_gx_entity_move(lh_gx_slider_get_rail_mut(self), place);
    lh_gx_rect_init(lh_addr_of(place), side / 2, top, offset, thick);
    lh_gx_entity_move(lh_gx_slider_get_fill_mut(self), place);
    lh_gx_rect_init(lh_addr_of(place), offset, 0, side, side);
    lh_gx_entity_move(lh_gx_slider_get_knob_mut(self), place);
}

lh_bool_t
lh_gx_slider_slide(lh_gx_slider_t *self, lh_s32_t value)
{
    const lh_s32_t held =
        lh_math_clamp(value, lh_gx_slider_get_min(self), lh_gx_slider_get_max(self));

    lh_return_if(held == lh_gx_slider_get_value(self), lh_bool_false);
    lh_gx_slider_set_value(self, held);
    lh_gx_slider_lay_out(self);
    return lh_bool_true;
}

lh_void
lh_gx_slider_init(lh_gx_slider_t *self, const lh_gx_style_t *style,
                  const lh_gx_style_t *accent_style, const lh_gx_style_t *knob_style,
                  lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t width, lh_gx_scalar_t height,
                  lh_s32_t min, lh_s32_t max, lh_s32_t value, lh_gx_slider_change_fn *on_change,
                  lh_ptr context)
{
    lh_gx_entity_t *row = lh_gx_slider_get_row_mut(self);

    lh_gx_entity_init(row, lh_addr_of(lh_gx_slider_class), lh_null, x, y, width, height);
    lh_gx_entity_set_content(row, self);
    lh_gx_entity_init(lh_gx_slider_get_rail_mut(self), lh_addr_of(lh_gx_box_class), style, 0, 0, 0,
                      0);
    lh_gx_entity_init(lh_gx_slider_get_fill_mut(self), lh_addr_of(lh_gx_box_class), accent_style, 0,
                      0, 0, 0);
    lh_gx_entity_init(lh_gx_slider_get_knob_mut(self), lh_addr_of(lh_gx_box_class), knob_style, 0,
                      0, 0, 0);
    lh_gx_entity_attach(row, lh_gx_slider_get_rail_mut(self));
    lh_gx_entity_attach(row, lh_gx_slider_get_fill_mut(self));
    lh_gx_entity_attach(row, lh_gx_slider_get_knob_mut(self));
    lh_gx_slider_set_style(self, style);
    lh_gx_slider_set_accent_style(self, accent_style);
    lh_gx_slider_set_knob_style(self, knob_style);
    lh_gx_slider_set_min(self, min);
    lh_gx_slider_set_max(self, max);
    lh_gx_slider_set_value(self, lh_math_clamp(value, min, max));
    lh_gx_slider_set_start(self, lh_gx_slider_get_value(self));
    lh_gx_slider_set_on_change(self, on_change);
    lh_gx_slider_set_context(self, context);
    lh_gx_drag_init(lh_gx_slider_get_drag_mut(self));
    lh_gx_slider_lay_out(self);
}

lh_void
lh_gx_slider_move(lh_gx_slider_t *self, lh_s32_t value)
{
    lh_return_if(!lh_gx_slider_slide(self, value));
    lh_gx_slider_change(self);
}

lh_void
lh_gx_slider_input(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    lh_gx_slider_t *self = lh_gx_slider_from_entity(entity);
    lh_gx_drag_t *drag = lh_gx_slider_get_drag_mut(self);

    switch (lh_gx_drag_take(drag, event))
    {
    case lh_gx_drag_step_begin:
        lh_gx_slider_set_start(self, lh_gx_slider_get_value(self));
        lh_gx_slider_move(self, lh_gx_slider_get_value_at(self, lh_gx_drag_get_at(drag)));
        break;
    case lh_gx_drag_step_move:
    case lh_gx_drag_step_end:
        lh_gx_slider_move(self, lh_gx_slider_get_value_at(self, lh_gx_drag_get_at(drag)));
        break;
    case lh_gx_drag_step_cancel:
        lh_gx_slider_move(self, lh_gx_slider_get_start(self));
        break;
    case lh_gx_drag_step_none:
    default:
        break;
    }
}
