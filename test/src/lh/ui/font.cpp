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

using lh_test::split_font;
using lh_test::tiny_font;

TEST(ui_font, init_keeps_the_cropped_layout)
{
    const lh_ui_font_t *font = tiny_font();

    EXPECT_EQ(lh_ui_font_get_line_height(font), 2);
    EXPECT_EQ(lh_ui_font_get_ascent(font), 2);
    EXPECT_EQ(lh_ui_font_get_first(font), static_cast<lh_u32_t>('A'));
    EXPECT_EQ(lh_ui_font_get_glyph_count(font), 3U);
    EXPECT_EQ(lh_ui_font_get_range_count(font), 1U);
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

TEST(ui_font, a_font_has_a_hole_where_its_coverage_stops_and_starts_again)
{
    /* The case one run cannot express. 'A'..'C' and 'X'..'Z' with the twenty letters
       between them absent is what a font looks like the moment it covers two scripts,
       and Cyrillic is a thousand codes above the space -- which is why this is the shape
       the format had to be able to hold before anything else was asked of it.

       The letter after the hole is the half of it that matters: 'X' is the **fourth**
       glyph of the tables, not the twenty-fourth, and a lookup still answering
       `code - first` would read off the end of a six-entry array. */
    const lh_ui_font_t *font = split_font();

    EXPECT_EQ(lh_ui_font_get_range_count(font), 2U);
    EXPECT_EQ(lh_ui_font_get_glyph_count(font), 6U);
    EXPECT_EQ(lh_ui_font_get_first(font), static_cast<lh_u32_t>('A'));

    EXPECT_EQ(lh_ui_font_has_code(font, 'C'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, 'D'), lh_bool_false);
    EXPECT_EQ(lh_ui_font_has_code(font, 'W'), lh_bool_false);
    EXPECT_EQ(lh_ui_font_has_code(font, 'X'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, 'Z'), lh_bool_true);
    EXPECT_EQ(lh_ui_font_has_code(font, '['), lh_bool_false);

    EXPECT_EQ(lh_ui_font_get_index(font, 'A'), 0U);
    EXPECT_EQ(lh_ui_font_get_index(font, 'X'), 3U);
    EXPECT_EQ(lh_ui_font_get_index(font, 'Z'), 5U);
}

TEST(ui_font, metrics_come_from_the_run_the_code_is_in)
{
    /* Each run carries its own base, and a wrong base is not a wrong pixel: it is a
       pixel from a different letter, with the advance of another one beside it. The
       advances here are 3/4/2 for the first run and 7/7/8 for the second, and the tops
       are -2 and -4, so reading the second run off the first gives a mask, an advance
       and a top that all belong to 'A'..'C' while the letter on screen is 'X'. */
    const lh_ui_font_t *font = split_font();

    EXPECT_EQ(lh_ui_font_get_advance(font, 'X'), 7);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'Z'), 8);
    EXPECT_EQ(lh_ui_font_get_top(font, 'X'), -4);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'A'), 3);
    EXPECT_EQ(lh_ui_font_get_top(font, 'A'), -2);

    /* A code in no run has no metric, and says so with the same zero an inkless
       glyph does not use for the other reason. */
    EXPECT_EQ(lh_ui_font_get_advance(font, 'W'), 0);
    EXPECT_EQ(lh_ui_font_get_top(font, 'W'), 0);
    EXPECT_EQ(lh_ui_font_get_advance(font, 'Q'), 0);
}

TEST(ui_font, a_run_reports_its_own_bounds)
{
    lh_ui_font_range_t run;

    lh_ui_font_range_init(lh_addr_of(run), 0x0400U, 96U, 100U);
    EXPECT_EQ(lh_ui_font_range_get_first(lh_addr_of(run)), 0x0400U);
    EXPECT_EQ(lh_ui_font_range_get_length(lh_addr_of(run)), 96U);
    EXPECT_EQ(lh_ui_font_range_get_base(lh_addr_of(run)), 100U);
    EXPECT_EQ(lh_ui_font_range_get_last(lh_addr_of(run)), 0x045FU);

    EXPECT_EQ(lh_ui_font_range_has(lh_addr_of(run), 0x0400U), lh_bool_true);
    EXPECT_EQ(lh_ui_font_range_has(lh_addr_of(run), 0x045FU), lh_bool_true);
    EXPECT_EQ(lh_ui_font_range_has(lh_addr_of(run), 0x03FFU), lh_bool_false);
    EXPECT_EQ(lh_ui_font_range_has(lh_addr_of(run), 0x0460U), lh_bool_false);

    /* An empty run has no last code, and saying so as `first - 1` is what keeps
       ::lh_ui_font_range_has from claiming the whole code space for a run that covers
       nothing -- `first + length - 1` wraps at the top of `lh_u32_t` and answers yes
       to everything. */
    lh_ui_font_range_init(lh_addr_of(run), 0x0400U, 0U, 0U);
    EXPECT_EQ(lh_ui_font_range_get_last(lh_addr_of(run)), 0x03FFU);
    EXPECT_EQ(lh_ui_font_range_has(lh_addr_of(run), 0x0400U), lh_bool_false);

    /* And the same at the very top of the space. */
    lh_ui_font_range_init(lh_addr_of(run), 0xFFFFFFFFU, 4U, 0U);
    EXPECT_EQ(lh_ui_font_range_has(lh_addr_of(run), 0xFFFFFFFFU), lh_bool_true);
    EXPECT_EQ(lh_ui_font_range_has(lh_addr_of(run), 0xFFFFFFFEU), lh_bool_false);
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
    EXPECT_EQ(lh_ui_font_get_glyph_count(font), 224U);
    EXPECT_EQ(lh_ui_font_get_range_count(font), 7U);
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

TEST(ui_font, roboto_draws_russian_and_its_latin_is_untouched)
{
    /* The reason the coverage is a list of runs and not one: Cyrillic is U+0400 and the
       space is U+0020, and a format that could only say "from here, this many" would
       have to carry every code point between them -- a thousand slots, twenty-four
       bytes a mask, for characters that cannot be typed.

       So the Cyrillic is checked where it actually is (a run of its own, 1000 codes up),
       and the two codes Roboto has no glyph for are checked as **absent** rather than as
       a hole with a zero advance in it: `scripts/font.py` drops them and the runs close
       either side, so U+00AD is not a code this font answers for. */
    const lh_ui_font_t *font = lh_ui_font_get_default();
    lh_ui_mask_t glyph;

    EXPECT_EQ(lh_ui_font_has_code(font, 0x0410U), lh_bool_true); /* А */
    EXPECT_EQ(lh_ui_font_has_code(font, 0x044FU), lh_bool_true); /* я */
    EXPECT_EQ(lh_ui_font_has_code(font, 0x0401U), lh_bool_true); /* Ё */
    EXPECT_EQ(lh_ui_font_has_code(font, 0x0451U), lh_bool_true); /* ё */
    EXPECT_EQ(lh_ui_font_has_code(font, 0x00ABU), lh_bool_true); /* « */
    EXPECT_EQ(lh_ui_font_has_code(font, 0x2116U), lh_bool_true); /* № */

    EXPECT_EQ(lh_ui_font_has_code(font, 0x00ADU), lh_bool_false); /* soft hyphen */
    EXPECT_EQ(lh_ui_font_has_code(font, 0x00B8U), lh_bool_false); /* spacing diaeresis */

    /* Cyrillic is in a later run, so its index is the base of that run and not
       `code - 32`. */
    EXPECT_GT(lh_ui_font_get_index(font, 0x0410U), static_cast<lh_u32_t>(95));
    EXPECT_EQ(lh_ui_font_get_index(font, 0x0411U), lh_ui_font_get_index(font, 0x0410U) + 1U);

    ASSERT_EQ(lh_ui_font_get_glyph(font, 0x0416U, lh_addr_of(glyph)), lh_bool_true);
    EXPECT_GT(lh_ui_mask_get_width(lh_addr_of(glyph)), 0);
    EXPECT_GT(lh_ui_mask_get_height(lh_addr_of(glyph)), 0);
    EXPECT_GT(lh_ui_font_get_advance(font, 0x0416U), 0);
    EXPECT_LT(lh_ui_font_get_top(font, 0x0416U), 0);

    /* And the Latin is where it was: same advances, same tops, same cap line. The cap
       height is the row every caption in the window is centred on, so a font that grew
       by a thousand codes must not have moved it by one. */
    EXPECT_EQ(lh_ui_font_get_advance(font, 'A'), 10);
    EXPECT_EQ(lh_ui_font_get_top(font, 'A'), -12);
    EXPECT_EQ(lh_ui_font_get_top(font, ' '), 0);
    EXPECT_EQ(lh_ui_font_get_cap_height(font), 12);
    EXPECT_EQ(lh_ui_font_get_line_height(font), 22);
    EXPECT_EQ(lh_ui_font_get_ascent(font), 17);
}

#else

TEST(ui_font, no_default_without_roboto)
{
    EXPECT_TRUE(lh_null_eq(lh_ui_font_get_default()));
}

#endif

} // namespace
