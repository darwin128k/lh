/**
 * @file input.c
 * @brief Implementation of `src/lh/os/system/win/input.h`.
 */

#include <lh/os/system/win/input.h>

lh_os_input_button_t
lh_os_system_win_input_button_of(lh_u32_t msg, lh_os_system_win_wparam_t wparam)
{
    switch (msg)
    {
    case LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN:
    case LH_OS_SYSTEM_WIN_WM_LBUTTONUP:
        return lh_os_input_button_primary;
    case LH_OS_SYSTEM_WIN_WM_RBUTTONDOWN:
    case LH_OS_SYSTEM_WIN_WM_RBUTTONUP:
        return lh_os_input_button_secondary;
    case LH_OS_SYSTEM_WIN_WM_MBUTTONDOWN:
    case LH_OS_SYSTEM_WIN_WM_MBUTTONUP:
        return lh_os_input_button_middle;
    case LH_OS_SYSTEM_WIN_WM_XBUTTONDOWN:
    case LH_OS_SYSTEM_WIN_WM_XBUTTONUP:
        return LH_OS_SYSTEM_WIN_HIWORD(wparam) == LH_OS_SYSTEM_WIN_XBUTTON2
                   ? lh_os_input_button_forward
                   : lh_os_input_button_back;
    default:
        return lh_os_input_button_none;
    }
}

lh_bool_t
lh_os_system_win_input_is_button_down(lh_u32_t msg)
{
    return lh_type_cast(lh_bool_t, msg == LH_OS_SYSTEM_WIN_WM_LBUTTONDOWN ||
                                       msg == LH_OS_SYSTEM_WIN_WM_RBUTTONDOWN ||
                                       msg == LH_OS_SYSTEM_WIN_WM_MBUTTONDOWN ||
                                       msg == LH_OS_SYSTEM_WIN_WM_XBUTTONDOWN);
}

/** @p button's bit when @p wparam has @p mk set, else `0`. */
static lh_u32_t
lh_os_system_win_input_bit_if(lh_os_system_win_wparam_t wparam, lh_u32_t mk,
                              lh_os_input_button_t button)
{
    return (LH_OS_SYSTEM_WIN_LOWORD(wparam) & mk) != 0U ? lh_os_input_button_to_mask(button) : 0U;
}

lh_u32_t
lh_os_system_win_input_buttons_of(lh_os_system_win_wparam_t wparam)
{
    return lh_os_system_win_input_bit_if(wparam, LH_OS_SYSTEM_WIN_MK_LBUTTON,
                                         lh_os_input_button_primary) |
           lh_os_system_win_input_bit_if(wparam, LH_OS_SYSTEM_WIN_MK_RBUTTON,
                                         lh_os_input_button_secondary) |
           lh_os_system_win_input_bit_if(wparam, LH_OS_SYSTEM_WIN_MK_MBUTTON,
                                         lh_os_input_button_middle) |
           lh_os_system_win_input_bit_if(wparam, LH_OS_SYSTEM_WIN_MK_XBUTTON1,
                                         lh_os_input_button_back) |
           lh_os_system_win_input_bit_if(wparam, LH_OS_SYSTEM_WIN_MK_XBUTTON2,
                                         lh_os_input_button_forward);
}

lh_bool_t
lh_os_system_win_input_is_repeat(lh_os_system_win_lparam_t lparam)
{
    return lh_type_cast(
        lh_bool_t, (lh_type_cast(lh_usize_t, lparam) & (lh_type_cast(lh_usize_t, 1U) << 30)) != 0U);
}

lh_void
lh_os_system_win_input_wheel_of(lh_u32_t msg, lh_os_system_win_wparam_t wparam, lh_s32_t *dx,
                                lh_s32_t *dy)
{
    const lh_s32_t delta = lh_type_cast(lh_s32_t, LH_OS_SYSTEM_WIN_GET_WHEEL_DELTA_WPARAM(wparam));

    /* Windows counts the sideways wheel positive to the right; an event counts left. */
    lh_ptr_deref(dx) = msg == LH_OS_SYSTEM_WIN_WM_MOUSEHWHEEL ? lh_math_neg(delta) : 0;
    lh_ptr_deref(dy) = msg == LH_OS_SYSTEM_WIN_WM_MOUSEHWHEEL ? 0 : delta;
}

/** @p mod when the virtual key @p vk is down (or, with @p toggled, toggled on), else `0`. */
static lh_os_input_mods_t
lh_os_system_win_input_mod_if(lh_int_t vk, lh_bool_t toggled, lh_os_input_mods_t mod)
{
    const lh_u16_t state = lh_type_cast(lh_u16_t, GetKeyState(vk));
    const lh_u16_t bit = toggled ? 0x0001U : 0x8000U;

    return (state & bit) != 0U ? mod : lh_type_cast(lh_os_input_mods_t, 0U);
}

lh_os_input_mods_t
lh_os_system_win_input_get_mods(lh_void)
{
    return lh_type_cast(lh_os_input_mods_t,
                        lh_os_system_win_input_mod_if(LH_OS_SYSTEM_WIN_VK_SHIFT, lh_bool_false,
                                                      LH_OS_INPUT_MODS_SHIFT) |
                            lh_os_system_win_input_mod_if(LH_OS_SYSTEM_WIN_VK_CONTROL,
                                                          lh_bool_false, LH_OS_INPUT_MODS_CONTROL) |
                            lh_os_system_win_input_mod_if(LH_OS_SYSTEM_WIN_VK_MENU, lh_bool_false,
                                                          LH_OS_INPUT_MODS_ALT) |
                            lh_os_system_win_input_mod_if(LH_OS_SYSTEM_WIN_VK_LWIN, lh_bool_false,
                                                          LH_OS_INPUT_MODS_META) |
                            lh_os_system_win_input_mod_if(LH_OS_SYSTEM_WIN_VK_RWIN, lh_bool_false,
                                                          LH_OS_INPUT_MODS_META) |
                            lh_os_system_win_input_mod_if(LH_OS_SYSTEM_WIN_VK_CAPITAL, lh_bool_true,
                                                          LH_OS_INPUT_MODS_CAPS_LOCK) |
                            lh_os_system_win_input_mod_if(LH_OS_SYSTEM_WIN_VK_NUMLOCK, lh_bool_true,
                                                          LH_OS_INPUT_MODS_NUM_LOCK));
}
