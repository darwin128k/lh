/**
 * @file step.h
 * @brief Which way a walk went: ::lh_gx_walk_step_t.
 */

#ifndef LH_GX_WALK_STEP_H
#define LH_GX_WALK_STEP_H

/**
 * @enum lh_gx_walk_step
 * @brief Which way ::lh_gx_walk_next went from one entity to the next.
 *
 * A walker that carries something down the tree with the origin — the clip of
 * the renderer — reads the step: down, the children inherit what was carried;
 * across, it stays; up, it is found again from the owners.
 */
enum lh_gx_walk_step
{
    /** The walk is over: there is no next entity. */
    lh_gx_walk_step_end,
    /** To the first child of the entity just visited. */
    lh_gx_walk_step_down,
    /** To the next sibling of the entity just visited. */
    lh_gx_walk_step_across,
    /** Out of a subtree: to the next sibling of an owner. */
    lh_gx_walk_step_up
};

/** @brief Which way a walk went. */
typedef enum lh_gx_walk_step lh_gx_walk_step_t;

#endif /* LH_GX_WALK_STEP_H */
