/**
 * @file pointer.h
 * @brief One pointer's gesture inside the recognizer:
 *        ::lh_os_input_recognizer_pointer_t.
 *
 * Part of ::lh_os_input_recognizer_t's storage. Only the recognizer reads and
 * writes it; what a caller may ask is on the recognizer
 * (::lh_os_input_recognizer_get_phase).
 */

#ifndef LH_OS_INPUT_RECOGNIZER_POINTER_H
#define LH_OS_INPUT_RECOGNIZER_POINTER_H

#include <lh/os/input/button.h>
#include <lh/os/input/coord.h>
#include <lh/os/input/device/kind.h>
#include <lh/os/input/mods.h>
#include <lh/os/input/recognizer/phase.h>
#include <lh/os/input/recognizer/pointer/fields.h>
#include <lh/timer/tick.h>

/**
 * @struct lh_os_input_recognizer_pointer
 * @brief The press, the drag and the last click of one pointer.
 */
struct lh_os_input_recognizer_pointer
{
    /** Members: ::lh_os_input_recognizer_pointer_fields. */
    lh_os_input_recognizer_pointer_fields(lh_os_input_recognizer_phase_t, lh_os_input_device_kind_t,
                                          lh_os_input_button_t, lh_os_input_mods_t,
                                          lh_os_input_coord_t, lh_tick_t);
};
/** @copybrief lh_os_input_recognizer_pointer */
typedef struct lh_os_input_recognizer_pointer lh_os_input_recognizer_pointer_t;

#endif /* LH_OS_INPUT_RECOGNIZER_POINTER_H */
