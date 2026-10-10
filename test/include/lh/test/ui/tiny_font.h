/**
 * @file tiny_font.h
 * @brief Test helpers: a three-glyph font with known pixels, tops and advances,
 *        a second one whose three metrics are three different numbers, and a third
 *        that has a hole in the middle of its coverage.
 *
 * Codes 'A', 'B', 'C' at 1 bpp. Line height 2, ascent 2 (baseline at the
 * bottom of the line), cap height 2 (the same two rows 'A' rises). 'A' is
 * a full 2 x 2 square, advance 3, top -2. 'B' is
 * a single top-left pixel cropped to 1 x 1, advance 4, top -2. 'C' has no
 * ink: a zero-size mask, advance 2, top 0. Every other code has no glyph.
 *
 * ::lh_test::cap_font is the second font, and the reason it exists is written
 * where it is defined: the three metrics of this one are all the same number.
 *
 * ::lh_test::split_font is the third, and it exists because of what a font **cannot**
 * say. A coverage that is one run with a first code and a count cannot hold 'X'..'Z'
 * as well as 'A'..'C' without every letter between them, so every test of "a code no
 * range covers has no glyph" passed against a font that had no way to have two runs at
 * all. Cyrillic is what made the format change: `U+0400` is a thousand codes above the
 * space, and the gap between them is not something a byte can skip.
 */

#ifndef LH_TEST_UI_TINY_FONT_H
#define LH_TEST_UI_TINY_FONT_H

#include <lh/byte.h>
#include <lh/null.h>
#include <lh/ui/font.h>
#include <lh/ui/font/range.h>
#include <lh/ui/mask.h>
#include <lh/util/addr.h>

namespace lh_test
{

/** The font, filled once. */
inline const lh_ui_font_t *
tiny_font()
{
    static const lh_byte_t bits[] = {0xC0, 0xC0, 0x80};
    static const lh_ui_mask_t glyphs[] = {
        {bits + 0, 2, 2, 1, 1},
        {bits + 2, 1, 1, 1, 1},
        {static_cast<const lh_byte_t *>(lh_null), 0, 0, 0, 1},
    };
    static const lh_byte_t advances[] = {3, 4, 2};
    static const lh_s32_t tops[] = {-2, -2, 0};
    static const lh_ui_font_range_t ranges[] = {
        {'A', 3U, 0U},
    };
    static lh_ui_font_t font;
    static bool ready = false;
    if (!ready)
    {
        lh_ui_font_init(lh_addr_of(font), glyphs, advances, tops, ranges, 2, 2, 2, 1U);
        ready = true;
    }
    return &font;
}

/**
 * @brief The font, filled once — a **second** one, and here is why.
 *
 * The tiny font above has cap height 2 out of a line of 2 and an ascent of 2, so
 * all three of its metrics are the same number and it cannot tell any two rules
 * apart. A rule that says "the baseline is the cap line" and a rule that says "the
 * baseline is where the tallest letter stands" agree on it exactly and disagree
 * nowhere, which is how a test suite ends up green over a wrong answer.
 *
 * So this one has the relation a real font has (Roboto 16: line 22, ascent 17, cap
 * 12, and letters that rise above the cap line), all three different:
 *
 * - line 8, ascent 6, cap 4;
 * - 'f' 4 x 4, top -4: a capital's worth of height, ink 2..6;
 * - 'e' 2 x 2, top -2: x-height, ink 4..6;
 * - 'd' 2 x 5, top -5: an **ascender**, taller than the cap line, ink 1..6.
 *
 * Every glyph's ink ends on row 6 = the ascent, which is what a baseline means, and
 * 'd' is the one that separates the two rules: its baseline is 5 rows below the top
 * of the ink and the cap height is 4. The three codes are consecutive from 'd'
 * because a font holds a run of codes and not a list of letters.
 */
inline const lh_ui_font_t *
cap_font()
{
    static const lh_byte_t f_bits[] = {0x0F, 0x0F, 0x0F, 0x0F};
    static const lh_byte_t e_bits[] = {0x03, 0x03};
    static const lh_byte_t d_bits[] = {0x03, 0x03, 0x03, 0x03, 0x03};
    static const lh_ui_mask_t glyphs[] = {
        {d_bits, 2, 5, 1, 1}, /* 'd' */
        {e_bits, 2, 2, 1, 1}, /* 'e' */
        {f_bits, 4, 4, 1, 1}, /* 'f' */
    };
    static const lh_byte_t advances[] = {5, 5, 6};
    static const lh_s32_t tops[] = {-5, -2, -4};
    static const lh_ui_font_range_t ranges[] = {
        {'d', 3U, 0U},
    };
    static lh_ui_font_t font;
    static bool ready = false;
    if (!ready)
    {
        lh_ui_font_init(lh_addr_of(font), glyphs, advances, tops, ranges, 8, 6, 4, 1U);
        ready = true;
    }
    return &font;
}

/**
 * @brief A font whose coverage has a **hole** in it: 'A'..'C' and 'X'..'Z'.
 *
 * Two runs, six glyphs, and the twenty letters between them absent -- the shape a
 * font has the moment it covers Latin and Cyrillic, and the one thing a single
 * `first` and `count` could not express at all.
 *
 * The second run starts at table index 3, so 'X' is glyph 3 and its advance is the
 * fourth one. A lookup that went on answering `code - first` would say 23 and read
 * off the end of a six-entry table, which is the bug this font exists to catch.
 */
inline const lh_ui_font_t *
split_font()
{
    static const lh_byte_t bits[] = {0xC0, 0xC0, 0x80, 0x40, 0x40, 0x40};
    static const lh_ui_mask_t glyphs[] = {
        {bits + 0, 2, 2, 1, 1}, /* 'A' */
        {bits + 2, 1, 1, 1, 1}, /* 'B' */
        {bits + 3, 1, 1, 1, 1}, /* 'C' */
        {bits + 4, 1, 2, 1, 1}, /* 'X' */
        {bits + 5, 1, 2, 1, 1}, /* 'Y' */
        {bits + 6, 1, 2, 1, 1}, /* 'Z' */
    };
    static const lh_byte_t advances[] = {3, 4, 2, 7, 7, 8};
    static const lh_s32_t tops[] = {-2, -2, -2, -4, -4, -4};
    static const lh_ui_font_range_t ranges[] = {
        {'A', 3U, 0U},
        {'X', 3U, 3U},
    };
    static lh_ui_font_t font;
    static bool ready = false;
    if (!ready)
    {
        lh_ui_font_init(lh_addr_of(font), glyphs, advances, tops, ranges, 6, 5, 4, 2U);
        ready = true;
    }
    return &font;
}

} // namespace lh_test

#endif /* LH_TEST_UI_TINY_FONT_H */