/**
 * @file fn.h
 * @brief ::lh_rail_loop_step_fn — one round of a loop's work.
 */

#ifndef LH_RAIL_LOOP_STEP_FN_H
#define LH_RAIL_LOOP_STEP_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Do what there is to do now, then return: the loop sleeps until it is woken,
 *        its period ends or a timer is due.
 *
 * Runs on the loop's thread only. When ::lh_rail_loop_is_last_step is true this is the
 * last round: finish, drain or release what is left.
 *
 * @param user The value the loop was given.
 */
typedef lh_void(lh_rail_loop_step_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_LOOP_STEP_FN_H */
