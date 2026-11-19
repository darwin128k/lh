/**
 * @file ramp.h
 * @brief A shadow's corner fade, worked out as thresholds: ::lh_pixel_shadow_ramp_t.
 *
 * Past a corner a pixel's distance is a square root, and that root was half of what
 * a whole frame of shadows cost (measured in gl). The alpha it leads to only falls as
 * the root grows, and there are at most 255 alphas, so where each alpha gives way to
 * the next is worked out once per shadow as a squared distance
 * (::lh_pixel_shadow_ramp_init). A pixel then compares its own squared distance with
 * them instead of taking the root, and a pixel beside the last one is a step or two
 * along the same thresholds (::lh_pixel_shadow_ramp_get_level). The alphas are the
 * root's, to the last one (::lh_pixel_shadow_get_alpha).
 *
 * 2 KB, a value on the stack of whoever draws the shadow.
 */

#ifndef LH_CORE_PIXEL_SHADOW_RAMP_H
#define LH_CORE_PIXEL_SHADOW_RAMP_H

#include <lh/core/pixel/shadow.h>
#include <lh/core/pixel/shadow/ramp/fields.h>

/**
 * @def LH_PIXEL_SHADOW_RAMP_LEVELS
 * @brief The thresholds a ramp holds: one per alpha above `0`.
 */
#define LH_PIXEL_SHADOW_RAMP_LEVELS 255

/**
 * @struct lh_pixel_shadow_ramp
 * @brief The thresholds of one shadow. Members: ::lh_pixel_shadow_ramp_fields.
 */
struct lh_pixel_shadow_ramp
{
    /** Members: ::lh_pixel_shadow_ramp_fields. */
    lh_pixel_shadow_ramp_fields(LH_PIXEL_SHADOW_RAMP_LEVELS);
};
/** @copybrief lh_pixel_shadow_ramp */
typedef struct lh_pixel_shadow_ramp lh_pixel_shadow_ramp_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The thresholds, `square[level]` (level 1).
 *
 * @param self The ramp; checked by ::lh_assert_runtime_ref.
 * @return The ramp's own table.
 */
const lh_u64_t *
lh_pixel_shadow_ramp_get_square(const lh_pixel_shadow_ramp_t *self);

/**
 * @brief The thresholds, to write them (level 1).
 *
 * @param self The ramp; checked by ::lh_assert_runtime_ref.
 * @return The ramp's own table.
 */
lh_u64_t *
lh_pixel_shadow_ramp_get_square_mut(lh_pixel_shadow_ramp_t *self);

/**
 * @brief How many thresholds are used: the shadow's peak (level 1).
 *
 * @param self The ramp; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_ramp_get_count(const lh_pixel_shadow_ramp_t *self);

/**
 * @brief Store how many thresholds are used (level 1).
 *
 * @param self  The ramp; checked by ::lh_assert_runtime_ref.
 * @param count `0 ..` ::LH_PIXEL_SHADOW_RAMP_LEVELS.
 */
lh_void
lh_pixel_shadow_ramp_set_count(lh_pixel_shadow_ramp_t *self, lh_s32_t count);

/**
 * @brief Work the thresholds of @p shadow out: for each alpha below the peak, the
 *        least squared distance past a corner's centre at which the fade has
 *        dropped to it.
 *
 * @param self   The ramp; checked by the accessors.
 * @param shadow The shadow; must not be null. Read, not kept.
 */
lh_void
lh_pixel_shadow_ramp_init(lh_pixel_shadow_ramp_t *self, const lh_pixel_shadow_t *shadow);

/**
 * @brief The alpha of a pixel @p square away (squared, sub-pixel units) from a
 *        corner's centre: how many thresholds lie beyond it, stepped to from
 *        @p level, the alpha of a pixel beside it.
 *
 * @param self   The ramp; checked by the accessors.
 * @param square The pixel's squared distance.
 * @param level  A neighbour's alpha, or `0`.
 * @return `0 ..` the count.
 */
lh_s32_t
lh_pixel_shadow_ramp_get_level(const lh_pixel_shadow_ramp_t *self, lh_u64_t square, lh_s32_t level);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_SHADOW_RAMP_H */
