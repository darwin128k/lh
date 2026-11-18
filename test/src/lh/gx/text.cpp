/**
 * @file text.cpp
 * @brief Tests for `lh/gx/glyph.h`, `lh/gx/font.h`, `lh/gx/font/baked.h` and
 *        `lh/gx/text.h`, and the UTF-8 decoder and the font they rest on.
 *
 * A tiny baked font made by hand — an "A" two pixels square, a "Ж" of one pixel —
 * so every pixel the text draws can be checked: where the baseline is, where each
 * glyph's top lands, how far the pen moves, and that a code with no glyph is
 * skipped without moving it.
 */

#include <gtest/gtest.h>

#include <lh/core/str/utf8.h>
#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/font/baked.h>
#include <lh/gx/render.h>
#include <lh/gx/text.h>
#include <lh/test/gx/tiny_font.h>

namespace
{

using Tiny = lh_test::gx_tiny_font;

TEST(str_utf8, the_decoder_reads_one_code_point_at_a_time)
{
    const lh_char_t *cursor = "A\xD0\x96\xE2\x80\x94";

    EXPECT_EQ(lh_str_utf8_get_next_code(&cursor), 0x41U);
    EXPECT_EQ(lh_str_utf8_get_next_code(&cursor), 0x416U) << "Zhe, two bytes";
    EXPECT_EQ(lh_str_utf8_get_next_code(&cursor), 0x2014U) << "em dash, three bytes";
    EXPECT_EQ(lh_str_utf8_get_next_code(&cursor), 0U);
    const lh_char_t *broken = "\x96Z";

    EXPECT_EQ(lh_str_utf8_get_next_code(&broken), 0x96U) << "a stray continuation byte is itself";
    EXPECT_EQ(lh_str_utf8_get_next_code(&broken), 0x5AU);
}

TEST(gx_font, a_baked_table_answers_through_the_contract)
{
    Tiny tiny;

    EXPECT_EQ(lh_gx_font_get_ascent(&tiny.font), 2);
    EXPECT_EQ(lh_gx_font_get_line_height(&tiny.font), 3);
    lh_gx_glyph_t glyph;
    ASSERT_EQ(lh_gx_font_get_glyph(&tiny.font, 0x416U, &glyph), lh_bool_true);
    EXPECT_EQ(lh_gx_glyph_get_advance(&glyph), 2);
    EXPECT_EQ(lh_gx_glyph_get_top(&glyph), -1);
    EXPECT_EQ(lh_pixel_mask_get_width(lh_gx_glyph_get_mask(&glyph)), 1);
    EXPECT_EQ(lh_gx_font_get_glyph(&tiny.font, 0x42U, &glyph), lh_bool_false) << "no B";
}

TEST(gx_text, the_width_is_the_advances_up_to_the_line_end)
{
    Tiny tiny;

    EXPECT_EQ(lh_gx_text_get_width(&tiny.font, "A\xD0\x96"
                                               "A"),
              8);
    EXPECT_EQ(lh_gx_text_get_width(&tiny.font, "A?A"), 6) << "no glyph, no advance";
    EXPECT_EQ(lh_gx_text_get_width(&tiny.font, "A\nA"), 3) << "one line";
}

/* A 12 by 4 screen, black, with a white line of text at (1, 0). */
TEST(gx_text, the_glyphs_land_on_the_baseline_and_the_pen_moves)
{
    Tiny tiny;
    lh_u32_t words[12 * 4] = {};
    static const lh_char_t text[] = "A\xD0\x96?A";

    lh_memory_bounds_t bits;
    lh_memory_bounds_init(&bits, words, sizeof(words));
    lh_gx_pixmap_t pixmap;
    lh_gx_pixmap_init(&pixmap, &bits, 12U, 4U, lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_t sw;
    lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
    lh_gx_canvas_t canvas;
    lh_gx_backend_sw_canvas_init(&canvas);
    lh_gx_render_t render;
    lh_gx_render_init(&render, &canvas, &sw, 12, 4);
    lh_gx_style_t back_style;
    lh_gx_style_init(&back_style, 0xFF000000U, 0U, 0, 0, 0U, nullptr);
    lh_gx_style_t ink;
    lh_gx_style_init(&ink, 0U, 0U, 0, 0, 0xFFFFFFFFU, &tiny.font);
    lh_gx_entity_t back;
    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 12, 4);
    lh_gx_entity_t line;
    lh_gx_entity_init(&line, &lh_gx_text_class, &ink, 1, 0, 10, 3);
    lh_gx_entity_set_content(&line, const_cast<lh_char_t *>(text));
    lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
    lh_gx_entity_attach(&back, &line);
    lh_gx_damage_t drawn;
    lh_gx_render_draw(&render, &drawn);

    const auto at = [&](int x, int y) { return words[y * 12 + x]; };
    /* Baseline at 0 + ascent 2. "A": top -2, so rows 0 and 1, columns 1 and 2. */
    EXPECT_EQ(at(1, 0), 0xFFFFFFFFU);
    EXPECT_EQ(at(2, 1), 0xFFFFFFFFU);
    EXPECT_EQ(at(3, 0), 0xFF000000U) << "the A's advance is 3, its ink 2";
    /* "Ж" at pen 4: top -1, so row 1. */
    EXPECT_EQ(at(4, 1), 0xFFFFFFFFU);
    EXPECT_EQ(at(4, 0), 0xFF000000U);
    /* "?" has no glyph: skipped, the pen stays at 6. "A" at 6..7. */
    EXPECT_EQ(at(6, 0), 0xFFFFFFFFU);
    EXPECT_EQ(at(7, 1), 0xFFFFFFFFU);
    EXPECT_EQ(at(8, 0), 0xFF000000U);
    EXPECT_EQ(at(1, 2), 0xFF000000U) << "below the ink";
}

} // namespace
