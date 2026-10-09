/**
 * @file tiny_font.h
 * @brief Test helper: a three-glyph font with known pixels, tops and advances.
 *
 * Codes 'A', 'B', 'C' at 1 bpp. Line height 2, ascent 2 (baseline at the
 * bottom of the line), cap height 2 (the same two rows 'A' rises). 'A' is a
 * full 2 x 2 square, advance 3, top -2. 'B' is
 * a single top-left pixel cropped to 1 x 1, advance 4, top -2. 'C' has no
 * ink: a zero-size mask, advance 2, top 0. Every other code has no glyph.
 */

#ifndef LH_TEST_UI_TINY_FONT_H
#define LH_TEST_UI_TINY_FONT_H

#include <lh/byte.h>
#include <lh/null.h>
#include <lh/ui/font.h>
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
    static lh_ui_font_t font;
    static bool ready = false;
    if (!ready)
    {
        lh_ui_font_init(lh_addr_of(font), glyphs, advances, tops, 2, 2, 2, 'A', 3U);
        ready = true;
    }
    return &font;
}

} // namespace lh_test

#endif /* LH_TEST_UI_TINY_FONT_H */
