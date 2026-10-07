/**
 * @file key.h
 * @brief Backend-private: Win32 keyboard messages to ::lh_key_t and code points.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_KEY_H
#define LH_SRC_OS_SYSTEM_WIN_KEY_H

#include <lh/bool.h>
#include <lh/key.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/system/win/types.h>

/** @brief Virtual-key codes ::lh_os_system_win_key_from_vk knows, in order. */
extern const lh_u32_t lh_os_system_win_key_vks[];

/** @brief The portable key of each entry of ::lh_os_system_win_key_vks. */
extern const lh_key_t lh_os_system_win_key_keys[];

/**
 * @brief The portable key for the virtual-key code @p vk (`WM_KEYDOWN`
 *        `wParam`); ::lh_key_other for any key not in ::lh_key_t.
 */
lh_key_t
lh_os_system_win_key_from_vk(lh_u32_t vk);

/**
 * @brief The code point of the `WM_CHAR` byte @p ch (ANSI code page of an
 *        ANSI window), or `0` when it does not convert.
 */
lh_u32_t
lh_os_system_win_text_from_char(lh_u32_t ch);

/**
 * @brief True when @p code is text to hand on, not a control character
 *        (those come as keys).
 */
lh_bool_t
lh_os_system_win_is_text(lh_u32_t code);

#endif /* LH_SRC_OS_SYSTEM_WIN_KEY_H */
