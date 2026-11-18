/**
 * @file step.h
 * @brief What one event did to a drag: ::lh_gx_drag_step_t.
 */

#ifndef LH_GX_DRAG_STEP_H
#define LH_GX_DRAG_STEP_H

/**
 * @enum lh_gx_drag_step
 * @brief What ::lh_gx_drag_take answers: the part of the gesture the event was.
 */
enum lh_gx_drag_step
{
    /** Nothing changed: a move or an end with no drag in progress. */
    lh_gx_drag_step_none,
    /** The entity was pressed: the drag began where the pointer is. */
    lh_gx_drag_step_begin,
    /** The pointer moved while the drag goes on. */
    lh_gx_drag_step_move,
    /** The pointer was released: the drag ended where it is. */
    lh_gx_drag_step_end,
    /** The drag was taken away: what it did should be undone. */
    lh_gx_drag_step_cancel
};
/** @copybrief lh_gx_drag_step */
typedef enum lh_gx_drag_step lh_gx_drag_step_t;

#endif /* LH_GX_DRAG_STEP_H */
