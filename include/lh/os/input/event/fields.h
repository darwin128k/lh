/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_event_t.
 */

#ifndef LH_OS_INPUT_EVENT_FIELDS_H
#define LH_OS_INPUT_EVENT_FIELDS_H

/**
 * @def lh_os_input_event_fields
 * @brief One input event, raw or synthesized, in this order:
 *
 * - `kind` — what happened;
 * - `device_kind` — the sort of device; `device` — its id (`0` is the system
 *   mouse and keyboard); `pointer` — the contact (the mouse is `0`, a touch
 *   screen numbers its fingers);
 * - `button` — the button that changed, `buttons` — the mask of every button
 *   of the device held after the event (::lh_os_input_button_to_mask);
 * - `x`, `y` — client position, Q24.8 (::lh_os_input_coord_t);
 * - `dx`, `dy` — a wheel in 1/120 notch (positive is left / up); a drag the
 *   distance in Q24.8 from the previous drag event (from the press for
 *   ::lh_os_input_kind_drag_begin);
 * - `axis`, `value` — a controller axis and its position;
 * - `key` — the portable key, `scancode` — the native code (virtual key,
 *   X keycode), `code` — the typed code point;
 * - `mods` — modifiers held, in every event;
 * - `time` — milliseconds on the ::lh_tick_t clock the recognizer is driven
 *   with;
 * - `count` — the click number of a click (`1`, `2` for the second ...);
 * - `flags` — `LH_OS_INPUT_FLAG_*`.
 *
 * @param kind_type        ::lh_os_input_kind_t.
 * @param device_kind_type ::lh_os_input_device_kind_t.
 * @param button_type      ::lh_os_input_button_t.
 * @param axis_type        ::lh_os_input_axis_t.
 * @param key_type         ::lh_os_key_t.
 * @param coord_type       ::lh_os_input_coord_t.
 * @param mods_type        ::lh_os_input_mods_t.
 * @param tick_type        ::lh_tick_t.
 * @param flags_type       ::lh_os_input_flags_t.
 */
#define lh_os_input_event_fields(kind_type, device_kind_type, button_type, axis_type, key_type,    \
                                 coord_type, mods_type, tick_type, flags_type)                     \
    kind_type kind;                                                                                \
    device_kind_type device_kind;                                                                  \
    lh_u8_t device;                                                                                \
    lh_u8_t pointer;                                                                               \
    button_type button;                                                                            \
    lh_u32_t buttons;                                                                              \
    coord_type x;                                                                                  \
    coord_type y;                                                                                  \
    lh_s32_t dx;                                                                                   \
    lh_s32_t dy;                                                                                   \
    axis_type axis;                                                                                \
    lh_s16_t value;                                                                                \
    key_type key;                                                                                  \
    lh_u32_t scancode;                                                                             \
    lh_u32_t code;                                                                                 \
    mods_type mods;                                                                                \
    tick_type time;                                                                                \
    lh_u8_t count;                                                                                 \
    flags_type flags

#endif /* LH_OS_INPUT_EVENT_FIELDS_H */
