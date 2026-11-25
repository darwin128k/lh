/**
 * @file phase.h
 * @brief Where one pointer is in its gesture: ::lh_input_recognizer_phase_t.
 */

#ifndef LH_INPUT_RECOGNIZER_PHASE_H
#define LH_INPUT_RECOGNIZER_PHASE_H

/**
 * @enum lh_input_recognizer_phase
 * @brief The state of one pointer in ::lh_input_recognizer_t.
 */
typedef enum lh_input_recognizer_phase
{
    lh_input_recognizer_phase_idle = 0, /**< Nothing held. */
    lh_input_recognizer_phase_pending,  /**< Down, not moved past the slop, not held long. */
    lh_input_recognizer_phase_holding,  /**< Down long enough for a long press. */
    lh_input_recognizer_phase_dragging  /**< Down and moved past the slop. */
} lh_input_recognizer_phase_t;          /**< The state of one pointer. */

#endif /* LH_INPUT_RECOGNIZER_PHASE_H */
