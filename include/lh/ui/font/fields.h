/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_font_t.
 *
 * The order is the positional initializer `scripts/font.py` writes.
 */

#ifndef LH_UI_FONT_FIELDS_H
#define LH_UI_FONT_FIELDS_H

/**
 * @def lh_ui_font_fields(view_type, coord_type, byte_type, count_type)
 * @brief Glyph bytes, advances, and the cell they share.
 *
 * @param view_type  Type of a byte view.
 * @param coord_type Type of a cell measurement.
 * @param byte_type  Type of the depth and the first code.
 * @param count_type Type of the glyph count.
 */
#define lh_ui_font_fields(view_type, coord_type, byte_type, count_type)                             \
    view_type glyphs;                                                                               \
    view_type advances;                                                                             \
    coord_type cell_width;                                                                          \
    coord_type height;                                                                              \
    coord_type row_bytes;                                                                           \
    byte_type bpp;                                                                                  \
    byte_type first;                                                                                \
    count_type count

#endif /* LH_UI_FONT_FIELDS_H */
