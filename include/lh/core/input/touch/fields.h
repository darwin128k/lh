/**
 * @file fields.h
 * @brief Member fields of ::lh_input_touch_t.
 */

#ifndef LH_INPUT_TOUCH_FIELDS_H
#define LH_INPUT_TOUCH_FIELDS_H

#include <lh/core/input/pointer.h>

/**
 * @def LH_INPUT_TOUCH_MAX
 * @brief Contacts one ::lh_input_touch_t remembers: every pointer id except
 *        the mouse's `0`.
 */
#define LH_INPUT_TOUCH_MAX (LH_INPUT_POINTER_MAX - 1)

/**
 * @def lh_input_touch_fields
 * @brief The last frame one touch driver reported:
 *
 * - `contacts` — which driver id is which pointer id;
 * - `samples` — the contacts that received a pointer id, `count` of them;
 * - `device` — the device id written on each event;
 * - `device_kind` — touch or pen.
 *
 * @param contacts_type    ::lh_input_contacts_t.
 * @param sample_type      ::lh_input_sample_t.
 * @param device_kind_type ::lh_input_device_kind_t.
 */
#define lh_input_touch_fields(contacts_type, sample_type, device_kind_type)                        \
    contacts_type contacts;                                                                        \
    sample_type samples[LH_INPUT_TOUCH_MAX];                                                       \
    lh_u8_t count;                                                                                 \
    lh_u8_t device;                                                                                \
    device_kind_type device_kind

#endif /* LH_INPUT_TOUCH_FIELDS_H */
