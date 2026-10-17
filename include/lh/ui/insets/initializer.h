/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_insets_t.
 */

#ifndef LH_UI_INSETS_INITIALIZER_H
#define LH_UI_INSETS_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_insets_initializer(left, top, right, bottom)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_insets_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_insets_fields.
 *
 * @param left   Value for @c left.
 * @param top    Value for @c top.
 * @param right  Value for @c right.
 * @param bottom Value for @c bottom.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_insets_t padding = lh_ui_insets_initializer(8, 4, 8, 4);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_insets_t
 */
#define lh_ui_insets_initializer(left, top, right, bottom) lh_initializer(left, top, right, bottom)

#endif /* LH_UI_INSETS_INITIALIZER_H */
