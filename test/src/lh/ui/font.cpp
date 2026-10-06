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

TEST(ui_font, init_keeps_the_cropped_layout)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_get_line_height(font), 2);
    EXPECT_EQ(lh_ui_font_get_ascent(font), 2);
    EXPECT_EQ(lh_ui_font_get_first(font), static_cast<lh_u32_t>('A'));
    EXPECT_EQ(lh_ui_font_get_count(font), 3U);
}

TEST(ui_font, has_code_covers_the_run_only)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_has_code(font, 'A'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, 'B'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, 'C'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, 'D'), lh_bool_false);
    EXPECT_EQ(lh_ui_font_has_code(font, '@'), lh_bool_false);
    EXPECT_EQ(lh_ui_font_get_index(font, 'B'), 1U);
}

TEST(ui_font, advance_is_zero_without_a_glyph)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_get_advance(font, 'A'), 3);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'B'), 4);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'C'), 2);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'Z'), 0);
}

TEST(ui_font, top_is_negative_for_ink_and_zero_without_a_glyph)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_get_top(font, 'A'), -2);
    EXPECT_EQ(lh_ui_font_get_top(font, 'B'), -2);
    EXPECT_EQ(lh_ui_font_get_top(font, 'C'), 0);
    EXPECT_EQ(lh_ui_font_get_top(font, 'Z'), 0);
}

TEST(ui_font, glyph_is_cropped_to_its_ink)
{
    const lh_ui_font_t *font = tiny_font();
    lh_ui_mask_t mask;

    ASSERT_EQ(lh_ui_font_get_glyph(font, 'A', lh_addr_of(mask)), lh_bool_true);
    EXPECT_EQ(lh_ui_mask_get_width(lh_addr_of(mask)), 2);
    EXPECT_EQ(lh_ui_mask_get_height(lh_addr_of(mask)), 2);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 0), 255);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 1, 1), 255);

    ASSERT_EQ(lh_ui_font_get_glyph(font, 'B', lh_addr_of(mask)), lh_bool_true);
    EXPECT_EQ(lh_ui_mask_get_width(lh_addr_of(mask)), 1);
    EXPECT_EQ(lh_ui_mask_get_height(lh_addr_of(mask)), 1);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 0), 255);

    ASSERT_EQ(lh_ui_font_get_glyph(font, 'C', lh_addr_of(mask)), lh_bool_true);
    EXPECT_EQ(lh_ui_mask_get_width(lh_addr_of(mask)), 0);
    EXPECT_EQ(lh_ui_mask_get_height(lh_addr_of(mask)), 0);
    EXPECT_TRUE(lh_null_eq(mask.bits));

    EXPECT_EQ(lh_ui_font_get_glyph(font, 'Z', lh_addr_of(mask)), lh_bool_false);
}

#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO

TEST(ui_font, roboto_is_the_default_and_has_its_metrics)
{
    const lh_ui_font_t *font = lh_ui_font_get_default();
    lh_ui_mask_t glyph;

    ASSERT_EQ(font, lh_addr_of(lh_ui_font_roboto));
    EXPECT_EQ(lh_ui_font_get_line_height(font), 22);
    EXPECT_EQ(lh_ui_font_get_ascent(font), 17);
    EXPECT_EQ(lh_ui_font_get_first(font), 32U);
    EXPECT_EQ(lh_ui_font_get_count(font), 95U);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'A'), 10);
    EXPECT_EQ(lh_ui_font_get_advance(font, ' '), 4);
    EXPECT_LT(lh_ui_font_get_top(font, 'A'), 0);
    ASSERT_EQ(lh_ui_font_get_glyph(font, ' ', lh_addr_of(glyph)), lh_bool_true);
    EXPECT_EQ(lh_ui_mask_get_width(lh_addr_of(glyph)), 0);
    EXPECT_EQ(lh_ui_mask_get_height(lh_addr_of(glyph)), 0);
    ASSERT_EQ(lh_ui_font_get_glyph(font, '!', lh_addr_of(glyph)), lh_bool_true);
    EXPECT_GT(lh_ui_mask_get_width(lh_addr_of(glyph)), 0);
    EXPECT_GT(lh_ui_mask_get_height(lh_addr_of(glyph)), 0);
    EXPECT_LT(lh_ui_mask_get_height(lh_addr_of(glyph)), 22);
    int ink = 0;
    for (int y = 0; y < lh_ui_mask_get_height(lh_addr_of(glyph)); ++y)
    {
        for (int x = 0; x < lh_ui_mask_get_width(lh_addr_of(glyph)); ++x)
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
