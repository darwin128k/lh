/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_font_range_t.
 */

#ifndef LH_GL_FONT_RANGE_INITIALIZER_H
#define LH_GL_FONT_RANGE_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_font_range_initializer(first, length, base)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_font_range_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_font_range_fields.
 *
 * @param first  First code point of the run.
 * @param length How many code points the run has.
 * @param base   Index of the run's first glyph in the font's tables.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_font_range_t latin = lh_gl_font_range_initializer(0x20, 95, 0);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_font_range_t
 */
#define lh_gl_font_range_initializer(first, length, base) lh_initializer(first, length, base)

#endif /* LH_GL_FONT_RANGE_INITIALIZER_H */
