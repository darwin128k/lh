/**
 * @file recognizer.c
 * @brief Implementation of `lh/os/input/recognizer.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/os/input/recognizer.h>
#include <lh/os/input/recognizer/pointer/initializer.h>
#include <lh/util/return.h>

/** @brief The pointer state a recognizer works on, inside `recognizer.c` only. */
#define lh_os_input_recognizer_pointer_ptr lh_ptr_of(lh_os_input_recognizer_pointer_t)
/** @brief The read-only pointer state, inside `recognizer.c` only. */
#define lh_os_input_recognizer_pointer_cptr lh_cptr_of(lh_os_input_recognizer_pointer_t)

/* -- Distances ------------------------------------------------------------- */

/** True when (@p x, @p y) is more than @p slop pixels from (@p from_x, @p from_y) on either
   axis. */
static lh_bool_t
lh_os_input_recognizer_is_beyond(lh_os_input_coord_t from_x, lh_os_input_coord_t from_y,
                                 lh_os_input_coord_t x, lh_os_input_coord_t y, lh_u16_t slop)
{
    const lh_os_input_coord_t limit = lh_os_input_coord_from_int(lh_type_cast(lh_s32_t, slop));

    return lh_type_cast(lh_bool_t,
                        lh_math_abs(x - from_x) > limit || lh_math_abs(y - from_y) > limit);
}

/* -- One pointer ----------------------------------------------------------- */

/** Nothing held, no click series to continue. */
static lh_void
lh_os_input_recognizer_pointer_clear(lh_os_input_recognizer_pointer_ptr self)
{
    static const lh_os_input_recognizer_pointer_t idle =
        lh_os_input_recognizer_pointer_idle_initializer();

    lh_ptr_deref(self) = idle;
}

static lh_os_input_recognizer_phase_t
lh_os_input_recognizer_pointer_get_phase(lh_os_input_recognizer_pointer_cptr self)
{
    return self->phase;
}

static lh_void
lh_os_input_recognizer_pointer_set_phase(lh_os_input_recognizer_pointer_ptr self,
                                         lh_os_input_recognizer_phase_t phase)
{
    self->phase = phase;
}

static lh_os_input_button_t
lh_os_input_recognizer_pointer_get_button(lh_os_input_recognizer_pointer_cptr self)
{
    return self->button;
}

static lh_tick_t
lh_os_input_recognizer_pointer_get_down_time(lh_os_input_recognizer_pointer_cptr self)
{
    return self->down_time;
}

/** No click series to continue: a drag, a long press or a cancel came in between. */
static lh_void
lh_os_input_recognizer_pointer_forget_click(lh_os_input_recognizer_pointer_ptr self)
{
    self->click_count = 0U;
}

/** Start a press from @p event (a pointer down). */
static lh_void
lh_os_input_recognizer_pointer_begin(lh_os_input_recognizer_pointer_ptr self,
                                     lh_os_input_event_cptr event)
{
    self->phase = lh_os_input_recognizer_phase_pending;
    self->device_kind = lh_os_input_event_get_device_kind(event);
    self->button = lh_os_input_event_get_button(event);
    self->mods = lh_os_input_event_get_mods(event);
    self->down_x = lh_os_input_event_get_x(event);
    self->down_y = lh_os_input_event_get_y(event);
    self->down_time = lh_os_input_event_get_time(event);
    self->last_x = self->down_x;
    self->last_y = self->down_y;
}

/** True when @p event has moved past @p slop pixels from where the press went down. */
static lh_bool_t
lh_os_input_recognizer_pointer_is_past_slop(lh_os_input_recognizer_pointer_cptr self,
                                            lh_os_input_event_cptr event, lh_u16_t slop)
{
    return lh_os_input_recognizer_is_beyond(self->down_x, self->down_y,
                                            lh_os_input_event_get_x(event),
                                            lh_os_input_event_get_y(event), slop);
}

/** The distance from the last drag position to @p event, into @p dx and @p dy, and @p event
   becomes the last drag position. */
static lh_void
lh_os_input_recognizer_pointer_advance(lh_os_input_recognizer_pointer_ptr self,
                                       lh_os_input_event_cptr event, lh_s32_t *dx, lh_s32_t *dy)
{
    lh_ptr_deref(dx) = lh_os_input_event_get_x(event) - self->last_x;
    lh_ptr_deref(dy) = lh_os_input_event_get_y(event) - self->last_y;
    self->last_x = lh_os_input_event_get_x(event);
    self->last_y = lh_os_input_event_get_y(event);
}

/** The click number the up @p event makes: one more than the last click when it was of the
   same button, released no more than @p window_ms before this press went down and within
   @p slop pixels; `1` otherwise. */
static lh_u8_t
lh_os_input_recognizer_pointer_count_click(lh_os_input_recognizer_pointer_cptr self,
                                           lh_os_input_event_cptr event, lh_u16_t window_ms,
                                           lh_u16_t slop)
{
    lh_return_if(self->click_count == 0U || self->click_button != self->button,
                 lh_type_cast(lh_u8_t, 1U));
    lh_return_if(lh_tick_elapsed(self->click_time, self->down_time) > window_ms,
                 lh_type_cast(lh_u8_t, 1U));
    lh_return_if(lh_os_input_recognizer_is_beyond(self->click_x, self->click_y,
                                                  lh_os_input_event_get_x(event),
                                                  lh_os_input_event_get_y(event), slop),
                 lh_type_cast(lh_u8_t, 1U));
    return self->click_count == 255U ? self->click_count
                                     : lh_type_cast(lh_u8_t, self->click_count + 1U);
}

/** Remember the click @p event ended as number @p count, for the next one to continue. */
static lh_void
lh_os_input_recognizer_pointer_remember_click(lh_os_input_recognizer_pointer_ptr self,
                                              lh_os_input_event_cptr event, lh_u8_t count)
{
    self->click_button = self->button;
    self->click_x = lh_os_input_event_get_x(event);
    self->click_y = lh_os_input_event_get_y(event);
    self->click_time = lh_os_input_event_get_time(event);
    self->click_count = count;
}

/** A gesture event of @p kind for pointer @p pointer at (@p x, @p y) and @p time, from the
   press @p self holds, into @p out. */
static lh_void
lh_os_input_recognizer_pointer_make(lh_os_input_recognizer_pointer_cptr self, lh_u8_t pointer,
                                    lh_os_input_kind_t kind, lh_os_input_coord_t x,
                                    lh_os_input_coord_t y, lh_tick_t time, lh_os_input_event_t *out)
{
    lh_os_input_event_init_pointer(out, kind, self->device_kind, pointer, self->button, x, y, time);
    lh_os_input_event_set_mods(out, self->mods);
    lh_os_input_event_set_flags(
        out, lh_type_cast(lh_os_input_flags_t,
                          lh_os_input_event_get_flags(out) | LH_OS_INPUT_FLAG_SYNTHESIZED));
}

/** The gesture event of @p kind that @p event completes, into @p out: @p event's position,
   time, buttons and mods, @p self's device and button. */
static lh_void
lh_os_input_recognizer_pointer_make_from(lh_os_input_recognizer_pointer_cptr self,
                                         lh_os_input_event_cptr event, lh_os_input_kind_t kind,
                                         lh_os_input_event_t *out)
{
    lh_os_input_recognizer_pointer_make(
        self, lh_os_input_event_get_pointer(event), kind, lh_os_input_event_get_x(event),
        lh_os_input_event_get_y(event), lh_os_input_event_get_time(event), out);
    lh_os_input_event_set_buttons(out, lh_os_input_event_get_buttons(event));
    lh_os_input_event_set_mods(out, lh_os_input_event_get_mods(event));
}

/** The gesture event of @p kind at @p time where the press @p self holds last was, into
   @p out: no event of the device's own to take it from (a timeout, a cancel). */
static lh_void
lh_os_input_recognizer_pointer_make_at_last(lh_os_input_recognizer_pointer_cptr self,
                                            lh_u8_t pointer, lh_os_input_kind_t kind,
                                            lh_tick_t time, lh_os_input_event_t *out)
{
    lh_os_input_recognizer_pointer_make(self, pointer, kind, self->last_x, self->last_y, time, out);
    lh_os_input_event_set_buttons(out, lh_os_input_button_to_mask(self->button));
}

/* -- Sending --------------------------------------------------------------- */

/** @p event to @p sink with @p flags added. */
static lh_void
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
        lh_os_input_recognizer_config_init_default(lh_addr_of(self->config));
    }
    else
    {
        self->config = lh_ptr_deref(config);
    }
    for (i = 0U; i < LH_OS_INPUT_POINTER_MAX; ++i)
    {
        lh_os_input_recognizer_pointer_clear(lh_addr_of(self->pointers[i]));
    }
}

lh_os_input_recognizer_config_cptr
lh_os_input_recognizer_get_config(lh_os_input_recognizer_cptr self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->config);
}

/** The state of @p pointer, or ::lh_null for an id with no gestures. */
static lh_os_input_recognizer_pointer_ptr
lh_os_input_recognizer_find(lh_os_input_recognizer_t *self, lh_u8_t pointer)
{
    lh_return_if(pointer >= LH_OS_INPUT_POINTER_MAX, lh_null);
    return lh_addr_of(self->pointers[pointer]);
}

/** The read-only state of @p pointer, or ::lh_null for an id with no gestures. */
static lh_os_input_recognizer_pointer_cptr
lh_os_input_recognizer_find_const(lh_os_input_recognizer_cptr self, lh_u8_t pointer)
{
    lh_return_if(pointer >= LH_OS_INPUT_POINTER_MAX, lh_null);
    return lh_addr_of(self->pointers[pointer]);
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

/** When the press of @p state turns into a long press, into @p out; false when it never will
   (not pending, or long presses are off). */
static lh_bool_t
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

/** Pointer @p pointer of @p state let go of without finishing its gesture at @p time: a drag
   ends cancelled, and the series of clicks is over. */
static lh_void
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

/** Every pointer still down is taken away at @p time: a synthesized pointer cancel for it,
   then what ::lh_os_input_recognizer_abandon sends. */
static lh_void
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

/** A pointer down: the first button down on an idle pointer starts a press. */
static lh_void
lh_os_input_recognizer_on_down(lh_os_input_recognizer_pointer_ptr state,
                               lh_os_input_event_cptr event)
{
    lh_return_if(lh_os_input_recognizer_pointer_get_phase(state) !=
                     lh_os_input_recognizer_phase_idle ||
                 lh_os_input_event_get_button(event) == lh_os_input_button_none);
    lh_os_input_recognizer_pointer_begin(state, event);
}

/** A pointer move: past the slop a press becomes a drag; a drag reports how far it went. */
static lh_void
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

/** An up of the button that started the press: a click from a press, a drag end from a drag,
   nothing after a long press. */
static lh_void
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

/** What a pointer event does to the gesture of its pointer. */
static lh_void
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

/** A vertical wheel with Shift held, turned sideways when the config says so: true when it
   was, and went to @p sink. */
static lh_bool_t
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
