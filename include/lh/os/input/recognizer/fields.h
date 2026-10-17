/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_recognizer_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_FIELDS_H
#define LH_OS_INPUT_RECOGNIZER_FIELDS_H

#include <lh/os/input/pointer.h>

/**
 * @def lh_os_input_recognizer_fields(config_type, pointer_type)
 * @brief The thresholds (`config`), then one gesture per pointer id
 *        (`pointers`, ::LH_OS_INPUT_POINTER_MAX of them).
 *
 * @param config_type  ::lh_os_input_recognizer_config_t.
 * @param pointer_type ::lh_os_input_recognizer_pointer_t.
 */
#define lh_os_input_recognizer_fields(config_type, pointer_type)                                   \
    config_type config;                                                                            \
    pointer_type pointers[LH_OS_INPUT_POINTER_MAX]

#endif /* LH_OS_INPUT_RECOGNIZER_FIELDS_H */
