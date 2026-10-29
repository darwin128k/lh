/**
 * @file event.c
 * @brief Implementation of `lh/os/input/event.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/os/input/event.h>

/* -- Building -------------------------------------------------------------- */

lh_void
lh_os_input_event_init(lh_os_input_event_t *self, lh_os_input_kind_t kind,
                       lh_os_input_device_kind_t device_kind, lh_tick_t time)
{
    lh_assert_runtime_ref(self);
    lh_os_input_event_set_kind(self, kind);
    lh_os_input_event_set_device_kind(self, device_kind);
    lh_os_input_event_set_device(self, 0U);
    lh_os_input_event_set_pointer(self, 0U);
    lh_os_input_event_set_button(self, lh_os_input_button_none);
    lh_os_input_event_set_buttons(self, 0U);
    lh_os_input_event_set_x(self, 0);
    lh_os_input_event_set_y(self, 0);
    lh_os_input_event_set_dx(self, 0);
    lh_os_input_event_set_dy(self, 0);
    lh_os_input_event_set_axis(self, lh_os_input_axis_none);
    lh_os_input_event_set_value(self, 0);
    lh_os_input_event_set_key(self, lh_os_key_other);
    lh_os_input_event_set_scancode(self, 0U);
    lh_os_input_event_set_code(self, 0U);
    lh_os_input_event_set_mods(self, LH_OS_INPUT_MODS_NONE);
    lh_os_input_event_set_time(self, time);
    lh_os_input_event_set_count(self, 0U);
    lh_os_input_event_set_flags(self, LH_OS_INPUT_FLAG_NONE);
}

lh_void
lh_os_input_event_init_pointer(lh_os_input_event_t *self, lh_os_input_kind_t kind,
                               lh_os_input_device_kind_t device_kind, lh_u8_t pointer,
                               lh_os_input_button_t button, lh_os_input_coord_t x,
                               lh_os_input_coord_t y, lh_tick_t time)
{
    lh_os_input_event_init(self, kind, device_kind, time);
    lh_os_input_event_set_pointer(self, pointer);
    lh_os_input_event_set_button(self, button);
    lh_os_input_event_set_x(self, x);
    lh_os_input_event_set_y(self, y);
    lh_os_input_event_set_flags(self,
                                pointer == 0U ? LH_OS_INPUT_FLAG_PRIMARY : LH_OS_INPUT_FLAG_NONE);
}

lh_void
lh_os_input_event_init_wheel(lh_os_input_event_t *self, lh_os_input_coord_t x,
                             lh_os_input_coord_t y, lh_s32_t dx, lh_s32_t dy, lh_tick_t time)
{
    lh_os_input_event_init_pointer(self, lh_os_input_kind_wheel, lh_os_input_device_kind_mouse, 0U,
                                   lh_os_input_button_none, x, y, time);
    lh_os_input_event_set_dx(self, dx);
    lh_os_input_event_set_dy(self, dy);
}

lh_void
lh_os_input_event_init_key(lh_os_input_event_t *self, lh_os_key_t key, lh_bool_t down,
                           lh_bool_t repeat, lh_u32_t scancode, lh_tick_t time)
{
    lh_os_input_event_init(self, down ? lh_os_input_kind_key_down : lh_os_input_kind_key_up,
                           lh_os_input_device_kind_keyboard, time);
    lh_os_input_event_set_key(self, key);
    lh_os_input_event_set_scancode(self, scancode);
    lh_os_input_event_set_flags(self, repeat ? LH_OS_INPUT_FLAG_REPEAT : LH_OS_INPUT_FLAG_NONE);
}

lh_void
lh_os_input_event_init_text(lh_os_input_event_t *self, lh_u32_t code, lh_tick_t time)
{
    lh_os_input_event_init(self, lh_os_input_kind_text, lh_os_input_device_kind_keyboard, time);
    lh_os_input_event_set_code(self, code);
}

lh_void
lh_os_input_event_init_controller_button(lh_os_input_event_t *self, lh_u8_t device,
                                         lh_os_input_button_t button, lh_bool_t down,
                                         lh_tick_t time)
{
    lh_os_input_event_init(self,
                           down ? lh_os_input_kind_controller_button_down
                                : lh_os_input_kind_controller_button_up,
                           lh_os_input_device_kind_controller, time);
    lh_os_input_event_set_device(self, device);
    lh_os_input_event_set_button(self, button);
}

lh_void
lh_os_input_event_init_controller_axis(lh_os_input_event_t *self, lh_u8_t device,
                                       lh_os_input_axis_t axis, lh_s16_t value, lh_tick_t time)
{
    lh_os_input_event_init(self, lh_os_input_kind_controller_axis,
                           lh_os_input_device_kind_controller, time);
    lh_os_input_event_set_device(self, device);
    lh_os_input_event_set_axis(self, axis);
    lh_os_input_event_set_value(self, value);
}

lh_void
lh_os_input_event_set_mods(lh_os_input_event_t *self, lh_os_input_mods_t mods)
{
    lh_assert_runtime_ref(self);
    self->mods = mods;
}

lh_void
lh_os_input_event_set_buttons(lh_os_input_event_t *self, lh_u32_t buttons)
{
    lh_assert_runtime_ref(self);
    self->buttons = buttons;
}

lh_void
lh_os_input_event_set_flags(lh_os_input_event_t *self, lh_os_input_flags_t flags)
{
    lh_assert_runtime_ref(self);
    self->flags = flags;
}

lh_void
lh_os_input_event_set_device(lh_os_input_event_t *self, lh_u8_t device)
{
    lh_assert_runtime_ref(self);
    self->device = device;
}

lh_void
lh_os_input_event_set_count(lh_os_input_event_t *self, lh_u8_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_void
lh_os_input_event_set_delta(lh_os_input_event_t *self, lh_s32_t dx, lh_s32_t dy)
{
    lh_assert_runtime_ref(self);
    lh_os_input_event_set_dx(self, dx);
    lh_os_input_event_set_dy(self, dy);
}

/* -- Reading --------------------------------------------------------------- */

lh_os_input_kind_t
lh_os_input_event_get_kind(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_os_input_device_kind_t
lh_os_input_event_get_device_kind(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->device_kind;
}

lh_u8_t
lh_os_input_event_get_device(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->device;
}

lh_u8_t
lh_os_input_event_get_pointer(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->pointer;
}

lh_os_input_button_t
lh_os_input_event_get_button(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->button;
}

lh_u32_t
lh_os_input_event_get_buttons(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->buttons;
}

lh_os_input_coord_t
lh_os_input_event_get_x(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_os_input_coord_t
lh_os_input_event_get_y(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_s32_t
lh_os_input_event_get_dx(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->dx;
}

lh_s32_t
lh_os_input_event_get_dy(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->dy;
}

lh_os_input_axis_t
lh_os_input_event_get_axis(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->axis;
}

lh_s16_t
lh_os_input_event_get_value(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->value;
}

lh_os_key_t
lh_os_input_event_get_key(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->key;
}

lh_u32_t
lh_os_input_event_get_scancode(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->scancode;
}

lh_u32_t
lh_os_input_event_get_code(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_os_input_mods_t
lh_os_input_event_get_mods(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->mods;
}

lh_tick_t
lh_os_input_event_get_time(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->time;
}

lh_u8_t
lh_os_input_event_get_count(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_os_input_flags_t
lh_os_input_event_get_flags(lh_os_input_event_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->flags;
}

lh_bool_t
lh_os_input_event_has_mods(lh_os_input_event_cptr self, lh_os_input_mods_t mods)
{
    return lh_type_cast(lh_bool_t, (lh_os_input_event_get_mods(self) & mods) == mods);
}

lh_bool_t
lh_os_input_event_has_flag(lh_os_input_event_cptr self, lh_os_input_flags_t flag)
{
    return lh_type_cast(lh_bool_t, (lh_os_input_event_get_flags(self) & flag) != 0U);
}

lh_bool_t
lh_os_input_event_is_pointer(lh_os_input_event_cptr self)
{
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(self);

    return lh_type_cast(lh_bool_t,
                        (kind >= lh_os_input_kind_pointer_down && kind <= lh_os_input_kind_wheel) ||
                            (kind >= lh_os_input_kind_click && kind <= lh_os_input_kind_drag_end));
}

lh_bool_t
lh_os_input_event_is_keyboard(lh_os_input_event_cptr self)
{
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(self);

    return lh_type_cast(lh_bool_t,
                        kind >= lh_os_input_kind_key_down && kind <= lh_os_input_kind_text);
}

lh_bool_t
lh_os_input_event_is_controller(lh_os_input_event_cptr self)
{
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(self);

    return lh_type_cast(lh_bool_t, kind >= lh_os_input_kind_controller_button_down &&
                                       kind <= lh_os_input_kind_controller_axis);
}

lh_bool_t
lh_os_input_event_is_repeat(lh_os_input_event_cptr self)
{
    return lh_os_input_event_has_flag(self, LH_OS_INPUT_FLAG_REPEAT);
}

lh_bool_t
lh_os_input_event_is_synthesized(lh_os_input_event_cptr self)
{
    return lh_os_input_event_has_flag(self, LH_OS_INPUT_FLAG_SYNTHESIZED);
}

lh_bool_t
lh_os_input_event_is_primary(lh_os_input_event_cptr self)
{
    return lh_os_input_event_has_flag(self, LH_OS_INPUT_FLAG_PRIMARY);
}

lh_bool_t
lh_os_input_event_is_cancelled(lh_os_input_event_cptr self)
{
    return lh_os_input_event_has_flag(self, LH_OS_INPUT_FLAG_CANCELLED);
}

lh_bool_t
lh_os_input_event_is_key_down(lh_os_input_event_cptr self, lh_os_key_t key)
{
    return lh_type_cast(lh_bool_t, lh_os_input_event_get_kind(self) == lh_os_input_kind_key_down &&
                                       lh_os_input_event_get_key(self) == key);
}

lh_void
lh_os_input_event_set_kind(lh_os_input_event_t *self, lh_os_input_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_void
lh_os_input_event_set_device_kind(lh_os_input_event_t *self, lh_os_input_device_kind_t device_kind)
{
    lh_assert_runtime_ref(self);
    self->device_kind = device_kind;
}

lh_void
lh_os_input_event_set_pointer(lh_os_input_event_t *self, lh_u8_t pointer)
{
    lh_assert_runtime_ref(self);
    self->pointer = pointer;
}

lh_void
lh_os_input_event_set_button(lh_os_input_event_t *self, lh_os_input_button_t button)
{
    lh_assert_runtime_ref(self);
    self->button = button;
}

lh_void
lh_os_input_event_set_x(lh_os_input_event_t *self, lh_os_input_coord_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_os_input_event_set_y(lh_os_input_event_t *self, lh_os_input_coord_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

lh_void
lh_os_input_event_set_dx(lh_os_input_event_t *self, lh_s32_t dx)
{
    lh_assert_runtime_ref(self);
    self->dx = dx;
}

lh_void
lh_os_input_event_set_dy(lh_os_input_event_t *self, lh_s32_t dy)
{
    lh_assert_runtime_ref(self);
    self->dy = dy;
}

lh_void
lh_os_input_event_set_axis(lh_os_input_event_t *self, lh_os_input_axis_t axis)
{
    lh_assert_runtime_ref(self);
    self->axis = axis;
}

lh_void
lh_os_input_event_set_value(lh_os_input_event_t *self, lh_s16_t value)
{
    lh_assert_runtime_ref(self);
    self->value = value;
}

lh_void
lh_os_input_event_set_key(lh_os_input_event_t *self, lh_os_key_t key)
{
    lh_assert_runtime_ref(self);
    self->key = key;
}

lh_void
lh_os_input_event_set_scancode(lh_os_input_event_t *self, lh_u32_t scancode)
{
    lh_assert_runtime_ref(self);
    self->scancode = scancode;
}

lh_void
lh_os_input_event_set_code(lh_os_input_event_t *self, lh_u32_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

lh_void
lh_os_input_event_set_time(lh_os_input_event_t *self, lh_tick_t time)
{
    lh_assert_runtime_ref(self);
    self->time = time;
}
