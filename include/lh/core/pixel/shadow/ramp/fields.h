/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_shadow_ramp_t.
 */

#ifndef LH_CORE_PIXEL_SHADOW_RAMP_FIELDS_H
#define LH_CORE_PIXEL_SHADOW_RAMP_FIELDS_H

/**
 * @def lh_pixel_shadow_ramp_fields(levels)
 * @brief Where each alpha of a shadow's corner gives way to the next, as a squared
 *        distance: `square[level]` is the least squared distance past the corner's
 *        centre whose alpha is `level` or less; `count` how many are used — the
 *        shadow's peak.
 *
 * @param levels The table's length (::LH_PIXEL_SHADOW_RAMP_LEVELS).
 */
#define lh_pixel_shadow_ramp_fields(levels)                                                        \
    lh_u64_t square[levels];                                                                       \
    lh_s32_t count

#endif /* LH_CORE_PIXEL_SHADOW_RAMP_FIELDS_H */
