/**
 * @file input.h
 * @brief Backend-private: X button numbers and `state` masks as the pieces of an
 *        ::lh_os_input_event_t.
 *
 * Pure functions, so they are tested without a display.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_INPUT_H
#define LH_SRC_OS_SYSTEM_POSIX_INPUT_H

#include <lh/os/input/event.h>

/**
 * @brief The button X numbers @p xbutton; ::lh_os_input_button_none for the wheel
 *        (4..7) and for any number past the side buttons.
 */
lh_os_input_button_t
lh_os_system_posix_input_button_of(lh_u32_t xbutton);

/**
 * @brief The distance one press of X button @p xbutton carries, in 1/120 notch
 *        (::LH_OS_INPUT_WHEEL_NOTCH); `0` for a button that is not the wheel.
 *
 * Core X11 reports the wheel as buttons 4..7, one press a notch, with no increment of
 * its own: every press is exactly one notch. A finer increment would come from the
 * XInput2 scroll valuators (`XIScrollClassInfo::increment`), which this backend does
 * not open, so this is core X11 only.
 */
lh_s32_t
lh_os_system_posix_input_wheel_increment_of(lh_u32_t xbutton);

/**
 * @brief The wheel distance of X button @p xbutton, in 1/120 notch, positive left /
 *        up (::lh_os_system_posix_input_wheel_increment_of a press); `0`, `0` for a
 *        button that is not the wheel.
 * @return True when @p xbutton is the wheel.
 */
lh_bool_t
lh_os_system_posix_input_wheel_of(lh_u32_t xbutton, lh_s32_t *dx, lh_s32_t *dy);

/**
 * @brief The modifiers in an event's X @p state.
 */
lh_os_input_mods_t
lh_os_system_posix_input_mods_of(lh_u32_t state);

/**
 * @brief The buttons an X @p state says are held (left, middle, right: X keeps no
 *        bit for the side buttons), as ::lh_os_input_button_to_mask bits.
 */
lh_u32_t
lh_os_system_posix_input_buttons_of(lh_u32_t state);

/**
 * @brief Buttons held after an X event: ::lh_os_system_posix_input_buttons_of(@p state)
 *        plus any side buttons still in @p side_held (back/forward — X has no mask bit).
 */
lh_u32_t
lh_os_system_posix_input_buttons_held(lh_u32_t state, lh_u32_t side_held);

#endif /* LH_SRC_OS_SYSTEM_POSIX_INPUT_H */
