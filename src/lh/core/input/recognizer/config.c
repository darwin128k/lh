/**
 * @file config.c
 * @brief Implementation of `lh/core/input/recognizer/config.h`.
 */

#include <lh/core/assert/runtime.h>
#include <lh/core/input/recognizer/config.h>
#include <lh/core/input/recognizer/config/initializer.h>

lh_void
lh_input_recognizer_config_init_default(lh_input_recognizer_config_t *self)
{
    static const lh_input_recognizer_config_t defaults =
        lh_input_recognizer_config_default_initializer();

    lh_assert_runtime_ref(self);
    lh_ptr_deref(self) = defaults;
}

lh_u16_t
lh_input_recognizer_config_get_slop(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->slop;
}

lh_u16_t
lh_input_recognizer_config_get_double_click_ms(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->double_click_ms;
}

lh_u16_t
lh_input_recognizer_config_get_double_click_slop(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->double_click_slop;
}

lh_u16_t
lh_input_recognizer_config_get_long_press_ms(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->long_press_ms;
}

lh_u16_t
lh_input_recognizer_config_get_repeat_delay_ms(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->repeat_delay_ms;
}

lh_u16_t
lh_input_recognizer_config_get_repeat_rate_ms(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->repeat_rate_ms;
}

lh_u16_t
lh_input_recognizer_config_get_axis_deadzone(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->axis_deadzone;
}

lh_u16_t
lh_input_recognizer_config_get_axis_hysteresis(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->axis_hysteresis;
}

lh_u16_t
lh_input_recognizer_config_get_swipe_px(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->swipe_px;
}

lh_bool_t
lh_input_recognizer_config_is_wheel_shift_horizontal(lh_input_recognizer_config_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->wheel_shift_horizontal;
}
