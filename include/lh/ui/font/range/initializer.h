/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_font_range_t.
 */

#ifndef LH_UI_FONT_RANGE_INITIALIZER_H
#define LH_UI_FONT_RANGE_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_font_range_initializer(first, length, base)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_font_range_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_font_range_fields.
 *
 * @param first  First code point of the run.
 * @param length How many code points the run has.
 * @param base   Index of the run's first glyph in the font's tables.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_font_range_t latin = lh_ui_font_range_initializer(0x20, 95, 0);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_font_range_t
 */
#define lh_ui_font_range_initializer(first, length, base) lh_initializer(first, length, base)

#endif /* LH_UI_FONT_RANGE_INITIALIZER_H */
