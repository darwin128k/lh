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
 * @brief The wheel distance of X button @p xbutton, in 1/120 notch, positive left /
 *        up; `0`, `0` for a button that is not the wheel.
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

#endif /* LH_SRC_OS_SYSTEM_POSIX_INPUT_H */
