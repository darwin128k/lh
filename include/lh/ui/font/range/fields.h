/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_font_range_t.
 *
 * The order is the positional initializer `scripts/font.py` writes.
 */

#ifndef LH_UI_FONT_RANGE_FIELDS_H
#define LH_UI_FONT_RANGE_FIELDS_H

/**
 * @def lh_ui_font_range_fields(code_type, count_type)
 * @brief One run of consecutive code points and where its glyphs sit.
 *
 * A font is a list of these and not a single run, because the code points a
 * font covers are not one run: Latin and Cyrillic are `0x20..0x7E` and
 * `0x400..0x45F`, and a format that could only say "from `first`, `count` of them"
 * would have to spend a thousand empty slots on the hole between them. At 24 bytes a
 * mask that is 35 KB of tables for 224 glyphs -- a third of a whole bitmap font
 * spent on characters that cannot be typed.
 *
 * `base` is where the run's first glyph sits in the font's own `glyphs`,
 * `advances` and `tops`, so the tables stay dense and the runs stay readable.
 *
 * @param code_type Type of a code point (::lh_u32_t: Cyrillic is 0x400 and a
 *                  `lh_byte_t` cannot hold it).
 * @param count_type Type of the length and the base (::lh_u32_t).
 */
#define lh_ui_font_range_fields(code_type, count_type)                                                  \
    code_type first;                                                                                  \
    count_type length;                                                                                \
    count_type base

#endif /* LH_UI_FONT_RANGE_FIELDS_H */