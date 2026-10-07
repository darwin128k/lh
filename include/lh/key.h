/**
 * @file key.h
 * @brief Portable key codes: ::lh_key_t.
 *
 * The keys a UI acts on without reading text: editing, moving, focus. The OS
 * layer reports these (Win32 virtual keys are mapped once, in
 * `src/lh/os/system/win/key.c`); everything else is ::lh_key_other, and what
 * it types arrives separately as text (a code point).
 */

#ifndef LH_KEY_H
#define LH_KEY_H

/**
 * @enum lh_key
 * @typedef lh_key_t
 * @brief One key.
 */
typedef enum lh_key
{
    lh_key_other = 0, /**< Any key not listed; its text (if any) comes as a code point. */
    lh_key_tab,
    lh_key_enter,
    lh_key_escape,
    lh_key_backspace,
    lh_key_delete,
    lh_key_space,
    lh_key_left,
    lh_key_right,
    lh_key_up,
    lh_key_down,
    lh_key_home,
    lh_key_end,
    lh_key_page_up,
    lh_key_page_down,
    lh_key_shift,
    lh_key_control,
    lh_key_alt
} lh_key_t;

#endif /* LH_KEY_H */
