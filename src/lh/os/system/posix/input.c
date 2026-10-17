/**
 * @file input.c
 * @brief Implementation of `src/lh/os/system/posix/input.h`.
 */

#include <lh/os/system/posix/input.h>
#include <lh/os/system/posix/x11.h>

lh_os_input_button_t
lh_os_system_posix_input_button_of(lh_u32_t xbutton)
{
    switch (xbutton)
    {
    case LH_OS_SYSTEM_POSIX_X_BUTTON_LEFT:
        return lh_os_input_button_primary;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_MIDDLE:
        return lh_os_input_button_middle;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_RIGHT:
        return lh_os_input_button_secondary;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_BACK:
        return lh_os_input_button_back;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_FORWARD:
        return lh_os_input_button_forward;
    default:
        return lh_os_input_button_none;
    }
}

lh_s32_t
lh_os_system_posix_input_wheel_increment_of(lh_u32_t xbutton)
{
    switch (xbutton)
    {
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_UP:
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_DOWN:
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_LEFT:
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_RIGHT:
        return LH_OS_INPUT_WHEEL_NOTCH;
    default:
        return 0;
    }
}

lh_bool_t
lh_os_system_posix_input_wheel_of(lh_u32_t xbutton, lh_s32_t *dx, lh_s32_t *dy)
{
    const lh_s32_t increment = lh_os_system_posix_input_wheel_increment_of(xbutton);
    lh_s32_t x = 0;
    lh_s32_t y = 0;

    switch (xbutton)
    {
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_UP:
        y = increment;
        break;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_DOWN:
        y = lh_math_neg(increment);
        break;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_LEFT:
        x = increment;
        break;
    case LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_RIGHT:
        x = lh_math_neg(increment);
        break;
    default:
        break;
    }
    lh_ptr_deref(dx) = x;
    lh_ptr_deref(dy) = y;
    return lh_type_cast(lh_bool_t, x != 0 || y != 0);
}

/** @p mod when @p state has @p bit, else `0`. */
static lh_os_input_mods_t
lh_os_system_posix_input_mod_if(lh_u32_t state, lh_u32_t bit, lh_os_input_mods_t mod)
{
    return (state & bit) != 0U ? mod : lh_type_cast(lh_os_input_mods_t, 0U);
}

lh_os_input_mods_t
lh_os_system_posix_input_mods_of(lh_u32_t state)
{
    return lh_type_cast(lh_os_input_mods_t,
                        lh_os_system_posix_input_mod_if(state, LH_OS_SYSTEM_POSIX_X_SHIFT_MASK,
                                                        LH_OS_INPUT_MODS_SHIFT) |
                            lh_os_system_posix_input_mod_if(state,
                                                            LH_OS_SYSTEM_POSIX_X_CONTROL_MASK,
                                                            LH_OS_INPUT_MODS_CONTROL) |
                            lh_os_system_posix_input_mod_if(state, LH_OS_SYSTEM_POSIX_X_MOD1_MASK,
                                                            LH_OS_INPUT_MODS_ALT) |
                            lh_os_system_posix_input_mod_if(state, LH_OS_SYSTEM_POSIX_X_MOD4_MASK,
                                                            LH_OS_INPUT_MODS_META) |
                            lh_os_system_posix_input_mod_if(state, LH_OS_SYSTEM_POSIX_X_LOCK_MASK,
                                                            LH_OS_INPUT_MODS_CAPS_LOCK) |
                            lh_os_system_posix_input_mod_if(state, LH_OS_SYSTEM_POSIX_X_MOD2_MASK,
                                                            LH_OS_INPUT_MODS_NUM_LOCK));
}

/** @p button's bit when @p state has @p bit, else `0`. */
static lh_u32_t
lh_os_system_posix_input_bit_if(lh_u32_t state, lh_u32_t bit, lh_os_input_button_t button)
{
    return (state & bit) != 0U ? lh_os_input_button_to_mask(button) : 0U;
}

lh_u32_t
lh_os_system_posix_input_buttons_of(lh_u32_t state)
{
    return lh_os_system_posix_input_bit_if(state, LH_OS_SYSTEM_POSIX_X_BUTTON1_MASK,
                                           lh_os_input_button_primary) |
           lh_os_system_posix_input_bit_if(state, LH_OS_SYSTEM_POSIX_X_BUTTON2_MASK,
                                           lh_os_input_button_middle) |
           lh_os_system_posix_input_bit_if(state, LH_OS_SYSTEM_POSIX_X_BUTTON3_MASK,
                                           lh_os_input_button_secondary);
}
