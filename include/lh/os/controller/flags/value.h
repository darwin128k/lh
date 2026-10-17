/**
 * @file value.h
 * @brief The bits of ::lh_os_controller_flags_t: ::lh_os_controller_flags_value_t.
 */

#ifndef LH_OS_CONTROLLER_FLAGS_VALUE_H
#define LH_OS_CONTROLLER_FLAGS_VALUE_H

#include <lh/bit/ops.h>

/**
 * @enum lh_os_controller_flags_value
 * @brief One bit of ::lh_os_controller_flags_t.
 *
 * ::lh_os_controller_init sets both ::lh_os_controller_flags_enabled and
 * ::lh_os_controller_flags_gestures.
 */
typedef enum lh_os_controller_flags_value
{
    lh_os_controller_flags_none = 0,           /**< No bit: every event is dropped. */
    lh_os_controller_flags_enabled = lh_bit_mask(0), /**< `handle_cb` receives events. */
    lh_os_controller_flags_gestures = lh_bit_mask(1) /**< ::lh_os_controller_feed runs the recognizer. */
} lh_os_controller_flags_value_t;              /**< One bit of ::lh_os_controller_flags_t. */

#endif /* LH_OS_CONTROLLER_FLAGS_VALUE_H */
