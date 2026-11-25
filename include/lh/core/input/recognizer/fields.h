/**
 * @file fields.h
 * @brief Member fields of ::lh_input_recognizer_t.
 */

#ifndef LH_INPUT_RECOGNIZER_FIELDS_H
#define LH_INPUT_RECOGNIZER_FIELDS_H

#include <lh/core/timer/tick.h>
#include <lh/core/input/axis.h>
#include <lh/core/input/pointer.h>

/**
 * @def lh_input_recognizer_fields(config_type, pointer_type)
 * @brief The thresholds (`config`), then one gesture per pointer id
 *        (`pointers`, ::LH_INPUT_POINTER_MAX of them). After that, the one
 *        controller the recognizer is filtering: its button mask and when the
 *        next repeat is due, and each axis's last output.
 *
 * @param config_type  ::lh_input_recognizer_config_t.
 * @param pointer_type ::lh_input_recognizer_pointer_t.
 */
#define lh_input_recognizer_fields(config_type, pointer_type)                                      \
    config_type config;                                                                            \
    pointer_type pointers[LH_INPUT_POINTER_MAX];                                                   \
    lh_u8_t pad_device;                                                                            \
    lh_u32_t pad_buttons;                                                                          \
    lh_tick_t pad_repeat_at;                                                                       \
    lh_bool_t pad_armed;                                                                           \
    lh_u8_t axis_device;                                                                           \
    lh_bool_t axis_hot[LH_INPUT_AXIS_COUNT];                                                       \
    lh_s16_t axis_out[LH_INPUT_AXIS_COUNT]

#endif /* LH_INPUT_RECOGNIZER_FIELDS_H */
