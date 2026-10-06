/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_gradient_stop_t.
 */

#ifndef LH_UI_GRADIENT_STOP_FIELDS_H
#define LH_UI_GRADIENT_STOP_FIELDS_H

/**
 * @def lh_ui_gradient_stop_fields(color_type, frac_type)
 * @brief Color and position of one gradient stop.
 *
 * @param color_type Type of the stop color.
 * @param frac_type  Type of the stop position (`0..255`).
 */
#define lh_ui_gradient_stop_fields(color_type, frac_type)                                           \
    color_type color;                                                                               \
    frac_type frac

#endif /* LH_UI_GRADIENT_STOP_FIELDS_H */
