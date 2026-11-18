/**
 * @file tiny_font.h
 * @brief Test helper: a gx font of two glyphs, small enough to check by pixel.
 *
 * `lh_test::gx_tiny_font` holds a baked table and the gx font over it:
 *
 * - `A` (U+0041): a 2 by 2 solid square, advance 3, its top 2 above the
 *   baseline;
 * - `Ж` (U+0416): a single solid pixel, advance 2, its top 1 above the
 *   baseline — a code point past ASCII, for the UTF-8 path.
 *
 * The line is 3 high, the ascent 2, the cap height 2. Any other code point
 * has no glyph. The record must not move once built: the font points at the
 * table.
 */

#ifndef LH_TEST_GX_TINY_FONT_H
#define LH_TEST_GX_TINY_FONT_H

#include <lh/core/font.h>
#include <lh/core/font/range/initializer.h>
#include <lh/core/pixel/mask/initializer.h>
#include <lh/gx/font/baked.h>

namespace lh_test
{

struct gx_tiny_font
{
    lh_font_t table;
    lh_gx_font_t font;

    gx_tiny_font()
    {
        static const lh_byte_t a_bits[4] = {255U, 255U, 255U, 255U};
        static const lh_byte_t zh_bits[1] = {255U};
        static const lh_pixel_mask_t glyphs[2] = {lh_pixel_mask_initializer(a_bits, 2, 2, 2, 8),
                                                  lh_pixel_mask_initializer(zh_bits, 1, 1, 1, 8)};
        static const lh_byte_t advances[2] = {3U, 2U};
        static const lh_s32_t tops[2] = {-2, -1};
        static const lh_font_range_t ranges[2] = {lh_font_range_initializer(0x41U, 1U, 0U),
                                                  lh_font_range_initializer(0x416U, 1U, 1U)};

        lh_font_init(&table, glyphs, advances, tops, ranges, 3, 2, 2, 2U);
        lh_gx_font_baked_init(&font, &table);
    }

    gx_tiny_font(const gx_tiny_font &) = delete;
    gx_tiny_font &
    operator=(const gx_tiny_font &) = delete;
};

} // namespace lh_test

#endif // LH_TEST_GX_TINY_FONT_H
