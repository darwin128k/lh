/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_recognizer_pointer_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_POINTER_FIELDS_H
#define LH_OS_INPUT_RECOGNIZER_POINTER_FIELDS_H

/**
 * @def lh_os_input_recognizer_pointer_fields
 * @brief One pointer's gesture, in this order:
 *
 * - `phase` — where the gesture is;
 * - `device_kind`, `button`, `mods` — of the press;
 * - `down_x`, `down_y`, `down_time` — where and when it went down;
 * - `last_x`, `last_y` — where the last drag event left it;
 * - `click_button`, `click_x`, `click_y`, `click_time`, `click_count` — the
 *   last click, for the next one to continue the series (`click_count` `0`
 *   when there is none to continue).
 *
 * @param phase_type       ::lh_os_input_recognizer_phase_t.
 * @param device_kind_type ::lh_os_input_device_kind_t.
 * @param button_type      ::lh_os_input_button_t.
 * @param mods_type        ::lh_os_input_mods_t.
 * @param coord_type       ::lh_os_input_coord_t.
 * @param tick_type        ::lh_tick_t.
 */
#define lh_os_input_recognizer_pointer_fields(phase_type, device_kind_type, button_type,           \
                                              mods_type, coord_type, tick_type)                    \
    phase_type phase;                                                                              \
    device_kind_type device_kind;                                                                  \
    button_type button;                                                                            \
    mods_type mods;                                                                                \
    coord_type down_x;                                                                             \
    coord_type down_y;                                                                             \
    tick_type down_time;                                                                           \
    coord_type last_x;                                                                             \
    coord_type last_y;                                                                             \
    button_type click_button;                                                                      \
    coord_type click_x;                                                                            \
    coord_type click_y;                                                                            \
    tick_type click_time;                                                                          \
    lh_u8_t click_count

#endif /* LH_OS_INPUT_RECOGNIZER_POINTER_FIELDS_H */
