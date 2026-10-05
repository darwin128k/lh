#include <gtest/gtest.h>

#include <lh/ui/font.h>
#include <lh/util/addr.h>

TEST(ui_font, cell_keeps_the_bitmap_and_the_advance)
{
    const lh_byte_t glyphs[] = {0x80, 0x40, 0x20, 0x10};
    const lh_byte_t advances[] = {3};
    lh_ui_font_t font;
    lh_ui_font_init(lh_addr_of(font), glyphs, sizeof glyphs, advances, 1, 4, 4, 1, 1, static_cast<lh_byte_t>('A'));

    EXPECT_EQ(lh_ui_font_get_cell_width(lh_addr_of(font)), 4);
    EXPECT_EQ(lh_ui_font_get_height(lh_addr_of(font)), 4);
    EXPECT_EQ(lh_ui_font_get_bpp(lh_addr_of(font)), 1);
    EXPECT_EQ(lh_ui_font_get_first(lh_addr_of(font)), static_cast<lh_byte_t>('A'));
    EXPECT_EQ(lh_ui_font_get_last(lh_addr_of(font)), static_cast<lh_uint_t>('A'));
    const lh_memory_view_t glyphs_view = lh_ui_font_get_glyphs(lh_addr_of(font));

    EXPECT_EQ(lh_ui_font_get_count(lh_addr_of(font)), 1u);
    EXPECT_EQ(lh_memory_view_get_size(&glyphs_view), sizeof glyphs);
    const lh_memory_view_t glyph = lh_ui_font_get_glyph(lh_addr_of(font), static_cast<lh_uint_t>('A'));
    const lh_memory_view_t missing = lh_ui_font_get_glyph(lh_addr_of(font), static_cast<lh_uint_t>('B'));

    EXPECT_TRUE(lh_ui_font_has_code(lh_addr_of(font), static_cast<lh_uint_t>('A')));
    EXPECT_FALSE(lh_ui_font_has_code(lh_addr_of(font), static_cast<lh_uint_t>('B')));
    EXPECT_EQ(lh_ui_font_get_index(lh_addr_of(font), static_cast<lh_uint_t>('A')), 0u);
    EXPECT_EQ(lh_ui_font_get_cell_bytes(lh_addr_of(font)), 4u);
    EXPECT_TRUE(lh_ui_font_has_pixel(lh_addr_of(font), 0, 0));
    EXPECT_FALSE(lh_ui_font_has_pixel(lh_addr_of(font), 4, 0));
    EXPECT_EQ(lh_memory_view_get_size(&glyph), 4u);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&missing));
    EXPECT_EQ(lh_ui_font_get_advance(lh_addr_of(font), static_cast<lh_uint_t>('A')), 3);
    EXPECT_EQ(lh_ui_font_get_advance(lh_addr_of(font), static_cast<lh_uint_t>('B')), 0);
    EXPECT_EQ(lh_ui_font_get_coverage(lh_addr_of(font), static_cast<lh_uint_t>('A'), 0, 0), 255);
    EXPECT_EQ(lh_ui_font_get_coverage(lh_addr_of(font), static_cast<lh_uint_t>('A'), 1, 0), 0);
    EXPECT_EQ(lh_ui_font_get_coverage(lh_addr_of(font), static_cast<lh_uint_t>('A'), 1, 1), 255);
    EXPECT_EQ(lh_ui_font_get_coverage(lh_addr_of(font), static_cast<lh_uint_t>('A'), 3, 3), 255);
    EXPECT_EQ(lh_ui_font_get_coverage(lh_addr_of(font), static_cast<lh_uint_t>('B'), 0, 0), 0);
}

TEST(ui_font, roboto_is_the_16px_regular)
{
    bool ink = false;

    EXPECT_EQ(lh_ui_font_get_cell_width(lh_addr_of(lh_ui_font_roboto)), 14);
    EXPECT_EQ(lh_ui_font_get_height(lh_addr_of(lh_ui_font_roboto)), 22);
    EXPECT_EQ(lh_ui_font_get_row_bytes(lh_addr_of(lh_ui_font_roboto)), 7);
    EXPECT_EQ(lh_ui_font_get_bpp(lh_addr_of(lh_ui_font_roboto)), 4);
    EXPECT_EQ(lh_ui_font_get_first(lh_addr_of(lh_ui_font_roboto)), 32);
    EXPECT_EQ(lh_ui_font_get_count(lh_addr_of(lh_ui_font_roboto)), 95u);
    EXPECT_EQ(lh_ui_font_get_last(lh_addr_of(lh_ui_font_roboto)), 126u);
    const lh_memory_view_t glyph = lh_ui_font_get_glyph(lh_addr_of(lh_ui_font_roboto), static_cast<lh_uint_t>('A'));

    EXPECT_TRUE(lh_ui_font_has_code(lh_addr_of(lh_ui_font_roboto), static_cast<lh_uint_t>('A')));
    EXPECT_FALSE(lh_ui_font_has_code(lh_addr_of(lh_ui_font_roboto), 31u));
    EXPECT_EQ(lh_ui_font_get_index(lh_addr_of(lh_ui_font_roboto), static_cast<lh_uint_t>('A')), 33u);
    EXPECT_EQ(lh_ui_font_get_cell_bytes(lh_addr_of(lh_ui_font_roboto)), 154u);
    EXPECT_EQ(lh_memory_view_get_size(&glyph), 154u);
    EXPECT_EQ(lh_ui_font_get_advance(lh_addr_of(lh_ui_font_roboto), static_cast<lh_uint_t>(' ')), 4);
    EXPECT_EQ(lh_ui_font_get_advance(lh_addr_of(lh_ui_font_roboto), static_cast<lh_uint_t>('A')), 10);
    EXPECT_EQ(lh_ui_font_get_advance(lh_addr_of(lh_ui_font_roboto), 31u), 0);
    for (lh_math_coord_t y = 0; y < lh_ui_font_get_height(lh_addr_of(lh_ui_font_roboto)); ++y)
        for (lh_math_coord_t x = 0; x < lh_ui_font_get_cell_width(lh_addr_of(lh_ui_font_roboto)); ++x)
            if (lh_ui_font_get_coverage(lh_addr_of(lh_ui_font_roboto), static_cast<lh_uint_t>('A'), x, y) != 0)
                ink = true;
    EXPECT_TRUE(ink);
}
