/**
 * @file pad.c
 * @brief Diff one stick or pad into controller events.
 */
#include <lh/os/input/pad.h>

#include <lh/core/assert/runtime.h>
#include <lh/os/input/event.h>

lh_u8_t
lh_os_input_pad_get_device(const lh_os_input_pad_t *self)
{
    lh_assert_runtime_ref(self);
    return self->device;
}

lh_void
lh_os_input_pad_set_device(lh_os_input_pad_t *self, lh_u8_t device)
{
    lh_assert_runtime_ref(self);
    self->device = device;
}

lh_u32_t
lh_os_input_pad_get_buttons(const lh_os_input_pad_t *self)
{
    lh_assert_runtime_ref(self);
    return self->buttons;
}

lh_void
lh_os_input_pad_set_buttons(lh_os_input_pad_t *self, lh_u32_t buttons)
{
    lh_assert_runtime_ref(self);
    self->buttons = buttons;
}

lh_s16_t
lh_os_input_pad_get_value(const lh_os_input_pad_t *self, lh_os_input_axis_t axis)
{
    lh_assert_runtime_ref(self);
    return self->values[axis];
}

lh_void
lh_os_input_pad_set_value(lh_os_input_pad_t *self, lh_os_input_axis_t axis, lh_s16_t value)
{
    lh_assert_runtime_ref(self);
    self->values[axis] = value;
}

lh_void
lh_os_input_pad_init(lh_os_input_pad_t *self, lh_u8_t device)
{
    lh_os_input_pad_set_device(self, device);
    lh_os_input_pad_set_buttons(self, 0U);
    for (lh_u8_t axis = 0U; axis < LH_OS_INPUT_AXIS_COUNT; ++axis)
    {
        lh_os_input_pad_set_value(self, lh_cast_static(lh_os_input_axis_t, axis), 0);
    }
}

lh_void
lh_os_input_pad_report_axis(lh_os_input_pad_t *self, lh_os_input_axis_t axis, lh_s16_t value,
                            lh_tick_t time, lh_os_input_sink_fn *sink, lh_self_ptr sink_self)
{
    lh_assert_runtime_ref(self);
    if (axis == lh_os_input_axis_none || axis >= LH_OS_INPUT_AXIS_COUNT)
    {
        return;
    }
    if (lh_os_input_pad_get_value(self, axis) == value)
    {
        return;
    }
    lh_os_input_pad_set_value(self, axis, value);
    if (lh_ptr_is_null(sink))
    {
        return;
    }
    lh_os_input_event_t event;

    lh_os_input_event_init_controller_axis(lh_addr_of(event), lh_os_input_pad_get_device(self),
                                           axis, value, time);
    sink(sink_self, lh_addr_of(event));
}

lh_void
lh_os_input_pad_report_button(lh_os_input_pad_t *self, lh_os_input_button_t button, lh_bool_t down,
                              lh_tick_t time, lh_os_input_sink_fn *sink, lh_self_ptr sink_self)
{
    lh_assert_runtime_ref(self);
    if (button == lh_os_input_button_none)
    {
        return;
    }
    lh_u32_t bit = lh_os_input_button_to_mask(button);
    lh_u32_t buttons = lh_os_input_pad_get_buttons(self);
    lh_bool_t held = lh_bit_and(buttons, bit) != 0U ? lh_bool_true : lh_bool_false;

    if (held == down)
    {
        return;
    }
    if (down == lh_bool_true)
    {
        buttons = lh_bit_or(buttons, bit);
    }
    else
    {
        buttons = lh_bit_and(buttons, lh_bit_not(bit));
    }
    lh_os_input_pad_set_buttons(self, buttons);
    if (lh_ptr_is_null(sink))
    {
        return;
    }
    lh_os_input_event_t event;

    lh_os_input_event_init_controller_button(lh_addr_of(event), lh_os_input_pad_get_device(self),
                                             button, down, time);
    lh_os_input_event_set_buttons(lh_addr_of(event), buttons);
    sink(sink_self, lh_addr_of(event));
}
