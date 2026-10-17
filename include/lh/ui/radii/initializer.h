/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_radii_t.
 */

#ifndef LH_UI_RADII_INITIALIZER_H
#define LH_UI_RADII_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_radii_initializer(top_left, top_right, bottom_right, bottom_left)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_radii_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_radii_fields.
 *
 * @param top_left     Value for @c top_left.
 * @param top_right    Value for @c top_right.
 * @param bottom_right Value for @c bottom_right.
 * @param bottom_left  Value for @c bottom_left.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_radii_t radii = lh_ui_radii_initializer(6, 6, 0, 0);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_radii_t
 */
#define lh_ui_radii_initializer(top_left, top_right, bottom_right, bottom_left)                    \
    lh_initializer(top_left, top_right, bottom_right, bottom_left)

#endif /* LH_UI_RADII_INITIALIZER_H */
