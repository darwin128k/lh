/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_font_t.
 *
 * The order is the positional initializer `scripts/font.py` writes.
 */

#ifndef LH_UI_FONT_FIELDS_H
#define LH_UI_FONT_FIELDS_H

/**
 * @def lh_ui_font_fields(byte_type, mask_type, length_type, count_type)
 * @brief Cropped glyph masks, advances, tops, and line metrics.
 *
 * `glyphs` holds `count` masks (not owned), each cropped to its own ink;
 * `advances` one byte per glyph; `tops` the mask top relative to the
 * baseline (up is negative). Codes run from `first`.
 *
 * @param byte_type   Type of one byte (also the first code).
 * @param mask_type   Type of one glyph mask (::lh_ui_mask_t).
 * @param length_type Type of a vertical metric, in pixels (signed: tops).
 * @param count_type  Type of the glyph count.
 */
#define lh_ui_font_fields(byte_type, mask_type, length_type, count_type)                            \
    const mask_type *glyphs;                                                                        \
    const byte_type *advances;                                                                      \
    const length_type *tops;                                                                        \
    length_type line_height;                                                                        \
    length_type ascent;                                                                             \
    byte_type first;                                                                                \
    count_type count

#endif /* LH_UI_FONT_FIELDS_H */
