/**
 * @file lerp.h
 * @brief ::lh_ui_gradient_sampler_lerp_t — A colour between two stops walked one step of the axis at a time: each channel `(from * (span - at) + to * at) / span` as a quotient and remainder, and what one step adds to both.
 */

#ifndef LH_UI_GRADIENT_SAMPLER_LERP_H
#define LH_UI_GRADIENT_SAMPLER_LERP_H

#include <lh/ui/gradient.h>
#include <lh/ui/gradient/sampler/fields.h>

/**
 * @struct lh_ui_gradient_sampler_lerp
 * @brief A colour between two stops walked one step of the axis at a time:
 *        each channel `(from * (span - at) + to * at) / span` as a quotient and
 *        remainder, and what one step adds to both.
 */
struct lh_ui_gradient_sampler_lerp
{
    lh_s32_t quotient[4];  /**< Channel values r, g, b, a. */
    lh_s32_t rest[4];      /**< Their remainders, `0 .. span - 1`. */
    lh_s32_t step[4];      /**< What one step adds to each quotient. */
    lh_s32_t step_rest[4]; /**< And to each remainder. */
    lh_s32_t span;         /**< Steps between the two stops (above zero). */
};
/** @copybrief lh_ui_gradient_sampler_lerp */
typedef struct lh_ui_gradient_sampler_lerp lh_ui_gradient_sampler_lerp_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start a walk @p at steps into the @p span steps from @p from to
 *        @p to (`span > 0`). @p self must not be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_gradient_sampler_lerp_init(lh_ui_gradient_sampler_lerp_t *self, const lh_ui_color_t *from,
                                 const lh_ui_color_t *to, lh_s32_t span, lh_s32_t at);

/**
 * @brief Move the walk one step on.
 */
lh_void
lh_ui_gradient_sampler_lerp_next(lh_ui_gradient_sampler_lerp_t *self);

/**
 * @brief The colour the walk is at, into @p out.
 */
lh_void
lh_ui_gradient_sampler_lerp_color(const lh_ui_gradient_sampler_lerp_t *self, lh_ui_color_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_SAMPLER_LERP_H */
