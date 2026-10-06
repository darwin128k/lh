/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_font_t.
 *
 * The order is the positional initializer `scripts/font.py` writes.
 */

#ifndef LH_UI_FONT_FIELDS_H
#define LH_UI_FONT_FIELDS_H

/**
 * @def lh_ui_font_fields(byte_type, length_type, count_type)
 * @brief Glyph cells, advances, and the cell layout they share.
 *
 * `bits` holds `count` cells of `height` rows of `row_bytes`; `advances`
 * holds one byte per glyph. Neither is owned. Codes run from `first`.
 *
 * @param byte_type   Type of one byte (also the depth and the first code).
 * @param length_type Type of a cell measurement, in pixels.
 * @param count_type  Type of the glyph count.
 */
#define lh_ui_font_fields(byte_type, length_type, count_type)                                       \
    const byte_type *bits;                                                                          \
    const byte_type *advances;                                                                      \
    length_type cell_width;                                                                         \
    length_type height;                                                                             \
    length_type row_bytes;                                                                          \
    byte_type bpp;                                                                                  \
    byte_type first;                                                                                \
    count_type count

#endif /* LH_UI_FONT_FIELDS_H */
