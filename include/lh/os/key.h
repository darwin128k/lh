/**
 * @file key.h
 * @brief Portable key codes: ::lh_os_key_t.
 *
 * The keys a UI acts on without reading text: editing, moving, focus. The OS
 * layer reports these (Win32 virtual keys are mapped once, in
 * `src/lh/os/system/win/key.c`); everything else is ::lh_os_key_other, and what
 * it types arrives separately as text (a code point).
 */

#ifndef LH_OS_KEY_H
#define LH_OS_KEY_H

/**
 * @enum lh_os_key
 * @brief One key.
 */
typedef enum lh_os_key
{
    lh_os_key_other = 0, /**< Any key not listed; its text (if any) comes as a code point. */
    lh_os_key_tab,
    lh_os_key_enter,
    lh_os_key_escape,
    lh_os_key_backspace,
    lh_os_key_delete,
    lh_os_key_space,
    lh_os_key_left,
    lh_os_key_right,
    lh_os_key_up,
    lh_os_key_down,
    lh_os_key_home,
    lh_os_key_end,
    lh_os_key_page_up,
    lh_os_key_page_down,
    lh_os_key_shift,
    lh_os_key_control,
    lh_os_key_alt
} lh_os_key_t; /**< One key. */

#endif /* LH_OS_KEY_H */
