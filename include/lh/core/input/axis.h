/**
 * @file axis.h
 * @brief Controller axes: ::lh_input_axis_t.
 */

#ifndef LH_INPUT_AXIS_H
#define LH_INPUT_AXIS_H

/**
 * @enum lh_input_axis
 * @brief One controller axis. A stick is -32768..32767, a trigger 0..32767.
 */
typedef enum lh_input_axis
{
    lh_input_axis_none = 0,     /**< Not an axis event. */
    lh_input_axis_left_x,       /**< Left stick, left to right. */
    lh_input_axis_left_y,       /**< Left stick, up to down. */
    lh_input_axis_right_x,      /**< Right stick, left to right. */
    lh_input_axis_right_y,      /**< Right stick, up to down. */
    lh_input_axis_trigger_left, /**< Left trigger. */
    lh_input_axis_trigger_right /**< Right trigger. */
} lh_input_axis_t;              /**< One controller axis. */

/**
 * @def LH_INPUT_AXIS_COUNT
 * @brief Entries in an axis table indexed by ::lh_input_axis_t, including
 *        ::lh_input_axis_none at 0.
 */
#define LH_INPUT_AXIS_COUNT (lh_input_axis_trigger_right + 1)

#endif /* LH_INPUT_AXIS_H */
