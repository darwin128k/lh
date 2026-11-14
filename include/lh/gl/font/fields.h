/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_font_t.
 */

#ifndef LH_GL_FONT_FIELDS_H
#define LH_GL_FONT_FIELDS_H

/**
 * @def lh_gl_font_fields(glyph_type, code_type, scalar_type)
 * @brief A borrowed row of glyphs for a contiguous run of codes.
 *
 * `glyphs` is not copied. Code `first` is entry 0. A code outside
 * `first .. first + count - 1` has no glyph.
 *
 * @param glyph_type  ::lh_gl_glyph_t. The member is `glyphs`, borrowed.
 * @param code_type   ::lh_u32_t. `first` and `count`.
 * @param scalar_type ::lh_gl_scalar_t. `line_height` and `ascent`. Ascent is
 *                    the baseline down from the top of the line.
 */
#define lh_gl_font_fields(glyph_type, code_type, scalar_type)                                      \
    const glyph_type *glyphs;                                                                      \
    code_type first;                                                                               \
    code_type count;                                                                               \
    scalar_type line_height;                                                                       \
    scalar_type ascent

#endif /* LH_GL_FONT_FIELDS_H */
