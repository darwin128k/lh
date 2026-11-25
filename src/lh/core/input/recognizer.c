/**
 * @file recognizer.c
 * @brief Implementation of `lh/core/input/recognizer.h`.
 */

#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>
#include <lh/core/input/recognizer.h>
#include <lh/core/util/return.h>

/* -- Sending --------------------------------------------------------------- */

lh_void
lh_input_recognizer_send(lh_input_event_t *event, lh_input_flags_t flags, lh_input_sink_cb sink,
                         lh_ptr context)
{
    lh_input_event_set_flags(
        event, lh_type_cast(lh_input_flags_t, lh_input_event_get_flags(event) | flags));
    sink(context, event);
}

/* -- The recognizer -------------------------------------------------------- */

lh_u8_t
lh_input_recognizer_get_pad_device(const lh_input_recognizer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pad_device;
}

lh_void
lh_input_recognizer_set_pad_device(lh_input_recognizer_t *self, lh_u8_t pad_device)
{
    lh_assert_runtime_ref(self);
    self->pad_device = pad_device;
}

lh_u32_t
lh_input_recognizer_get_pad_buttons(const lh_input_recognizer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pad_buttons;
}

lh_void
lh_input_recognizer_set_pad_buttons(lh_input_recognizer_t *self, lh_u32_t pad_buttons)
{
    lh_assert_runtime_ref(self);
    self->pad_buttons = pad_buttons;
}

lh_tick_t
lh_input_recognizer_get_pad_repeat_at(const lh_input_recognizer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pad_repeat_at;
}

lh_void
lh_input_recognizer_set_pad_repeat_at(lh_input_recognizer_t *self, lh_tick_t pad_repeat_at)
{
    lh_assert_runtime_ref(self);
    self->pad_repeat_at = pad_repeat_at;
}

lh_bool_t
lh_input_recognizer_get_pad_armed(const lh_input_recognizer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pad_armed;
}

lh_void
lh_input_recognizer_set_pad_armed(lh_input_recognizer_t *self, lh_bool_t pad_armed)
{
    lh_assert_runtime_ref(self);
    self->pad_armed = pad_armed;
}

lh_u8_t
lh_input_recognizer_get_axis_device(const lh_input_recognizer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->axis_device;
}

lh_void
lh_input_recognizer_set_axis_device(lh_input_recognizer_t *self, lh_u8_t axis_device)
{
    lh_assert_runtime_ref(self);
    self->axis_device = axis_device;
}

lh_bool_t
lh_input_recognizer_get_axis_hot(const lh_input_recognizer_t *self, lh_input_axis_t axis)
{
    lh_assert_runtime_ref(self);
    return self->axis_hot[axis];
}

lh_void
lh_input_recognizer_set_axis_hot(lh_input_recognizer_t *self, lh_input_axis_t axis, lh_bool_t hot)
{
    lh_assert_runtime_ref(self);
    self->axis_hot[axis] = hot;
}

lh_s16_t
lh_input_recognizer_get_axis_out(const lh_input_recognizer_t *self, lh_input_axis_t axis)
{
    lh_assert_runtime_ref(self);
    return self->axis_out[axis];
}

lh_void
lh_input_recognizer_set_axis_out(lh_input_recognizer_t *self, lh_input_axis_t axis, lh_s16_t value)
{
    lh_assert_runtime_ref(self);
    self->axis_out[axis] = value;
}

lh_void
lh_input_recognizer_forget(lh_input_recognizer_t *self)
{
    for (lh_u8_t i = 0U; i < LH_INPUT_POINTER_MAX; ++i)
    {
        lh_input_recognizer_pointer_clear(lh_input_recognizer_get_pointer_mut(self, i));
    }
    lh_input_recognizer_set_pad_device(self, 0U);
    lh_input_recognizer_set_pad_buttons(self, 0U);
    lh_input_recognizer_set_pad_repeat_at(self, 0U);
    lh_input_recognizer_set_pad_armed(self, lh_bool_false);
    lh_input_recognizer_set_axis_device(self, 0U);
    for (lh_u8_t axis = 0U; axis < LH_INPUT_AXIS_COUNT; ++axis)
    {
        lh_input_axis_t which = lh_type_cast(lh_input_axis_t, axis);

        lh_input_recognizer_set_axis_hot(self, which, lh_bool_false);
        lh_input_recognizer_set_axis_out(self, which, 0);
    }
}

lh_void
lh_input_recognizer_init(lh_input_recognizer_t *self, lh_input_recognizer_config_cptr config)
{
    if (lh_null_eq(config))
    {
        lh_input_recognizer_config_init_default(lh_input_recognizer_get_config_mut(self));
    }
    else
    {
        lh_input_recognizer_set_config(self, config);
    }
    lh_input_recognizer_forget(self);
}

lh_input_recognizer_config_cptr
lh_input_recognizer_get_config(lh_input_recognizer_cptr self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->config);
}

lh_input_recognizer_pointer_ptr
lh_input_recognizer_find(lh_input_recognizer_t *self, lh_u8_t pointer)
{
    lh_return_if(pointer >= LH_INPUT_POINTER_MAX, lh_null);
    return lh_input_recognizer_get_pointer_mut(self, pointer);
}

lh_input_recognizer_pointer_cptr
lh_input_recognizer_find_const(lh_input_recognizer_cptr self, lh_u8_t pointer)
{
    lh_return_if(pointer >= LH_INPUT_POINTER_MAX, lh_null);
    return lh_input_recognizer_get_pointer(self, pointer);
}

lh_input_recognizer_phase_t
lh_input_recognizer_get_phase(lh_input_recognizer_cptr self, lh_u8_t pointer)
{
    lh_assert_runtime_ref(self);
    lh_input_recognizer_pointer_cptr state = lh_input_recognizer_find_const(self, pointer);
    lh_return_if(lh_null_eq(state), lh_input_recognizer_phase_idle);
    return lh_input_recognizer_pointer_get_phase(state);
}

lh_bool_t
lh_input_recognizer_is_dragging(lh_input_recognizer_cptr self, lh_u8_t pointer)
{
    return lh_type_cast(lh_bool_t, lh_input_recognizer_get_phase(self, pointer) ==
                                       lh_input_recognizer_phase_dragging);
}

lh_bool_t
lh_input_recognizer_get_long_press_due(lh_input_recognizer_cptr self,
                                       lh_input_recognizer_pointer_cptr state, lh_tick_t *out)
{
    const lh_u16_t long_press_ms =
        lh_input_recognizer_config_get_long_press_ms(lh_input_recognizer_get_config(self));

    lh_return_if(lh_input_recognizer_pointer_get_phase(state) !=
                         lh_input_recognizer_phase_pending ||
                     long_press_ms == 0U,
                 lh_bool_false);
    lh_ptr_deref(out) =
        lh_type_cast(lh_tick_t, lh_input_recognizer_pointer_get_down_time(state) + long_press_ms);
    return lh_bool_true;
}

lh_bool_t
lh_input_recognizer_get_next_deadline(lh_input_recognizer_cptr self, lh_tick_t *out)
{
    lh_bool_t found = lh_bool_false;
    lh_tick_t earliest = 0U;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    for (lh_u8_t i = 0U; i < LH_INPUT_POINTER_MAX; ++i)
    {
        lh_tick_t due;

        if (lh_input_recognizer_get_long_press_due(self, lh_input_recognizer_find_const(self, i),
                                                   lh_addr_of(due)) &&
            (!found || lh_tick_is_after(earliest, due)))
        {
            earliest = due;
            found = lh_bool_true;
        }
    }
    if (lh_input_recognizer_get_pad_armed(self) == lh_bool_true &&
        (!found || lh_tick_is_after(earliest, lh_input_recognizer_get_pad_repeat_at(self))))
    {
        earliest = lh_input_recognizer_get_pad_repeat_at(self);
        found = lh_bool_true;
    }
    lh_return_if(found == lh_bool_false, lh_bool_false);
    lh_ptr_deref(out) = earliest;
    return lh_bool_true;
}

lh_void
lh_input_recognizer_update(lh_input_recognizer_t *self, lh_tick_t now, lh_input_sink_cb sink,
                           lh_ptr context)
{
    lh_assert_runtime_ref(self);
    for (lh_u8_t i = 0U; i < LH_INPUT_POINTER_MAX; ++i)
    {
        lh_input_recognizer_pointer_ptr state = lh_input_recognizer_find(self, i);
        lh_tick_t due;

        if (lh_input_recognizer_get_long_press_due(self, state, lh_addr_of(due)) &&
            !lh_tick_is_after(due, now))
        {
            lh_input_event_t gesture;

            /* Held still long enough: a long press, and the up that ends it is no click. */
            lh_input_recognizer_pointer_set_phase(state, lh_input_recognizer_phase_holding);
            lh_input_recognizer_pointer_forget_click(state);
            lh_input_recognizer_pointer_make_at_last(state, i, lh_input_kind_long_press, now,
                                                     lh_addr_of(gesture));
            lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_NONE, sink, context);
        }
    }
    lh_input_recognizer_repeat_pad(self, now, sink, context);
}

lh_void
lh_input_recognizer_abandon(lh_input_recognizer_pointer_ptr state, lh_u8_t pointer, lh_tick_t time,
                            lh_input_sink_cb sink, lh_ptr context)
{
    lh_input_event_t gesture;

    if (lh_input_recognizer_pointer_get_phase(state) == lh_input_recognizer_phase_dragging)
    {
        lh_input_recognizer_pointer_make_at_last(state, pointer, lh_input_kind_drag_end, time,
                                                 lh_addr_of(gesture));
        lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_CANCELLED, sink, context);
    }
    lh_input_recognizer_pointer_set_phase(state, lh_input_recognizer_phase_idle);
    lh_input_recognizer_pointer_forget_click(state);
}

lh_void
lh_input_recognizer_cancel_all(lh_input_recognizer_t *self, lh_tick_t time, lh_input_sink_cb sink,
                               lh_ptr context)
{
    for (lh_u8_t i = 0U; i < LH_INPUT_POINTER_MAX; ++i)
    {
        lh_input_recognizer_pointer_ptr state = lh_input_recognizer_find(self, i);

        if (lh_input_recognizer_pointer_get_phase(state) != lh_input_recognizer_phase_idle)
        {
            lh_input_event_t cancel;

            lh_input_recognizer_pointer_make_at_last(state, i, lh_input_kind_pointer_cancel, time,
                                                     lh_addr_of(cancel));
            lh_input_event_set_buttons(lh_addr_of(cancel), 0U);
            lh_input_recognizer_send(lh_addr_of(cancel), LH_INPUT_FLAG_CANCELLED, sink, context);
            lh_input_recognizer_abandon(state, i, time, sink, context);
        }
        lh_input_recognizer_pointer_forget_click(state);
    }
}

lh_void
lh_input_recognizer_reset(lh_input_recognizer_t *self, lh_tick_t now, lh_input_sink_cb sink,
                          lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_input_recognizer_cancel_all(self, now, sink, context);
}

lh_void
lh_input_recognizer_on_down(lh_input_recognizer_pointer_ptr state, lh_input_event_cptr event)
{
    lh_return_if(lh_input_recognizer_pointer_get_phase(state) != lh_input_recognizer_phase_idle ||
                 lh_input_event_get_button(event) == lh_input_button_none);
    lh_input_recognizer_pointer_begin(state, event);
}

lh_void
lh_input_recognizer_on_move(lh_input_recognizer_t *self, lh_input_recognizer_pointer_ptr state,
                            lh_input_event_cptr event, lh_input_sink_cb sink, lh_ptr context)
{
    const lh_input_recognizer_phase_t phase = lh_input_recognizer_pointer_get_phase(state);

    if (phase == lh_input_recognizer_phase_dragging)
    {
        lh_s32_t dx;
        lh_s32_t dy;

        lh_input_recognizer_pointer_advance(state, event, lh_addr_of(dx), lh_addr_of(dy));
        lh_return_if(dx == 0 && dy == 0);
        lh_input_event_t gesture;

        lh_input_recognizer_pointer_make_from(state, event, lh_input_kind_drag,
                                              lh_addr_of(gesture));
        lh_input_event_set_delta(lh_addr_of(gesture), dx, dy);
        lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_NONE, sink, context);
        return;
    }
    lh_return_if(phase == lh_input_recognizer_phase_idle);
    lh_return_if(!lh_input_recognizer_pointer_is_past_slop(
        state, event, lh_input_recognizer_config_get_slop(lh_input_recognizer_get_config(self))));
    /* The drag starts from where the press went down, so its first distance is all of it. */
    lh_input_recognizer_pointer_set_phase(state, lh_input_recognizer_phase_dragging);
    lh_input_recognizer_pointer_forget_click(state);
    {
        lh_s32_t dx;
        lh_s32_t dy;
        lh_input_event_t gesture;

        lh_input_recognizer_pointer_advance(state, event, lh_addr_of(dx), lh_addr_of(dy));
        lh_input_recognizer_pointer_make_from(state, event, lh_input_kind_drag_begin,
                                              lh_addr_of(gesture));
        lh_input_event_set_delta(lh_addr_of(gesture), dx, dy);
        lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_NONE, sink, context);
    }
}

lh_void
lh_input_recognizer_on_up(lh_input_recognizer_t *self, lh_input_recognizer_pointer_ptr state,
                          lh_input_event_cptr event, lh_input_sink_cb sink, lh_ptr context)
{
    const lh_input_recognizer_phase_t phase = lh_input_recognizer_pointer_get_phase(state);
    lh_input_recognizer_config_cptr config = lh_input_recognizer_get_config(self);

    lh_return_if(phase == lh_input_recognizer_phase_idle ||
                 lh_input_event_get_button(event) != lh_input_recognizer_pointer_get_button(state));
    lh_input_recognizer_pointer_set_phase(state, lh_input_recognizer_phase_idle);
    if (phase == lh_input_recognizer_phase_dragging)
    {
        lh_s32_t dx;
        lh_s32_t dy;

        lh_input_recognizer_pointer_advance(state, event, lh_addr_of(dx), lh_addr_of(dy));
        lh_input_event_t gesture;
        lh_s32_t across =
            lh_input_event_get_x(event) - lh_input_recognizer_pointer_get_down_x(state);
        lh_s32_t down = lh_input_event_get_y(event) - lh_input_recognizer_pointer_get_down_y(state);
        lh_s32_t reach = across < 0 ? -across : across;
        lh_s32_t drop = down < 0 ? -down : down;
        lh_u16_t swipe_px = lh_input_recognizer_config_get_swipe_px(config);

        if (drop > reach)
        {
            reach = drop;
        }
        lh_input_recognizer_pointer_make_from(state, event, lh_input_kind_drag_end,
                                              lh_addr_of(gesture));
        lh_input_event_set_delta(lh_addr_of(gesture), dx, dy);
        lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_NONE, sink, context);
        if (swipe_px != 0U && reach >= lh_type_cast(lh_s32_t, swipe_px) * LH_INPUT_COORD_ONE)
        {
            lh_input_recognizer_pointer_make_from(state, event, lh_input_kind_swipe,
                                                  lh_addr_of(gesture));
            lh_input_event_set_delta(lh_addr_of(gesture), across, down);
            lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_NONE, sink, context);
        }
        return;
    }
    lh_return_if(phase != lh_input_recognizer_phase_pending);
    lh_u8_t count = lh_input_recognizer_pointer_count_click(
        state, event, lh_input_recognizer_config_get_double_click_ms(config),
        lh_input_recognizer_config_get_double_click_slop(config));

    lh_input_recognizer_pointer_remember_click(state, event, count);
    lh_input_event_t gesture;

    lh_input_recognizer_pointer_make_from(state, event, lh_input_kind_click, lh_addr_of(gesture));
    lh_input_event_set_count(lh_addr_of(gesture), count);
    lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_NONE, sink, context);
    lh_return_if(count != 2U);
    lh_input_recognizer_pointer_make_from(state, event, lh_input_kind_double_click,
                                          lh_addr_of(gesture));
    lh_input_event_set_count(lh_addr_of(gesture), count);
    lh_input_recognizer_send(lh_addr_of(gesture), LH_INPUT_FLAG_NONE, sink, context);
}

lh_void
lh_input_recognizer_on_pointer(lh_input_recognizer_t *self, lh_input_event_cptr event,
                               lh_input_sink_cb sink, lh_ptr context)
{
    lh_input_recognizer_pointer_ptr state =
        lh_input_recognizer_find(self, lh_input_event_get_pointer(event));

    lh_return_if(lh_null_eq(state));
    switch (lh_input_event_get_kind(event))
    {
    case lh_input_kind_pointer_down:
        lh_input_recognizer_on_down(state, event);
        break;
    case lh_input_kind_pointer_move:
        lh_input_recognizer_on_move(self, state, event, sink, context);
        break;
    case lh_input_kind_pointer_up:
        lh_input_recognizer_on_up(self, state, event, sink, context);
        break;
    case lh_input_kind_pointer_cancel:
        lh_input_recognizer_abandon(state, lh_input_event_get_pointer(event),
                                    lh_input_event_get_time(event), sink, context);
        break;
    default:
        break;
    }
}

lh_bool_t
lh_input_recognizer_send_shifted_wheel(lh_input_recognizer_t *self, lh_input_event_cptr event,
                                       lh_input_sink_cb sink, lh_ptr context)
{
    lh_return_if(lh_input_event_get_kind(event) != lh_input_kind_wheel ||
                     !lh_input_recognizer_config_is_wheel_shift_horizontal(
                         lh_input_recognizer_get_config(self)) ||
                     !lh_input_event_has_mods(event, LH_INPUT_MODS_SHIFT) ||
                     lh_input_event_get_dx(event) != 0,
                 lh_bool_false);
    lh_input_event_t sideways = lh_ptr_deref(event);
    lh_input_event_set_delta(lh_addr_of(sideways), lh_input_event_get_dy(event), 0);
    sink(context, lh_addr_of(sideways));
    return lh_bool_true;
}

lh_bool_t
lh_input_recognizer_take_axis(lh_input_recognizer_t *self, lh_input_event_cptr event,
                              lh_input_sink_cb sink, lh_ptr context)
{
    if (lh_input_event_get_kind(event) != lh_input_kind_controller_axis)
    {
        return lh_bool_false;
    }
    lh_u16_t deadzone =
        lh_input_recognizer_config_get_axis_deadzone(lh_input_recognizer_get_config(self));

    if (deadzone == 0U)
    {
        return lh_bool_false;
    }
    lh_input_axis_t axis = lh_input_event_get_axis(event);

    if (axis == lh_input_axis_none || axis >= LH_INPUT_AXIS_COUNT)
    {
        return lh_bool_false;
    }
    lh_u8_t device = lh_input_event_get_device(event);

    if (device != lh_input_recognizer_get_axis_device(self))
    {
        lh_input_recognizer_set_axis_device(self, device);
        for (lh_u8_t index = 0U; index < LH_INPUT_AXIS_COUNT; ++index)
        {
            lh_input_axis_t which = lh_type_cast(lh_input_axis_t, index);

            lh_input_recognizer_set_axis_hot(self, which, lh_bool_false);
            lh_input_recognizer_set_axis_out(self, which, 0);
        }
    }
    lh_s16_t value = lh_input_event_get_value(event);
    lh_s32_t magnitude = value < 0 ? lh_type_cast(lh_s32_t, -lh_type_cast(lh_s32_t, value)) : value;
    lh_u16_t inner =
        lh_input_recognizer_config_get_axis_hysteresis(lh_input_recognizer_get_config(self));

    inner = deadzone > inner ? lh_type_cast(lh_u16_t, deadzone - inner) : 0U;
    lh_bool_t hot = lh_input_recognizer_get_axis_hot(self, axis);
    lh_s16_t out;
    if (hot == lh_bool_true && magnitude <= lh_type_cast(lh_s32_t, inner))
    {
        hot = lh_bool_false;
        out = 0;
    }
    else if (hot == lh_bool_false && magnitude <= lh_type_cast(lh_s32_t, deadzone))
    {
        out = 0;
    }
    else
    {
        hot = lh_bool_true;
        out = value;
    }
    lh_input_recognizer_set_axis_hot(self, axis, hot);
    if (out == lh_input_recognizer_get_axis_out(self, axis))
    {
        return lh_bool_true;
    }
    lh_input_recognizer_set_axis_out(self, axis, out);
    lh_input_event_t filtered = lh_ptr_deref(event);
    lh_input_event_set_value(lh_addr_of(filtered), out);
    sink(context, lh_addr_of(filtered));
    return lh_bool_true;
}

lh_void
lh_input_recognizer_send_step(lh_input_event_cptr event, lh_input_kind_t kind, lh_s32_t dx,
                              lh_s32_t dy, lh_input_sink_cb sink, lh_ptr context)
{
    lh_input_event_t step = lh_ptr_deref(event);

    lh_input_event_set_kind(lh_addr_of(step), kind);
    lh_input_event_set_delta(lh_addr_of(step), dx, dy);
    lh_input_recognizer_send(lh_addr_of(step), LH_INPUT_FLAG_SYNTHESIZED, sink, context);
}

lh_void
lh_input_recognizer_on_command(lh_input_recognizer_t *self, lh_input_event_cptr event,
                               lh_input_sink_cb sink, lh_ptr context)
{
    lh_input_kind_t kind = lh_input_event_get_kind(event);

    if (kind == lh_input_kind_key_down)
    {
        lh_key_t key = lh_input_event_get_key(event);

        if (key == lh_key_left)
        {
            lh_input_recognizer_send_step(event, lh_input_kind_navigate, -1, 0, sink, context);
        }
        else if (key == lh_key_right)
        {
            lh_input_recognizer_send_step(event, lh_input_kind_navigate, 1, 0, sink, context);
        }
        else if (key == lh_key_up)
        {
            lh_input_recognizer_send_step(event, lh_input_kind_navigate, 0, -1, sink, context);
        }
        else if (key == lh_key_down)
        {
            lh_input_recognizer_send_step(event, lh_input_kind_navigate, 0, 1, sink, context);
        }
        else if (key == lh_key_enter || key == lh_key_space)
        {
            lh_input_recognizer_send_step(event, lh_input_kind_activate, 0, 0, sink, context);
        }
        return;
    }
    if (kind != lh_input_kind_controller_button_down && kind != lh_input_kind_controller_button_up)
    {
        return;
    }
    lh_u8_t device = lh_input_event_get_device(event);
    lh_input_button_t button = lh_input_event_get_button(event);
    lh_u32_t bit = lh_input_button_to_mask(button);
    lh_u32_t held;

    if (device != lh_input_recognizer_get_pad_device(self))
    {
        lh_input_recognizer_set_pad_device(self, device);
        lh_input_recognizer_set_pad_buttons(self, 0U);
        lh_input_recognizer_set_pad_armed(self, lh_bool_false);
    }
    held = lh_input_recognizer_get_pad_buttons(self);
    if (kind == lh_input_kind_controller_button_down)
    {
        held = lh_bit_or(held, bit);
    }
    else
    {
        held = lh_bit_and(held, lh_bit_not(bit));
    }
    lh_input_recognizer_set_pad_buttons(self, held);
    if (held == 0U)
    {
        lh_input_recognizer_set_pad_armed(self, lh_bool_false);
    }
    else if (lh_input_recognizer_get_pad_armed(self) == lh_bool_false &&
             lh_input_recognizer_config_get_repeat_delay_ms(lh_input_recognizer_get_config(self)) !=
                 0U &&
             lh_input_recognizer_config_get_repeat_rate_ms(lh_input_recognizer_get_config(self)) !=
                 0U)
    {
        lh_u16_t delay =
            lh_input_recognizer_config_get_repeat_delay_ms(lh_input_recognizer_get_config(self));

        lh_input_recognizer_set_pad_repeat_at(
            self, lh_type_cast(lh_tick_t, lh_input_event_get_time(event) + delay));
        lh_input_recognizer_set_pad_armed(self, lh_bool_true);
    }
    if (kind != lh_input_kind_controller_button_down)
    {
        return;
    }
    if (button == lh_input_button_dpad_left)
    {
        lh_input_recognizer_send_step(event, lh_input_kind_navigate, -1, 0, sink, context);
    }
    else if (button == lh_input_button_dpad_right)
    {
        lh_input_recognizer_send_step(event, lh_input_kind_navigate, 1, 0, sink, context);
    }
    else if (button == lh_input_button_dpad_up)
    {
        lh_input_recognizer_send_step(event, lh_input_kind_navigate, 0, -1, sink, context);
    }
    else if (button == lh_input_button_dpad_down)
    {
        lh_input_recognizer_send_step(event, lh_input_kind_navigate, 0, 1, sink, context);
    }
    else if (button == lh_input_button_south)
    {
        lh_input_recognizer_send_step(event, lh_input_kind_activate, 0, 0, sink, context);
    }
}

lh_void
lh_input_recognizer_repeat_pad(lh_input_recognizer_t *self, lh_tick_t now, lh_input_sink_cb sink,
                               lh_ptr context)
{
    if (lh_input_recognizer_get_pad_armed(self) == lh_bool_false ||
        lh_tick_is_after(lh_input_recognizer_get_pad_repeat_at(self), now))
    {
        return;
    }
    lh_u16_t rate =
        lh_input_recognizer_config_get_repeat_rate_ms(lh_input_recognizer_get_config(self));

    if (rate == 0U)
    {
        lh_input_recognizer_set_pad_armed(self, lh_bool_false);
        return;
    }
    lh_u32_t held = lh_input_recognizer_get_pad_buttons(self);
    for (lh_u8_t index = lh_input_button_south; index <= lh_input_button_dpad_right; ++index)
    {
        lh_input_button_t button = lh_type_cast(lh_input_button_t, index);
        lh_input_event_t again;

        if (lh_bit_and(held, lh_input_button_to_mask(button)) == 0U)
        {
            continue;
        }
        lh_input_event_init_controller_button(
            lh_addr_of(again), lh_input_recognizer_get_pad_device(self), button, lh_bool_true, now);
        lh_input_event_set_buttons(lh_addr_of(again), held);
        lh_input_recognizer_send(lh_addr_of(again), LH_INPUT_FLAG_REPEAT, sink, context);
        if (button == lh_input_button_dpad_left)
        {
            lh_input_recognizer_send_step(lh_addr_of(again), lh_input_kind_navigate, -1, 0, sink,
                                          context);
        }
        else if (button == lh_input_button_dpad_right)
        {
            lh_input_recognizer_send_step(lh_addr_of(again), lh_input_kind_navigate, 1, 0, sink,
                                          context);
        }
        else if (button == lh_input_button_dpad_up)
        {
            lh_input_recognizer_send_step(lh_addr_of(again), lh_input_kind_navigate, 0, -1, sink,
                                          context);
        }
        else if (button == lh_input_button_dpad_down)
        {
            lh_input_recognizer_send_step(lh_addr_of(again), lh_input_kind_navigate, 0, 1, sink,
                                          context);
        }
        else if (button == lh_input_button_south)
        {
            lh_input_recognizer_send_step(lh_addr_of(again), lh_input_kind_activate, 0, 0, sink,
                                          context);
        }
    }
    lh_input_recognizer_set_pad_repeat_at(self, lh_type_cast(lh_tick_t, now + rate));
}

lh_void
lh_input_recognizer_feed(lh_input_recognizer_t *self, lh_input_event_cptr event,
                         lh_input_sink_cb sink, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    lh_input_recognizer_update(self, lh_input_event_get_time(event), sink, context);
    lh_return_if(lh_input_recognizer_send_shifted_wheel(self, event, sink, context));
    lh_return_if(lh_input_recognizer_take_axis(self, event, sink, context));
    sink(context, event);
    lh_return_if(lh_input_event_is_synthesized(event));
    if (lh_input_event_get_kind(event) == lh_input_kind_focus_out)
    {
        lh_input_recognizer_cancel_all(self, lh_input_event_get_time(event), sink, context);
        return;
    }
    lh_input_recognizer_on_pointer(self, event, sink, context);
    lh_input_recognizer_on_command(self, event, sink, context);
}

lh_input_recognizer_config_ptr
lh_input_recognizer_get_config_mut(lh_input_recognizer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->config);
}

lh_void
lh_input_recognizer_set_config(lh_input_recognizer_t *self, lh_input_recognizer_config_cptr config)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(config);
    self->config = lh_ptr_deref(config);
}

lh_input_recognizer_pointer_cptr
lh_input_recognizer_get_pointer(lh_input_recognizer_cptr self, lh_u8_t pointer)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointers[pointer]);
}

lh_input_recognizer_pointer_ptr
lh_input_recognizer_get_pointer_mut(lh_input_recognizer_t *self, lh_u8_t pointer)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointers[pointer]);
}
