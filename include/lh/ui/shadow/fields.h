/**
 * @file fields.h
 * @brief Member fields ::lh_ui_shadow_t adds after ::lh_ui_effect_fields.
 */

#ifndef LH_UI_SHADOW_FIELDS_H
#define LH_UI_SHADOW_FIELDS_H

/**
 * @def lh_ui_shadow_fields(color_type, int_type, side_type)
 * @brief A soft shadow around a rounded box.
 *
 * - `color`: the shadow color. Its alpha is the darkest the shadow gets.
 *   Zero alpha paints nothing.
 * - `spread`: how far the shadow fades, in pixels. Zero paints nothing.
 * - `offset_x`, `offset_y`: where the shadow sits relative to the box.
 *   Positive `offset_y` moves it down.
 * - `sides`: which edges cast a shadow (::lh_ui_shadow_side_t).
 *
 * @param color_type ::lh_ui_color_t.
 * @param int_type   ::lh_int_t.
 * @param side_type  ::lh_ui_shadow_side_t.
 */
#define lh_ui_shadow_fields(color_type, int_type, side_type)                                       \
    color_type color;                                                                              \
    int_type spread;                                                                               \
    int_type offset_x;                                                                             \
    int_type offset_y;                                                                             \
    side_type sides

#endif /* LH_UI_SHADOW_FIELDS_H */
