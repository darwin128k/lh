/**
 * @file input.c
 * @brief Implementation of `src/lh/os/system/win/input.h`.
 */

#include <lh/os/input/wheel.h>
#include <lh/os/system/win/input.h>
#include <lh/util/return.h>

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

lh_u32_t
lh_os_system_win_input_buttons_of(lh_os_system_win_wparam_t wparam)
{
    const lh_u32_t mk = LH_OS_SYSTEM_WIN_LOWORD(wparam);

    return lh_bit_or(
        lh_os_input_button_mask_if(!lh_math_is_zero(lh_bit_and(mk, LH_OS_SYSTEM_WIN_MK_LBUTTON)),
                                   lh_os_input_button_primary),
        lh_bit_or(
            lh_os_input_button_mask_if(
                !lh_math_is_zero(lh_bit_and(mk, LH_OS_SYSTEM_WIN_MK_RBUTTON)),
                lh_os_input_button_secondary),
            lh_bit_or(
                lh_os_input_button_mask_if(
                    !lh_math_is_zero(lh_bit_and(mk, LH_OS_SYSTEM_WIN_MK_MBUTTON)),
                    lh_os_input_button_middle),
                lh_bit_or(lh_os_input_button_mask_if(
                              !lh_math_is_zero(lh_bit_and(mk, LH_OS_SYSTEM_WIN_MK_XBUTTON1)),
                              lh_os_input_button_back),
                          lh_os_input_button_mask_if(
                              !lh_math_is_zero(lh_bit_and(mk, LH_OS_SYSTEM_WIN_MK_XBUTTON2)),
                              lh_os_input_button_forward)))));
}

lh_bool_t
lh_os_system_win_input_is_repeat(lh_os_system_win_lparam_t lparam)
{
    return lh_type_cast(
        lh_bool_t,
        !lh_math_is_zero(lh_bit_and(lh_type_cast(lh_usize_t, lparam),
                                    lh_bit_shl(lh_type_cast(lh_usize_t, 1U), 30))));
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

lh_u32_t
lh_os_system_win_input_wheel_lines_of(lh_os_system_win_bool_t got, lh_os_system_win_uint_t lines)
{
    if (got == LH_OS_SYSTEM_WIN_FALSE)
    {
        return lh_type_cast(lh_u32_t, LH_OS_INPUT_WHEEL_LINES_DEFAULT);
    }
    if (lines == LH_OS_SYSTEM_WIN_WHEEL_PAGESCROLL)
    {
        return lh_type_cast(lh_u32_t, LH_OS_INPUT_WHEEL_LINES_PAGE);
    }
    return lh_type_cast(lh_u32_t, lines);
}

lh_u32_t
lh_os_system_win_input_get_wheel_lines(lh_void)
{
    lh_os_system_win_uint_t lines = 0U;
    const lh_os_system_win_bool_t got =
        SystemParametersInfoA(LH_OS_SYSTEM_WIN_SPI_GETWHEELSCROLLLINES, 0U, lh_addr_of(lines), 0U);

    return lh_os_system_win_input_wheel_lines_of(got, lines);
}

lh_os_input_device_kind_t
lh_os_system_win_input_device_kind_of_pointer_type(lh_os_system_win_pointer_input_type_t type)
{
    switch (type)
    {
    case LH_OS_SYSTEM_WIN_PT_TOUCH:
        return lh_os_input_device_kind_touch;
    case LH_OS_SYSTEM_WIN_PT_PEN:
        return lh_os_input_device_kind_pen;
    case LH_OS_SYSTEM_WIN_PT_MOUSE:
    case LH_OS_SYSTEM_WIN_PT_TOUCHPAD:
        return lh_os_input_device_kind_mouse;
    default:
        return lh_os_input_device_kind_none;
    }
}

lh_os_system_win_pointer_flags_t
lh_os_system_win_input_pointer_flags_of(lh_os_system_win_wparam_t wparam)
{
    return lh_type_cast(lh_os_system_win_pointer_flags_t, LH_OS_SYSTEM_WIN_HIWORD(wparam));
}

lh_os_input_kind_t
lh_os_system_win_input_pointer_kind_of(lh_u32_t msg, lh_os_system_win_pointer_flags_t flags)
{
    const lh_bool_t cancelled = lh_type_cast(
        lh_bool_t, !lh_math_is_zero(lh_bit_and(flags, LH_OS_SYSTEM_WIN_POINTER_FLAG_CANCELED)));

    switch (msg)
    {
    case LH_OS_SYSTEM_WIN_WM_POINTERDOWN:
        return cancelled ? lh_os_input_kind_pointer_cancel : lh_os_input_kind_pointer_down;
    case LH_OS_SYSTEM_WIN_WM_POINTERUPDATE:
        return cancelled ? lh_os_input_kind_pointer_cancel : lh_os_input_kind_pointer_move;
    case LH_OS_SYSTEM_WIN_WM_POINTERUP:
        return cancelled ? lh_os_input_kind_pointer_cancel : lh_os_input_kind_pointer_up;
    case LH_OS_SYSTEM_WIN_WM_POINTERCAPTURECHANGED:
        return lh_os_input_kind_pointer_cancel;
    default:
        return lh_os_input_kind_none;
    }
}

lh_bool_t
lh_os_system_win_input_is_promoted_mouse(lh_os_system_win_lparam_t extra)
{
    const lh_u32_t signature = lh_type_cast(lh_u32_t, lh_type_cast(lh_usize_t, extra));

    return lh_type_cast(
        lh_bool_t, lh_math_eq(lh_bit_and(signature, LH_OS_SYSTEM_WIN_MI_WP_SIGNATURE_MASK),
                              LH_OS_SYSTEM_WIN_MI_WP_SIGNATURE));
}

lh_os_input_device_kind_t
lh_os_system_win_input_device_kind_of_mouse(lh_os_system_win_lparam_t extra)
{
    const lh_u32_t signature = lh_type_cast(lh_u32_t, lh_type_cast(lh_usize_t, extra));

    lh_return_if(!lh_os_system_win_input_is_promoted_mouse(extra), lh_os_input_device_kind_mouse);
    return !lh_math_is_zero(lh_bit_and(signature, LH_OS_SYSTEM_WIN_MI_WP_TOUCH))
               ? lh_os_input_device_kind_touch
               : lh_os_input_device_kind_pen;
}

lh_bool_t
lh_os_system_win_input_vk_held(lh_int_t vk, lh_bool_t toggled)
{
    const lh_u16_t state = lh_type_cast(lh_u16_t, GetKeyState(vk));
    const lh_u16_t bit = toggled ? 0x0001U : 0x8000U;

    return lh_type_cast(lh_bool_t, !lh_math_is_zero(lh_bit_and(state, bit)));
}

lh_os_input_mods_t
lh_os_system_win_input_get_mods(lh_void)
{
    return lh_type_cast(
        lh_os_input_mods_t,
        lh_bit_or(
            lh_os_input_mods_if(
                lh_os_system_win_input_vk_held(LH_OS_SYSTEM_WIN_VK_SHIFT, lh_bool_false),
                LH_OS_INPUT_MODS_SHIFT),
            lh_bit_or(
                lh_os_input_mods_if(
                    lh_os_system_win_input_vk_held(LH_OS_SYSTEM_WIN_VK_CONTROL, lh_bool_false),
                    LH_OS_INPUT_MODS_CONTROL),
                lh_bit_or(
                    lh_os_input_mods_if(
                        lh_os_system_win_input_vk_held(LH_OS_SYSTEM_WIN_VK_MENU, lh_bool_false),
                        LH_OS_INPUT_MODS_ALT),
                    lh_bit_or(
                        lh_os_input_mods_if(
                            lh_os_system_win_input_vk_held(LH_OS_SYSTEM_WIN_VK_LWIN, lh_bool_false),
                            LH_OS_INPUT_MODS_META),
                        lh_bit_or(
                            lh_os_input_mods_if(
                                lh_os_system_win_input_vk_held(LH_OS_SYSTEM_WIN_VK_RWIN,
                                                               lh_bool_false),
                                LH_OS_INPUT_MODS_META),
                            lh_bit_or(
                                lh_os_input_mods_if(
                                    lh_os_system_win_input_vk_held(LH_OS_SYSTEM_WIN_VK_CAPITAL,
                                                                   lh_bool_true),
                                    LH_OS_INPUT_MODS_CAPS_LOCK),
                                lh_os_input_mods_if(
                                    lh_os_system_win_input_vk_held(LH_OS_SYSTEM_WIN_VK_NUMLOCK,
                                                                   lh_bool_true),
                                    LH_OS_INPUT_MODS_NUM_LOCK))))))));
}
