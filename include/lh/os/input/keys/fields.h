/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_keys_t.
 */

#ifndef LH_OS_INPUT_KEYS_FIELDS_H
#define LH_OS_INPUT_KEYS_FIELDS_H

/**
 * @def lh_os_input_keys_fields(mask_type)
 * @brief The last report of one polled keyboard:
 *
 * - `device` — the device id written on each event;
 * - `down` — the mask of ::lh_os_key_t values that are held. Bit 0
 *   (::lh_os_key_other) stays clear.
 *
 * @param mask_type ::lh_u32_t.
 */
#define lh_os_input_keys_fields(mask_type)                                                         \
    lh_u8_t device;                                                                                \
    mask_type down

#endif /* LH_OS_INPUT_KEYS_FIELDS_H */
