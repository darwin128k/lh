/**
 * @file pointer.c
 * @brief Implementation of `lh/os/input/recognizer/pointer.h`.
 */
#include <lh/assert.h>

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
    lh_os_input_recognizer_pointer_set_click_count(self, 0U);
}

lh_void
lh_os_input_recognizer_pointer_begin(lh_os_input_recognizer_pointer_ptr self,
                                     lh_os_input_event_cptr event)
{
    lh_os_input_recognizer_pointer_set_phase(self, lh_os_input_recognizer_phase_pending);
    lh_os_input_recognizer_pointer_set_device_kind(self, lh_os_input_event_get_device_kind(event));
    lh_os_input_recognizer_pointer_set_button(self, lh_os_input_event_get_button(event));
    lh_os_input_recognizer_pointer_set_mods(self, lh_os_input_event_get_mods(event));
    lh_os_input_recognizer_pointer_set_down_x(self, lh_os_input_event_get_x(event));
    lh_os_input_recognizer_pointer_set_down_y(self, lh_os_input_event_get_y(event));
    lh_os_input_recognizer_pointer_set_down_time(self, lh_os_input_event_get_time(event));
    lh_os_input_recognizer_pointer_set_last_x(self,
                                              lh_os_input_recognizer_pointer_get_down_x(self));
    lh_os_input_recognizer_pointer_set_last_y(self,
                                              lh_os_input_recognizer_pointer_get_down_y(self));
}

lh_bool_t
lh_os_input_recognizer_pointer_is_past_slop(lh_os_input_recognizer_pointer_cptr self,
                                            lh_os_input_event_cptr event, lh_u16_t slop)
{
    return lh_os_input_recognizer_is_beyond(lh_os_input_recognizer_pointer_get_down_x(self),
                                            lh_os_input_recognizer_pointer_get_down_y(self),
                                            lh_os_input_event_get_x(event),
                                            lh_os_input_event_get_y(event), slop);
}

lh_void
lh_os_input_recognizer_pointer_advance(lh_os_input_recognizer_pointer_ptr self,
                                       lh_os_input_event_cptr event, lh_s32_t *dx, lh_s32_t *dy)
{
    lh_ptr_deref(dx) =
        lh_os_input_event_get_x(event) - lh_os_input_recognizer_pointer_get_last_x(self);
    lh_ptr_deref(dy) =
        lh_os_input_event_get_y(event) - lh_os_input_recognizer_pointer_get_last_y(self);
    lh_os_input_recognizer_pointer_set_last_x(self, lh_os_input_event_get_x(event));
    lh_os_input_recognizer_pointer_set_last_y(self, lh_os_input_event_get_y(event));
}

lh_u8_t
lh_os_input_recognizer_pointer_count_click(lh_os_input_recognizer_pointer_cptr self,
                                           lh_os_input_event_cptr event, lh_u16_t window_ms,
                                           lh_u16_t slop)
{
    lh_return_if(lh_os_input_recognizer_pointer_get_click_count(self) == 0U ||
                     lh_os_input_recognizer_pointer_get_click_button(self) !=
                         lh_os_input_recognizer_pointer_get_button(self),
                 lh_type_cast(lh_u8_t, 1U));
    lh_return_if(lh_tick_elapsed(lh_os_input_recognizer_pointer_get_click_time(self),
                                 lh_os_input_recognizer_pointer_get_down_time(self)) > window_ms,
                 lh_type_cast(lh_u8_t, 1U));
    lh_return_if(lh_os_input_recognizer_is_beyond(lh_os_input_recognizer_pointer_get_click_x(self),
                                                  lh_os_input_recognizer_pointer_get_click_y(self),
                                                  lh_os_input_event_get_x(event),
                                                  lh_os_input_event_get_y(event), slop),
                 lh_type_cast(lh_u8_t, 1U));
    return lh_os_input_recognizer_pointer_get_click_count(self) == 255U
               ? lh_os_input_recognizer_pointer_get_click_count(self)
               : lh_type_cast(lh_u8_t, lh_os_input_recognizer_pointer_get_click_count(self) + 1U);
}

lh_void
lh_os_input_recognizer_pointer_remember_click(lh_os_input_recognizer_pointer_ptr self,
                                              lh_os_input_event_cptr event, lh_u8_t count)
{
    lh_os_input_recognizer_pointer_set_click_button(
        self, lh_os_input_recognizer_pointer_get_button(self));
    lh_os_input_recognizer_pointer_set_click_x(self, lh_os_input_event_get_x(event));
    lh_os_input_recognizer_pointer_set_click_y(self, lh_os_input_event_get_y(event));
    lh_os_input_recognizer_pointer_set_click_time(self, lh_os_input_event_get_time(event));
    lh_os_input_recognizer_pointer_set_click_count(self, count);
}

lh_void
lh_os_input_recognizer_pointer_make(lh_os_input_recognizer_pointer_cptr self, lh_u8_t pointer,
                                    lh_os_input_kind_t kind, lh_os_input_coord_t x,
                                    lh_os_input_coord_t y, lh_tick_t time, lh_os_input_event_t *out)
{
    lh_os_input_event_init_pointer(out, kind, lh_os_input_recognizer_pointer_get_device_kind(self),
                                   pointer, lh_os_input_recognizer_pointer_get_button(self), x, y,
                                   time);
    lh_os_input_event_set_mods(out, lh_os_input_recognizer_pointer_get_mods(self));
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
    lh_os_input_recognizer_pointer_make(self, pointer, kind,
                                        lh_os_input_recognizer_pointer_get_last_x(self),
                                        lh_os_input_recognizer_pointer_get_last_y(self), time, out);
    lh_os_input_event_set_buttons(
        out, lh_os_input_button_to_mask(lh_os_input_recognizer_pointer_get_button(self)));
}

lh_os_input_device_kind_t
lh_os_input_recognizer_pointer_get_device_kind(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->device_kind;
}

lh_void
lh_os_input_recognizer_pointer_set_device_kind(lh_os_input_recognizer_pointer_t *self,
                                               lh_os_input_device_kind_t device_kind)
{
    lh_assert_runtime_ref(self);
    self->device_kind = device_kind;
}

lh_void
lh_os_input_recognizer_pointer_set_button(lh_os_input_recognizer_pointer_t *self,
                                          lh_os_input_button_t button)
{
    lh_assert_runtime_ref(self);
    self->button = button;
}

lh_os_input_mods_t
lh_os_input_recognizer_pointer_get_mods(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mods;
}

lh_void
lh_os_input_recognizer_pointer_set_mods(lh_os_input_recognizer_pointer_t *self,
                                        lh_os_input_mods_t mods)
{
    lh_assert_runtime_ref(self);
    self->mods = mods;
}

lh_os_input_coord_t
lh_os_input_recognizer_pointer_get_down_x(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->down_x;
}

lh_void
lh_os_input_recognizer_pointer_set_down_x(lh_os_input_recognizer_pointer_t *self,
                                          lh_os_input_coord_t down_x)
{
    lh_assert_runtime_ref(self);
    self->down_x = down_x;
}

lh_os_input_coord_t
lh_os_input_recognizer_pointer_get_down_y(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->down_y;
}

lh_void
lh_os_input_recognizer_pointer_set_down_y(lh_os_input_recognizer_pointer_t *self,
                                          lh_os_input_coord_t down_y)
{
    lh_assert_runtime_ref(self);
    self->down_y = down_y;
}

lh_void
lh_os_input_recognizer_pointer_set_down_time(lh_os_input_recognizer_pointer_t *self,
                                             lh_tick_t down_time)
{
    lh_assert_runtime_ref(self);
    self->down_time = down_time;
}

lh_os_input_coord_t
lh_os_input_recognizer_pointer_get_last_x(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_x;
}

lh_void
lh_os_input_recognizer_pointer_set_last_x(lh_os_input_recognizer_pointer_t *self,
                                          lh_os_input_coord_t last_x)
{
    lh_assert_runtime_ref(self);
    self->last_x = last_x;
}

lh_os_input_coord_t
lh_os_input_recognizer_pointer_get_last_y(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_y;
}

lh_void
lh_os_input_recognizer_pointer_set_last_y(lh_os_input_recognizer_pointer_t *self,
                                          lh_os_input_coord_t last_y)
{
    lh_assert_runtime_ref(self);
    self->last_y = last_y;
}

lh_os_input_button_t
lh_os_input_recognizer_pointer_get_click_button(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->click_button;
}

lh_void
lh_os_input_recognizer_pointer_set_click_button(lh_os_input_recognizer_pointer_t *self,
                                                lh_os_input_button_t click_button)
{
    lh_assert_runtime_ref(self);
    self->click_button = click_button;
}

lh_os_input_coord_t
lh_os_input_recognizer_pointer_get_click_x(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->click_x;
}

lh_void
lh_os_input_recognizer_pointer_set_click_x(lh_os_input_recognizer_pointer_t *self,
                                           lh_os_input_coord_t click_x)
{
    lh_assert_runtime_ref(self);
    self->click_x = click_x;
}

lh_os_input_coord_t
lh_os_input_recognizer_pointer_get_click_y(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->click_y;
}

lh_void
lh_os_input_recognizer_pointer_set_click_y(lh_os_input_recognizer_pointer_t *self,
                                           lh_os_input_coord_t click_y)
{
    lh_assert_runtime_ref(self);
    self->click_y = click_y;
}

lh_tick_t
lh_os_input_recognizer_pointer_get_click_time(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->click_time;
}

lh_void
lh_os_input_recognizer_pointer_set_click_time(lh_os_input_recognizer_pointer_t *self,
                                              lh_tick_t click_time)
{
    lh_assert_runtime_ref(self);
    self->click_time = click_time;
}

lh_u8_t
lh_os_input_recognizer_pointer_get_click_count(const lh_os_input_recognizer_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->click_count;
}

lh_void
lh_os_input_recognizer_pointer_set_click_count(lh_os_input_recognizer_pointer_t *self,
                                               lh_u8_t click_count)
{
    lh_assert_runtime_ref(self);
    self->click_count = click_count;
}
