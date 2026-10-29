/**
 * @file ramp.h
 * @brief A shadow's corner fade, worked out as thresholds: ::lh_ui_shadow_ramp_t.
 *
 * In a corner a pixel's distance is a square root, and that root was half of what a
 * whole frame of shadows cost. The alpha it leads to only falls as the root grows,
 * and there are at most 255 alphas, so where each alpha gives way to the next is
 * worked out once per shadow as a squared distance. A pixel then compares its own
 * squared distance with them instead of taking the root, and a pixel beside the last
 * one is a step or two along the same thresholds. The alphas are the root's, to the
 * last one (::lh_ui_shadow_shape_alpha).
 */

#ifndef LH_UI_SHADOW_RAMP_H
#define LH_UI_SHADOW_RAMP_H

#include <lh/ui/shadow/ramp/fields.h>
#include <lh/ui/shadow/shape.h>

/** @brief The most thresholds a ramp holds: one below every alpha but the peak. */
#define LH_UI_SHADOW_RAMP_LEVELS 255

/**
 * @struct lh_ui_shadow_ramp
 * @brief A shadow's corner fade as squared distances.
 */
struct lh_ui_shadow_ramp
{
    /** Members: ::lh_ui_shadow_ramp_fields. */
    lh_ui_shadow_ramp_fields(LH_UI_SHADOW_RAMP_LEVELS);
};
/** @copybrief lh_ui_shadow_ramp */
typedef struct lh_ui_shadow_ramp lh_ui_shadow_ramp_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The corner fade of @p shape.
 */
lh_void
lh_ui_shadow_ramp_init(lh_ui_shadow_ramp_t *self, const lh_ui_shadow_shape_t *shape);

/**
 * @brief The alpha at squared distance @p square past a corner's centre, walked to
 *        from @p level -- the alpha of a pixel near this one, or `0`.
 */
lh_s32_t
lh_ui_shadow_ramp_walk(const lh_ui_shadow_ramp_t *self, lh_u64_t square, lh_s32_t level);

/**
 * @brief The falloff, indexed by squared distance (level 1).
 *
 * @param self The ramp; checked by ::lh_assert_runtime_ref.
 */
const lh_u64_t *
lh_ui_shadow_ramp_get_square(const lh_ui_shadow_ramp_t *self);

/**
 * @brief The falloff, indexed by squared distance, to change in place (level 1).
 *
 * @param self The ramp; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_u64_t *
lh_ui_shadow_ramp_get_square_mut(lh_ui_shadow_ramp_t *self);

/**
 * @brief How many entries the falloff has (level 1).
 *
 * @param self The ramp; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_ramp_get_count(const lh_ui_shadow_ramp_t *self);

/**
 * @brief Store how many entries the falloff has (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The ramp; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_ui_shadow_ramp_set_count(lh_ui_shadow_ramp_t *self, lh_s32_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SHADOW_RAMP_H */
