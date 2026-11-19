/**
 * @file lerp.h
 * @brief The colours between two stops, a step at a time: ::lh_pixel_gradient_lerp_t.
 *
 * Between two stops each channel is `(from * (span - at) + to * at) / span` —
 * what ::lh_pixel_gradient_get_color_at computes with a division per channel.
 * One step on adds `to - from` to what is divided, so the quotient and its
 * remainder can be carried instead of dividing again: the lerp gives the same
 * colours, step after step, with additions only. The colour table
 * (`lh/core/pixel/gradient/table.h`) fills its 256 entries with it.
 *
 * A value on the stack of whoever walks the span; it points at nothing.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_LERP_H
#define LH_CORE_PIXEL_GRADIENT_LERP_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/core/void.h>
#include <lh/core/pixel/gradient/lerp/fields.h>

/**
 * @struct lh_pixel_gradient_lerp
 * @brief A colour between two stops and what a step adds. Members:
 *        ::lh_pixel_gradient_lerp_fields.
 */
struct lh_pixel_gradient_lerp
{
    /** Members: ::lh_pixel_gradient_lerp_fields. */
    lh_pixel_gradient_lerp_fields(lh_s32_t);
};
/** @copybrief lh_pixel_gradient_lerp */
typedef struct lh_pixel_gradient_lerp lh_pixel_gradient_lerp_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The channels' values, blue, green, red, alpha (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
const lh_s32_t *
lh_pixel_gradient_lerp_get_quotient(const lh_pixel_gradient_lerp_t *self);

/**
 * @brief The channels' values, to change them (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
lh_s32_t *
lh_pixel_gradient_lerp_get_quotient_mut(lh_pixel_gradient_lerp_t *self);

/**
 * @brief The channels' remainders, `0 .. span - 1` (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
const lh_s32_t *
lh_pixel_gradient_lerp_get_rest(const lh_pixel_gradient_lerp_t *self);

/**
 * @brief The channels' remainders, to change them (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
lh_s32_t *
lh_pixel_gradient_lerp_get_rest_mut(lh_pixel_gradient_lerp_t *self);

/**
 * @brief What one step adds to each value (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
const lh_s32_t *
lh_pixel_gradient_lerp_get_step(const lh_pixel_gradient_lerp_t *self);

/**
 * @brief What one step adds to each value, to change it (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
lh_s32_t *
lh_pixel_gradient_lerp_get_step_mut(lh_pixel_gradient_lerp_t *self);

/**
 * @brief What one step adds to each remainder, `0 .. span - 1` (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
const lh_s32_t *
lh_pixel_gradient_lerp_get_step_rest(const lh_pixel_gradient_lerp_t *self);

/**
 * @brief What one step adds to each remainder, to change it (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @return Four values, the lerp's own.
 */
lh_s32_t *
lh_pixel_gradient_lerp_get_step_rest_mut(lh_pixel_gradient_lerp_t *self);

/**
 * @brief The number of steps between the two stops (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_gradient_lerp_get_span(const lh_pixel_gradient_lerp_t *self);

/**
 * @brief Store the number of steps between the two stops (level 1).
 *
 * @param self The lerp; checked by ::lh_assert_runtime_ref.
 * @param span Above zero.
 */
lh_void
lh_pixel_gradient_lerp_set_span(lh_pixel_gradient_lerp_t *self, lh_s32_t span);

/**
 * @brief Start at step @p at of @p span between @p from and @p to: the colour
 *        there is what ::lh_pixel_gradient_get_color_at gives.
 *
 * @param self The lerp; checked by the accessors.
 * @param from The first stop's colour, `0xAARRGGBB`.
 * @param to   The second's.
 * @param span The steps between them; above zero (not checked: a division).
 * @param at   The step to start at, `0 .. span`.
 */
lh_void
lh_pixel_gradient_lerp_init(lh_pixel_gradient_lerp_t *self, lh_u32_t from, lh_u32_t to,
                            lh_s32_t span, lh_s32_t at);

/**
 * @brief Move one step on.
 *
 * @param self The lerp; checked by the accessors.
 */
lh_void
lh_pixel_gradient_lerp_next(lh_pixel_gradient_lerp_t *self);

/**
 * @brief The colour at the current step.
 *
 * @param self The lerp; checked by ::lh_pixel_gradient_lerp_get_quotient.
 * @return `0xAARRGGBB`.
 */
lh_u32_t
lh_pixel_gradient_lerp_get_color(const lh_pixel_gradient_lerp_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_GRADIENT_LERP_H */
