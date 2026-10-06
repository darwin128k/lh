#include <gtest/gtest.h>

#include <lh/test/ui/tiny_font.h>

#include <lh/config.h>
#include <lh/null.h>
#include <lh/ui/font.h>
#include <lh/util/addr.h>

#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO
#    include <lh/ui/font/roboto.h>
#endif

namespace
{

using lh_test::tiny_font;

TEST(ui_font, init_keeps_the_cell_layout)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_get_cell_width(font), 2);
    EXPECT_EQ(lh_ui_font_get_line_height(font), 2);
    EXPECT_EQ(lh_ui_font_get_first(font), static_cast<lh_u32_t>('A'));
    EXPECT_EQ(lh_ui_font_get_count(font), 2U);
    EXPECT_EQ(lh_ui_font_get_cell_bytes(font), 2U);
}

TEST(ui_font, has_code_covers_the_run_only)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_has_code(font, 'A'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, 'B'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, 'C'), lh_bool_false);
    EXPECT_EQ(lh_ui_font_has_code(font, '@'), lh_bool_false);
    EXPECT_EQ(lh_ui_font_get_index(font, 'B'), 1U);
}

TEST(ui_font, advance_is_zero_without_a_glyph)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_get_advance(font, 'A'), 3);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'B'), 4);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'Z'), 0);
}

TEST(ui_font, glyph_is_the_mask_of_its_cell)
{
    const lh_ui_font_t *font = tiny_font();
    lh_ui_mask_t mask;

    ASSERT_EQ(lh_ui_font_get_glyph(font, 'B', lh_addr_of(mask)), lh_bool_true);
    EXPECT_EQ(lh_ui_mask_get_width(lh_addr_of(mask)), 2);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 0), 255);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 1, 0), 0);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 1), 0);
    EXPECT_EQ(lh_ui_font_get_glyph(font, 'Z', lh_addr_of(mask)), lh_bool_false);
}

#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO

TEST(ui_font, roboto_is_the_default_and_has_its_metrics)
{
    const lh_ui_font_t *font = lh_ui_font_get_default();
    lh_ui_mask_t glyph;

    ASSERT_EQ(font, lh_addr_of(lh_ui_font_roboto));
    EXPECT_EQ(lh_ui_font_get_line_height(font), 22);
    EXPECT_EQ(lh_ui_font_get_cell_width(font), 14);
    EXPECT_EQ(lh_ui_font_get_first(font), 32U);
    EXPECT_EQ(lh_ui_font_get_count(font), 95U);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'A'), 10);
    EXPECT_EQ(lh_ui_font_get_advance(font, ' '), 4);
    ASSERT_EQ(lh_ui_font_get_glyph(font, '!', lh_addr_of(glyph)), lh_bool_true);
    /* '!' has ink in its stem: some pixel of the cell is covered. */
    int ink = 0;
    for (int y = 0; y < 22; ++y)
    {
        for (int x = 0; x < 14; ++x)
        {
            ink += lh_ui_mask_get_coverage(lh_addr_of(glyph), x, y) > 0 ? 1 : 0;
        }
    }
    EXPECT_GT(ink, 0);
}

#else

TEST(ui_font, no_default_without_roboto)
{
    EXPECT_TRUE(lh_null_eq(lh_ui_font_get_default()));
}

#endif

} // namespace
