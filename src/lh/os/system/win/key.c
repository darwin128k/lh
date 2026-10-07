/**
 * @file key.c
 * @brief Implementation of `src/lh/os/system/win/key.h`.
 */

#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/os/system/win/key.h>
#include <lh/os/system/win/user32.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>
#include <lh/wchar.h>

/* Virtual key and portable key, side by side. */
const lh_u32_t lh_os_system_win_key_vks[] = {
    LH_OS_SYSTEM_WIN_VK_TAB,   LH_OS_SYSTEM_WIN_VK_RETURN, LH_OS_SYSTEM_WIN_VK_ESCAPE, LH_OS_SYSTEM_WIN_VK_BACK,
    LH_OS_SYSTEM_WIN_VK_DELETE, LH_OS_SYSTEM_WIN_VK_SPACE, LH_OS_SYSTEM_WIN_VK_LEFT,  LH_OS_SYSTEM_WIN_VK_RIGHT,
    LH_OS_SYSTEM_WIN_VK_UP,    LH_OS_SYSTEM_WIN_VK_DOWN,   LH_OS_SYSTEM_WIN_VK_HOME,   LH_OS_SYSTEM_WIN_VK_END,
    LH_OS_SYSTEM_WIN_VK_PRIOR, LH_OS_SYSTEM_WIN_VK_NEXT,   LH_OS_SYSTEM_WIN_VK_SHIFT,  LH_OS_SYSTEM_WIN_VK_CONTROL,
    LH_OS_SYSTEM_WIN_VK_MENU,
};
const lh_key_t lh_os_system_win_key_keys[] = {
    lh_key_tab,  lh_key_enter, lh_key_escape, lh_key_backspace, lh_key_delete,  lh_key_space,
    lh_key_left, lh_key_right, lh_key_up,     lh_key_down,      lh_key_home,    lh_key_end,
    lh_key_page_up, lh_key_page_down, lh_key_shift, lh_key_control, lh_key_alt,
};

lh_key_t
lh_os_system_win_key_from_vk(lh_u32_t vk)
{
    lh_u32_t i;

    for (i = 0U; i < sizeof(lh_os_system_win_key_vks) / sizeof(lh_os_system_win_key_vks[0]); ++i)
    {
        lh_return_if(lh_os_system_win_key_vks[i] == vk, lh_os_system_win_key_keys[i]);
    }
    return lh_key_other;
}

lh_u32_t
lh_os_system_win_text_from_char(lh_u32_t ch)
{
    const char byte = lh_cast_static(char, ch);
    lh_wchar_t wide = 0;

    lh_return_if(MultiByteToWideChar(LH_OS_SYSTEM_WIN_CP_ACP, 0U, lh_addr_of(byte), 1, lh_addr_of(wide), 1) != 1, 0U);
    return lh_cast_static(lh_u32_t, wide);
}

lh_bool_t
lh_os_system_win_is_text(lh_u32_t code)
{
    return code >= 0x20U && code != 0x7FU ? lh_bool_true : lh_bool_false;
}
