/**
 * @file tiny_font.h
 * @brief Test helper: a two-glyph 2 x 2 font with known pixels and advances.
 *
 * Codes 'A' and 'B' at 1 bpp, rows of one byte, line height 2. 'A' is a full
 * square with advance 3; 'B' only its top-left pixel, advance 4. Every other
 * code has no glyph. Text, canvas and label tests measure against it.
 */

#ifndef LH_TEST_UI_TINY_FONT_H
#define LH_TEST_UI_TINY_FONT_H

#include <lh/byte.h>
#include <lh/ui/font.h>
#include <lh/util/addr.h>

namespace lh_test
{

/** The font, filled once. */
inline const lh_ui_font_t *
tiny_font()
{
    static const lh_byte_t bits[] = {0xC0, 0xC0, 0x80, 0x00};
    static const lh_byte_t advances[] = {3, 4};
    static lh_ui_font_t font;
    static bool ready = false;
    if (!ready)
    {
        lh_ui_font_init(lh_addr_of(font), bits, advances, 2, 2, 1, 1, 'A', 2U);
        ready = true;
    }
    return &font;
}

} // namespace lh_test

#endif /* LH_TEST_UI_TINY_FONT_H */
