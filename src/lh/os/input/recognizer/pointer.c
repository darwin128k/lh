/**
 * @file pointer.c
 * @brief Implementation of `lh/os/input/recognizer/pointer.h`.
 */

#include <lh/os/input/recognizer/pointer.h>
#include <lh/math.h>
#include <lh/os/input/recognizer/pointer/initializer.h>
#include <lh/util/return.h>

lh_bool_t
lh_os_input_recognizer_is_beyond(lh_os_input_coord_t from_x, lh_os_input_coord_t from_y,
                                 lh_os_input_coord_t x, lh_os_input_coord_t y, lh_u16_t slop)
{
    const lh_os_input_coord_t limit = lh_os_input_coord_from_int(lh_type_cast(lh_s32_t, slop));

    return lh_type_cast(lh_bool_t,
                        lh_math_abs(x - from_x) > limit || lh_math_abs(y - from_y) > limit);
}

lh_void
lh_os_input_recognizer_pointer_clear(lh_os_input_recognizer_pointer_ptr self)
{
    static const lh_os_input_recognizer_pointer_t idle =
        lh_os_input_recognizer_pointer_idle_initializer();

    lh_ptr_deref(self) = idle;
}

lh_os_input_recognizer_phase_t
lh_os_input_recognizer_pointer_get_phase(lh_os_input_recognizer_pointer_cptr self)
{
    return self->phase;
}

lh_void
lh_os_input_recognizer_pointer_set_phase(lh_os_input_recognizer_pointer_ptr self,
                                         lh_os_input_recognizer_phase_t phase)
{
    self->phase = phase;
}

lh_os_input_button_t
lh_os_input_recognizer_pointer_get_button(lh_os_input_recognizer_pointer_cptr self)
{
    return self->button;
}

lh_tick_t
lh_os_input_recognizer_pointer_get_down_time(lh_os_input_recognizer_pointer_cptr self)
{
    return self->down_time;
}

lh_void
lh_os_input_recognizer_pointer_forget_click(lh_os_input_recognizer_pointer_ptr self)
{
    self->click_count = 0U;
}

lh_void
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

lh_bool_t
lh_os_input_recognizer_pointer_is_past_slop(lh_os_input_recognizer_pointer_cptr self,
                                            lh_os_input_event_cptr event, lh_u16_t slop)
{
    return lh_os_input_recognizer_is_beyond(self->down_x, self->down_y,
                                            lh_os_input_event_get_x(event),
                                            lh_os_input_event_get_y(event), slop);
}

lh_void
lh_os_input_recognizer_pointer_advance(lh_os_input_recognizer_pointer_ptr self,
                                       lh_os_input_event_cptr event, lh_s32_t *dx, lh_s32_t *dy)
{
    lh_ptr_deref(dx) = lh_os_input_event_get_x(event) - self->last_x;
    lh_ptr_deref(dy) = lh_os_input_event_get_y(event) - self->last_y;
    self->last_x = lh_os_input_event_get_x(event);
    self->last_y = lh_os_input_event_get_y(event);
}

lh_u8_t
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

lh_void
lh_os_input_recognizer_pointer_remember_click(lh_os_input_recognizer_pointer_ptr self,
                                              lh_os_input_event_cptr event, lh_u8_t count)
{
    self->click_button = self->button;
    self->click_x = lh_os_input_event_get_x(event);
    self->click_y = lh_os_input_event_get_y(event);
    self->click_time = lh_os_input_event_get_time(event);
    self->click_count = count;
}

lh_void
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

lh_void
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

lh_void
lh_os_input_recognizer_pointer_make_at_last(lh_os_input_recognizer_pointer_cptr self,
                                            lh_u8_t pointer, lh_os_input_kind_t kind,
                                            lh_tick_t time, lh_os_input_event_t *out)
{
    lh_os_input_recognizer_pointer_make(self, pointer, kind, self->last_x, self->last_y, time, out);
    lh_os_input_event_set_buttons(out, lh_os_input_button_to_mask(self->button));
}
