/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_pen_t.
 */

#ifndef LH_UI_PEN_INITIALIZER_H
#define LH_UI_PEN_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_pen_initializer(paint, width)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_pen_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_pen_fields.
 *
 * @param paint Initializer of the ::lh_ui_paint_t (::lh_ui_paint_initializer).
 * @param width Width of the stroke.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_pen_t pen = lh_ui_pen_initializer(
 *     lh_ui_paint_initializer(lh_ui_paint_kind_solid,
 *                             lh_ui_color_initializer(0x00, 0x00, 0x00, 0xFF),
 *                             lh_null),  // paint
 *     1);                                // width
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_pen_t
 */
#define lh_ui_pen_initializer(paint, width) lh_initializer(paint, width)

#endif /* LH_UI_PEN_INITIALIZER_H */
