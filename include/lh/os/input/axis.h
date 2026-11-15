/**
 * @file axis.h
 * @brief Controller axes: ::lh_os_input_axis_t.
 */

#ifndef LH_OS_INPUT_AXIS_H
#define LH_OS_INPUT_AXIS_H

/**
 * @enum lh_os_input_axis
 * @brief One controller axis. A stick is -32768..32767, a trigger 0..32767.
 */
typedef enum lh_os_input_axis
{
    lh_os_input_axis_none = 0,     /**< Not an axis event. */
    lh_os_input_axis_left_x,       /**< Left stick, left to right. */
    lh_os_input_axis_left_y,       /**< Left stick, up to down. */
    lh_os_input_axis_right_x,      /**< Right stick, left to right. */
    lh_os_input_axis_right_y,      /**< Right stick, up to down. */
    lh_os_input_axis_trigger_left, /**< Left trigger. */
    lh_os_input_axis_trigger_right /**< Right trigger. */
} lh_os_input_axis_t;              /**< One controller axis. */

/**
 * @def LH_OS_INPUT_AXIS_COUNT
 * @brief Entries in an axis table indexed by ::lh_os_input_axis_t, including
 *        ::lh_os_input_axis_none at 0.
 */
#define LH_OS_INPUT_AXIS_COUNT (lh_os_input_axis_trigger_right + 1)

#endif /* LH_OS_INPUT_AXIS_H */
