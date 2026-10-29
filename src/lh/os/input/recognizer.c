/**
 * @file recognizer.c
 * @brief Implementation of `lh/os/input/recognizer.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/os/input/recognizer.h>
#include <lh/util/return.h>

/* -- Sending --------------------------------------------------------------- */

lh_void
lh_os_input_recognizer_send(lh_os_input_event_t *event, lh_os_input_flags_t flags,
                            lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_os_input_event_set_flags(
        event, lh_type_cast(lh_os_input_flags_t, lh_os_input_event_get_flags(event) | flags));
    sink(context, event);
}

/* -- The recognizer -------------------------------------------------------- */

lh_void
lh_os_input_recognizer_init(lh_os_input_recognizer_t *self,
                            lh_os_input_recognizer_config_cptr config)
{
    lh_u8_t i;

    lh_assert_runtime_ref(self);
    if (lh_null_eq(config))
    {
        lh_os_input_recognizer_config_init_default(lh_os_input_recognizer_get_config_mut(self));
    }
    else
    {
        lh_os_input_recognizer_set_config(self, config);
    }
    for (i = 0U; i < LH_OS_INPUT_POINTER_MAX; ++i)
    {
        lh_os_input_recognizer_pointer_clear(lh_os_input_recognizer_get_pointer_mut(self, i));
    }
}

lh_os_input_recognizer_config_cptr
lh_os_input_recognizer_get_config(lh_os_input_recognizer_cptr self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->config);
}

lh_os_input_recognizer_pointer_ptr
lh_os_input_recognizer_find(lh_os_input_recognizer_t *self, lh_u8_t pointer)
{
    lh_return_if(pointer >= LH_OS_INPUT_POINTER_MAX, lh_null);
    return lh_os_input_recognizer_get_pointer_mut(self, pointer);
}

lh_os_input_recognizer_pointer_cptr
lh_os_input_recognizer_find_const(lh_os_input_recognizer_cptr self, lh_u8_t pointer)
{
    lh_return_if(pointer >= LH_OS_INPUT_POINTER_MAX, lh_null);
    return lh_os_input_recognizer_get_pointer(self, pointer);
}

lh_os_input_recognizer_phase_t
lh_os_input_recognizer_get_phase(lh_os_input_recognizer_cptr self, lh_u8_t pointer)
{
    lh_os_input_recognizer_pointer_cptr state;

    lh_assert_runtime_ref(self);
    state = lh_os_input_recognizer_find_const(self, pointer);
    lh_return_if(lh_null_eq(state), lh_os_input_recognizer_phase_idle);
    return lh_os_input_recognizer_pointer_get_phase(state);
}

lh_bool_t
lh_os_input_recognizer_is_dragging(lh_os_input_recognizer_cptr self, lh_u8_t pointer)
{
    return lh_type_cast(lh_bool_t, lh_os_input_recognizer_get_phase(self, pointer) ==
                                       lh_os_input_recognizer_phase_dragging);
}

lh_bool_t
lh_os_input_recognizer_get_long_press_due(lh_os_input_recognizer_cptr self,
                                          lh_os_input_recognizer_pointer_cptr state, lh_tick_t *out)
{
    const lh_u16_t long_press_ms =
        lh_os_input_recognizer_config_get_long_press_ms(lh_os_input_recognizer_get_config(self));

    lh_return_if(lh_os_input_recognizer_pointer_get_phase(state) !=
                         lh_os_input_recognizer_phase_pending ||
                     long_press_ms == 0U,
                 lh_bool_false);
    lh_ptr_deref(out) = lh_type_cast(
        lh_tick_t, lh_os_input_recognizer_pointer_get_down_time(state) + long_press_ms);
    return lh_bool_true;
}

lh_bool_t
lh_os_input_recognizer_get_next_deadline(lh_os_input_recognizer_cptr self, lh_tick_t *out)
{
    lh_bool_t found = lh_bool_false;
    lh_tick_t earliest = 0U;
    lh_tick_t due;
    lh_u8_t i;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    for (i = 0U; i < LH_OS_INPUT_POINTER_MAX; ++i)
    {
        if (lh_os_input_recognizer_get_long_press_due(
                self, lh_os_input_recognizer_find_const(self, i), lh_addr_of(due)) &&
            (!found || lh_tick_is_after(earliest, due)))
        {
            earliest = due;
            found = lh_bool_true;
        }
    }
    lh_return_if(!found, lh_bool_false);
    lh_ptr_deref(out) = earliest;
    return lh_bool_true;
}

lh_void
lh_os_input_recognizer_update(lh_os_input_recognizer_t *self, lh_tick_t now,
                              lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_os_input_recognizer_pointer_ptr state;
    lh_os_input_event_t gesture;
    lh_tick_t due;
    lh_u8_t i;

    lh_assert_runtime_ref(self);
    for (i = 0U; i < LH_OS_INPUT_POINTER_MAX; ++i)
    {
        state = lh_os_input_recognizer_find(self, i);
        if (lh_os_input_recognizer_get_long_press_due(self, state, lh_addr_of(due)) &&
            !lh_tick_is_after(due, now))
        {
            /* Held still long enough: a long press, and the up that ends it is no click. */
            lh_os_input_recognizer_pointer_set_phase(state, lh_os_input_recognizer_phase_holding);
            lh_os_input_recognizer_pointer_forget_click(state);
            lh_os_input_recognizer_pointer_make_at_last(state, i, lh_os_input_kind_long_press, now,
                                                        lh_addr_of(gesture));
            lh_os_input_recognizer_send(lh_addr_of(gesture), LH_OS_INPUT_FLAG_NONE, sink, context);
        }
    }
}

lh_void
lh_os_input_recognizer_abandon(lh_os_input_recognizer_pointer_ptr state, lh_u8_t pointer,
                               lh_tick_t time, lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_os_input_event_t gesture;

    if (lh_os_input_recognizer_pointer_get_phase(state) == lh_os_input_recognizer_phase_dragging)
    {
        lh_os_input_recognizer_pointer_make_at_last(state, pointer, lh_os_input_kind_drag_end, time,
                                                    lh_addr_of(gesture));
        lh_os_input_recognizer_send(lh_addr_of(gesture), LH_OS_INPUT_FLAG_CANCELLED, sink, context);
    }
    lh_os_input_recognizer_pointer_set_phase(state, lh_os_input_recognizer_phase_idle);
    lh_os_input_recognizer_pointer_forget_click(state);
}

lh_void
lh_os_input_recognizer_cancel_all(lh_os_input_recognizer_t *self, lh_tick_t time,
                                  lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_os_input_recognizer_pointer_ptr state;
    lh_os_input_event_t cancel;
    lh_u8_t i;

    for (i = 0U; i < LH_OS_INPUT_POINTER_MAX; ++i)
    {
        state = lh_os_input_recognizer_find(self, i);
        if (lh_os_input_recognizer_pointer_get_phase(state) != lh_os_input_recognizer_phase_idle)
        {
            lh_os_input_recognizer_pointer_make_at_last(state, i, lh_os_input_kind_pointer_cancel,
                                                        time, lh_addr_of(cancel));
            lh_os_input_event_set_buttons(lh_addr_of(cancel), 0U);
            lh_os_input_recognizer_send(lh_addr_of(cancel), LH_OS_INPUT_FLAG_CANCELLED, sink,
                                        context);
            lh_os_input_recognizer_abandon(state, i, time, sink, context);
        }
        lh_os_input_recognizer_pointer_forget_click(state);
    }
}

lh_void
lh_os_input_recognizer_reset(lh_os_input_recognizer_t *self, lh_tick_t now,
                             lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_os_input_recognizer_cancel_all(self, now, sink, context);
}

lh_void
lh_os_input_recognizer_on_down(lh_os_input_recognizer_pointer_ptr state,
                               lh_os_input_event_cptr event)
{
    lh_return_if(lh_os_input_recognizer_pointer_get_phase(state) !=
                     lh_os_input_recognizer_phase_idle ||
                 lh_os_input_event_get_button(event) == lh_os_input_button_none);
    lh_os_input_recognizer_pointer_begin(state, event);
}

lh_void
lh_os_input_recognizer_on_move(lh_os_input_recognizer_t *self,
                               lh_os_input_recognizer_pointer_ptr state,
                               lh_os_input_event_cptr event, lh_os_input_sink_cb sink,
                               lh_ptr context)
{
    const lh_os_input_recognizer_phase_t phase = lh_os_input_recognizer_pointer_get_phase(state);
    lh_os_input_event_t gesture;
    lh_s32_t dx;
    lh_s32_t dy;

    if (phase == lh_os_input_recognizer_phase_dragging)
    {
        lh_os_input_recognizer_pointer_advance(state, event, lh_addr_of(dx), lh_addr_of(dy));
        lh_return_if(dx == 0 && dy == 0);
        lh_os_input_recognizer_pointer_make_from(state, event, lh_os_input_kind_drag,
                                                 lh_addr_of(gesture));
        lh_os_input_event_set_delta(lh_addr_of(gesture), dx, dy);
        lh_os_input_recognizer_send(lh_addr_of(gesture), LH_OS_INPUT_FLAG_NONE, sink, context);
        return;
    }
    lh_return_if(phase == lh_os_input_recognizer_phase_idle);
    lh_return_if(!lh_os_input_recognizer_pointer_is_past_slop(
        state, event,
        lh_os_input_recognizer_config_get_slop(lh_os_input_recognizer_get_config(self))));
    /* The drag starts from where the press went down, so its first distance is all of it. */
    lh_os_input_recognizer_pointer_set_phase(state, lh_os_input_recognizer_phase_dragging);
    lh_os_input_recognizer_pointer_forget_click(state);
    lh_os_input_recognizer_pointer_advance(state, event, lh_addr_of(dx), lh_addr_of(dy));
    lh_os_input_recognizer_pointer_make_from(state, event, lh_os_input_kind_drag_begin,
                                             lh_addr_of(gesture));
    lh_os_input_event_set_delta(lh_addr_of(gesture), dx, dy);
    lh_os_input_recognizer_send(lh_addr_of(gesture), LH_OS_INPUT_FLAG_NONE, sink, context);
}

lh_void
lh_os_input_recognizer_on_up(lh_os_input_recognizer_t *self,
                             lh_os_input_recognizer_pointer_ptr state, lh_os_input_event_cptr event,
                             lh_os_input_sink_cb sink, lh_ptr context)
{
    const lh_os_input_recognizer_phase_t phase = lh_os_input_recognizer_pointer_get_phase(state);
    lh_os_input_recognizer_config_cptr config = lh_os_input_recognizer_get_config(self);
    lh_os_input_event_t gesture;
    lh_s32_t dx;
    lh_s32_t dy;
    lh_u8_t count;

    lh_return_if(phase == lh_os_input_recognizer_phase_idle ||
                 lh_os_input_event_get_button(event) !=
                     lh_os_input_recognizer_pointer_get_button(state));
    lh_os_input_recognizer_pointer_set_phase(state, lh_os_input_recognizer_phase_idle);
    if (phase == lh_os_input_recognizer_phase_dragging)
    {
        lh_os_input_recognizer_pointer_advance(state, event, lh_addr_of(dx), lh_addr_of(dy));
        lh_os_input_recognizer_pointer_make_from(state, event, lh_os_input_kind_drag_end,
                                                 lh_addr_of(gesture));
        lh_os_input_event_set_delta(lh_addr_of(gesture), dx, dy);
        lh_os_input_recognizer_send(lh_addr_of(gesture), LH_OS_INPUT_FLAG_NONE, sink, context);
        return;
    }
    lh_return_if(phase != lh_os_input_recognizer_phase_pending);
    count = lh_os_input_recognizer_pointer_count_click(
        state, event, lh_os_input_recognizer_config_get_double_click_ms(config),
        lh_os_input_recognizer_config_get_double_click_slop(config));
    lh_os_input_recognizer_pointer_remember_click(state, event, count);
    lh_os_input_recognizer_pointer_make_from(state, event, lh_os_input_kind_click,
                                             lh_addr_of(gesture));
    lh_os_input_event_set_count(lh_addr_of(gesture), count);
    lh_os_input_recognizer_send(lh_addr_of(gesture), LH_OS_INPUT_FLAG_NONE, sink, context);
    lh_return_if(count != 2U);
    lh_os_input_recognizer_pointer_make_from(state, event, lh_os_input_kind_double_click,
                                             lh_addr_of(gesture));
    lh_os_input_event_set_count(lh_addr_of(gesture), count);
    lh_os_input_recognizer_send(lh_addr_of(gesture), LH_OS_INPUT_FLAG_NONE, sink, context);
}

lh_void
lh_os_input_recognizer_on_pointer(lh_os_input_recognizer_t *self, lh_os_input_event_cptr event,
                                  lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_os_input_recognizer_pointer_ptr state =
        lh_os_input_recognizer_find(self, lh_os_input_event_get_pointer(event));

    lh_return_if(lh_null_eq(state));
    switch (lh_os_input_event_get_kind(event))
    {
    case lh_os_input_kind_pointer_down:
        lh_os_input_recognizer_on_down(state, event);
        break;
    case lh_os_input_kind_pointer_move:
        lh_os_input_recognizer_on_move(self, state, event, sink, context);
        break;
    case lh_os_input_kind_pointer_up:
        lh_os_input_recognizer_on_up(self, state, event, sink, context);
        break;
    case lh_os_input_kind_pointer_cancel:
        lh_os_input_recognizer_abandon(state, lh_os_input_event_get_pointer(event),
                                       lh_os_input_event_get_time(event), sink, context);
        break;
    default:
        break;
    }
}

lh_bool_t
lh_os_input_recognizer_send_shifted_wheel(lh_os_input_recognizer_t *self,
                                          lh_os_input_event_cptr event, lh_os_input_sink_cb sink,
                                          lh_ptr context)
{
    lh_os_input_event_t sideways;

    lh_return_if(lh_os_input_event_get_kind(event) != lh_os_input_kind_wheel ||
                     !lh_os_input_recognizer_config_is_wheel_shift_horizontal(
                         lh_os_input_recognizer_get_config(self)) ||
                     !lh_os_input_event_has_mods(event, LH_OS_INPUT_MODS_SHIFT) ||
                     lh_os_input_event_get_dx(event) != 0,
                 lh_bool_false);
    sideways = lh_ptr_deref(event);
    lh_os_input_event_set_delta(lh_addr_of(sideways), lh_os_input_event_get_dy(event), 0);
    sink(context, lh_addr_of(sideways));
    return lh_bool_true;
}

lh_void
lh_os_input_recognizer_feed(lh_os_input_recognizer_t *self, lh_os_input_event_cptr event,
                            lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(event);
    lh_os_input_recognizer_update(self, lh_os_input_event_get_time(event), sink, context);
    lh_return_if(lh_os_input_recognizer_send_shifted_wheel(self, event, sink, context));
    sink(context, event);
    lh_return_if(lh_os_input_event_is_synthesized(event));
    if (lh_os_input_event_get_kind(event) == lh_os_input_kind_focus_out)
    {
        lh_os_input_recognizer_cancel_all(self, lh_os_input_event_get_time(event), sink, context);
        return;
    }
    lh_os_input_recognizer_on_pointer(self, event, sink, context);
}

lh_os_input_recognizer_config_ptr
lh_os_input_recognizer_get_config_mut(lh_os_input_recognizer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->config);
}

lh_void
lh_os_input_recognizer_set_config(lh_os_input_recognizer_t *self,
                                  lh_os_input_recognizer_config_cptr config)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(config);
    self->config = lh_ptr_deref(config);
}

lh_os_input_recognizer_pointer_cptr
lh_os_input_recognizer_get_pointer(lh_os_input_recognizer_cptr self, lh_u8_t pointer)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointers[pointer]);
}

lh_os_input_recognizer_pointer_ptr
lh_os_input_recognizer_get_pointer_mut(lh_os_input_recognizer_t *self, lh_u8_t pointer)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pointers[pointer]);
}
