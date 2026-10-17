/**
 * @file config.c
 * @brief Implementation of `lh/os/input/recognizer/config.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/os/input/recognizer/config.h>
#include <lh/os/input/recognizer/config/initializer.h>

lh_void
lh_os_input_recognizer_config_init_default(lh_os_input_recognizer_config_t *self)
{
    static const lh_os_input_recognizer_config_t defaults =
        lh_os_input_recognizer_config_default_initializer();

    lh_assert_runtime_ref(self);
    lh_ptr_deref(self) = defaults;
}

lh_u16_t
lh_os_input_recognizer_config_get_slop(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->slop;
}

lh_u16_t
lh_os_input_recognizer_config_get_double_click_ms(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->double_click_ms;
}

lh_u16_t
lh_os_input_recognizer_config_get_double_click_slop(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->double_click_slop;
}

lh_u16_t
lh_os_input_recognizer_config_get_long_press_ms(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->long_press_ms;
}

lh_u16_t
lh_os_input_recognizer_config_get_repeat_delay_ms(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->repeat_delay_ms;
}

lh_u16_t
lh_os_input_recognizer_config_get_repeat_rate_ms(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->repeat_rate_ms;
}

lh_u16_t
lh_os_input_recognizer_config_get_axis_deadzone(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->axis_deadzone;
}

lh_u16_t
lh_os_input_recognizer_config_get_axis_hysteresis(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->axis_hysteresis;
}

lh_bool_t
lh_os_input_recognizer_config_is_wheel_shift_horizontal(lh_os_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->wheel_shift_horizontal;
}
