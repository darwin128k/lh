/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_font_t.
 *
 * The order is the positional initializer `scripts/font.py` writes.
 */

#ifndef LH_UI_FONT_FIELDS_H
#define LH_UI_FONT_FIELDS_H

/**
 * @def lh_ui_font_fields(byte_type, mask_type, length_type, count_type, range_type)
 * @brief Cropped glyph masks, advances, tops, the runs they sit in, and line metrics.
 *
 * `glyphs`, `advances` and `tops` are **dense**: one entry per glyph the font has,
 * in the order `ranges` lists them, and a run's glyphs start at its `base`. A glyph
 * with no ink (a space) is still a zero-size mask with a real advance, because the
 * pen has to move.
 *
 * `ranges` is what says which code point is which glyph. It is a list rather than a
 * `first` and a `count` because the code points a real font covers are not one run:
 * Cyrillic is `0x400..0x45F` and ASCII is `0x20..0x7E`, and a font that could only
 * name a single run would have to carry every slot between them. At 24 bytes a mask
 * and 5 bytes of metrics, a dense `0x20..0x4FF` is 1216 slots for 224 glyphs --
 * about 35 KB of tables, nearly all of it characters nobody can type.
 *
 * `cap_height` is the distance from the baseline up to a capital letter, which
 * is not a property of any one glyph: it is what the letters of a word share
 * from their cap line down to the line they sit on, and it is what a text is
 * centred by (see `LH_UI_TRIM_*`). The generator reads it from the font file.
 *
 * @param byte_type   Type of one advance (also the bits of a packed glyph).
 * @param mask_type   Type of one glyph mask (::lh_ui_mask_t).
 * @param length_type Type of a vertical metric, in pixels (signed: tops).
 * @param count_type  Type of a count.
 * @param range_type  Type of one run (::lh_ui_font_range_t).
 */
#define lh_ui_font_fields(byte_type, mask_type, length_type, count_type, range_type)                    \
    const mask_type *glyphs;                                                                            \
    const byte_type *advances;                                                                          \
    const length_type *tops;                                                                            \
    const range_type *ranges;                                                                           \
    length_type line_height;                                                                            \
    length_type ascent;                                                                                 \
    length_type cap_height;                                                                             \
    count_type range_count

#endif /* LH_UI_FONT_FIELDS_H */