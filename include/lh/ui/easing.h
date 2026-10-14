/**
 * @file easing.h
 * @brief How a transition moves through its time: ::lh_ui_easing_t.
 *
 * The curves CSS names, as CSS defines them -- cubic Béziers from (0, 0) to (1, 1) --
 * sampled once into a table of seventeen points and read between them in whole
 * numbers, so a build with no FPU moves the same way. ::lh_ui_easing_ease is the
 * default, because it is CSS's: a `transition: 150ms` that names no curve eases.
 */

#ifndef LH_UI_EASING_H
#define LH_UI_EASING_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

/**
 * @enum lh_ui_easing
 * @brief A curve from the start of a transition to its end.
 */
typedef enum lh_ui_easing
{
    lh_ui_easing_ease = 0,    /**< `cubic-bezier(.25, .1, .25, 1)`: quick, then settling. */
    lh_ui_easing_linear,      /**< The same speed throughout. */
    lh_ui_easing_ease_in,     /**< `cubic-bezier(.42, 0, 1, 1)`: slow to start. */
    lh_ui_easing_ease_out,    /**< `cubic-bezier(0, 0, .58, 1)`: slow to stop. */
    lh_ui_easing_ease_in_out  /**< `cubic-bezier(.42, 0, .58, 1)`: slow at both ends. */
} lh_ui_easing_t; /**< A curve from the start of a transition to its end. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How far along @p easing is when @p time of the transition is spent, both in
 *        weights of `255`: `0` and `255` map to themselves.
 */
lh_u32_t
lh_ui_easing_apply(lh_ui_easing_t easing, lh_u32_t time);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_EASING_H */
