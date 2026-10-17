/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_gradient_stop_t.
 */

#ifndef LH_UI_GRADIENT_STOP_INITIALIZER_H
#define LH_UI_GRADIENT_STOP_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_gradient_stop_initializer(color, frac)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_gradient_stop_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_gradient_stop_fields.
 *
 * @param color Initializer of the ::lh_ui_color_t (::lh_ui_color_initializer).
 * @param frac  Position of the stop (`0..255`).
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_gradient_stop_t stop = lh_ui_gradient_stop_initializer(
 *     lh_ui_color_initializer(0xFF, 0xFF, 0xFF, 0xFF),  // color
 *     128);                                             // frac
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_gradient_stop_t
 */
#define lh_ui_gradient_stop_initializer(color, frac) lh_initializer(color, frac)

#endif /* LH_UI_GRADIENT_STOP_INITIALIZER_H */
