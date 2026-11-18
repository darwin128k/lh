/**
 * @file step.h
 * @brief What one event did to a tap: ::lh_gx_tap_step_t.
 */

#ifndef LH_GX_TAP_STEP_H
#define LH_GX_TAP_STEP_H

/**
 * @enum lh_gx_tap_step
 * @brief What ::lh_gx_tap_take answers: the part of the gesture the event was.
 */
enum lh_gx_tap_step
{
    /** Nothing changed: a move, or an end with no press in progress. */
    lh_gx_tap_step_none,
    /** The entity was pressed: the gesture began. */
    lh_gx_tap_step_down,
    /** The gesture ended without a tap: released elsewhere, or cancelled. */
    lh_gx_tap_step_up,
    /** The gesture ended with a tap: released on the entity. */
    lh_gx_tap_step_tap
};
/** @copybrief lh_gx_tap_step */
typedef enum lh_gx_tap_step lh_gx_tap_step_t;

#endif /* LH_GX_TAP_STEP_H */
