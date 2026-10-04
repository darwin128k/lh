/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_font_t.
 */

#ifndef LH_UI_FONT_FIELDS_H
#define LH_UI_FONT_FIELDS_H

/**
 * @def lh_ui_font_fields(view_type, int_type)
 * @brief A font wraps glyph bitmaps that already live in memory.
 *
 * `glyphs` is a non-owning view of the bitmaps. Each glyph is
 * `row_bytes * glyph_height` bytes. A row packs `bpp` bits per pixel
 * from the high bit of the first byte, the same way LVGL's font
 * converter does, and `bpp` is 1, 2, 4 or 8. `row_bytes` is
 * `(glyph_width * bpp + 7) / 8`. `advances` is one byte per glyph, the
 * pen step; an uninitialized view means every glyph is `glyph_width`
 * wide. `first` is the first character code, and `count` glyphs follow
 * it in order. Neither view is copied.
 *
 * @param view_type Type of `glyphs` and `advances` (::lh_memory_view_t).
 * @param int_type  Type of the sizes and the character range (::lh_int_t).
 */
#define lh_ui_font_fields(view_type, int_type)                                                     \
    view_type glyphs;                                                                              \
    view_type advances;                                                                            \
    int_type glyph_width;                                                                          \
    int_type glyph_height;                                                                         \
    int_type row_bytes;                                                                            \
    int_type bpp;                                                                                  \
    int_type first;                                                                                \
    int_type count

#endif /* LH_UI_FONT_FIELDS_H */
