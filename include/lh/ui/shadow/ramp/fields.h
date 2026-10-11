/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_shadow_ramp_t.
 */

#ifndef LH_UI_SHADOW_RAMP_FIELDS_H
#define LH_UI_SHADOW_RAMP_FIELDS_H

/**
 * @def lh_ui_shadow_ramp_fields(levels)
 * @brief A corner's fade as squared distances: `square[a]` is the least
 *        `ox * ox + oy * oy` at which the alpha is `a` or less, for every `a` below
 *        the peak, and `count` is how many of them there are -- the peak.
 *
 * @param levels ::LH_UI_SHADOW_RAMP_LEVELS.
 */
#define lh_ui_shadow_ramp_fields(levels)                                                            \
    lh_u64_t square[levels];                                                                        \
    lh_s32_t count

#endif /* LH_UI_SHADOW_RAMP_FIELDS_H */
