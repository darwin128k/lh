/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_glyph_t.
 */

#ifndef LH_GL_GLYPH_FIELDS_H
#define LH_GL_GLYPH_FIELDS_H

/**
 * @def lh_gl_glyph_fields(scalar_type, byte_type)
 * @brief One glyph's ink and the step the pen takes after it.
 *
 * `bits` is borrowed, one byte per pixel, rows of `width`. A byte is coverage
 * `0..255`. A glyph with no ink has a zero size and a null `bits`.
 *
 * @param scalar_type ::lh_gl_scalar_t. `width`, `height`, `advance` and `top`.
 *                    `top` is the ink's top relative to the baseline. Up is
 *                    negative.
 * @param byte_type   ::lh_byte_t. The member is `bits`, borrowed.
 */
#define lh_gl_glyph_fields(scalar_type, byte_type)                                                 \
    scalar_type width;                                                                             \
    scalar_type height;                                                                            \
    scalar_type advance;                                                                           \
    scalar_type top;                                                                               \
    const byte_type *bits

#endif /* LH_GL_GLYPH_FIELDS_H */
