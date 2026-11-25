/**
 * @file fields.h
 * @brief Member fields of ::lh_input_pad_t.
 */

#ifndef LH_INPUT_PAD_FIELDS_H
#define LH_INPUT_PAD_FIELDS_H

#include <lh/core/input/axis.h>

/**
 * @def lh_input_pad_fields
 * @brief The last report of one stick or pad:
 *
 * - `device` — the device id written on each event;
 * - `buttons` — the mask of buttons held;
 * - `values` — the last value of each ::lh_input_axis_t.
 *
 * @param value_type ::lh_s16_t.
 */
#define lh_input_pad_fields(value_type)                                                            \
    lh_u8_t device;                                                                                \
    lh_u32_t buttons;                                                                              \
    value_type values[LH_INPUT_AXIS_COUNT]

#endif /* LH_INPUT_PAD_FIELDS_H */
