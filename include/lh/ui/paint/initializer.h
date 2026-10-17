/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_paint_t.
 */

#ifndef LH_UI_PAINT_INITIALIZER_H
#define LH_UI_PAINT_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_paint_initializer(kind, color, gradient)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_paint_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_paint_fields.
 *
 * @param kind     Paint kind (::lh_ui_paint_kind_t).
 * @param color    Initializer of the ::lh_ui_color_t (::lh_ui_color_initializer).
 * @param gradient `const lh_ui_gradient_t *`, or ::lh_null.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_paint_t paint = lh_ui_paint_initializer(
 *     lh_ui_paint_kind_solid,                           // kind
 *     lh_ui_color_initializer(0x20, 0x40, 0x80, 0xFF),  // color
 *     lh_null);                                         // gradient
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_paint_t
 * @see lh_ui_paint_init
 */
#define lh_ui_paint_initializer(kind, color, gradient) lh_initializer(kind, color, gradient)

#endif /* LH_UI_PAINT_INITIALIZER_H */
