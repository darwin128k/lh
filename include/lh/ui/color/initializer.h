/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_color_t.
 */

#ifndef LH_UI_COLOR_INITIALIZER_H
#define LH_UI_COLOR_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_color_initializer(r, g, b, a)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_color_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_color_fields.
 *
 * @param r Value for @c r (::lh_ui_color_channel_t).
 * @param g Value for @c g (::lh_ui_color_channel_t).
 * @param b Value for @c b (::lh_ui_color_channel_t).
 * @param a Value for @c a (::lh_ui_color_channel_t).
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_color_t red = lh_ui_color_initializer(0xFF, 0x00, 0x00, 0xFF);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_color_t
 */
#define lh_ui_color_initializer(r, g, b, a) lh_initializer(r, g, b, a)

#endif /* LH_UI_COLOR_INITIALIZER_H */
