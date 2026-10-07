#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>

namespace
{

/* 4x3 pixels in rows of 5 (one spare word per row). */
struct pixmap_fixture
{
    lh_u32_t words[3 * 5];
    lh_ui_pixmap_t pixmap;

    explicit pixmap_fixture(lh_u32_t fill)
    {
        for (lh_u32_t &w : words)
        {
            w = fill;
        }
        lh_ui_pixmap_init(lh_addr_of(pixmap), words, 4, 3, 5);
    }
};

lh_ui_color_t
color_of(int r, int g, int b, int a)
{
    lh_ui_color_t c;
    lh_ui_color_init(lh_addr_of(c), static_cast<lh_u8_t>(r), static_cast<lh_u8_t>(g), static_cast<lh_u8_t>(b),
                     static_cast<lh_u8_t>(a));
    return c;
}

} // namespace

TEST(ui_pixmap, init_keeps_size_and_rows_follow_the_stride)
{
    pixmap_fixture f(0u);

    EXPECT_EQ(lh_ui_pixmap_get_width(lh_addr_of(f.pixmap)), 4);
    EXPECT_EQ(lh_ui_pixmap_get_height(lh_addr_of(f.pixmap)), 3);
    EXPECT_EQ(lh_ui_pixmap_get_row(lh_addr_of(f.pixmap), 2), f.words + 10);
    const lh_ui_rect_t bounds = lh_ui_pixmap_get_bounds(lh_addr_of(f.pixmap));
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(bounds))), 4);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(bounds))), 3);
}

TEST(ui_pixmap, set_and_get_pixel_round_trip_through_argb)
{
    pixmap_fixture f(0u);
    const lh_ui_color_t c = color_of(1, 2, 3, 4);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 3, 1, lh_addr_of(c));
    EXPECT_EQ(f.words[1 * 5 + 3], 0x04010203u);
    const lh_ui_color_t back = lh_ui_pixmap_get_pixel(lh_addr_of(f.pixmap), 3, 1);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(back), lh_addr_of(c)));
}

TEST(ui_pixmap, fill_span_stores_an_opaque_color_and_leaves_the_rest)
{
    pixmap_fixture f(0x11111111u);
    const lh_ui_color_t c = color_of(10, 20, 30, 255);

    lh_ui_pixmap_fill_span(lh_addr_of(f.pixmap), 1, 3, 2, lh_addr_of(c));
    EXPECT_EQ(f.words[2 * 5 + 0], 0x11111111u);
    EXPECT_EQ(f.words[2 * 5 + 1], 0xff0a141eu);
    EXPECT_EQ(f.words[2 * 5 + 2], 0xff0a141eu);
    EXPECT_EQ(f.words[2 * 5 + 3], 0x11111111u);
}

TEST(ui_pixmap, fill_span_blends_a_translucent_color)
{
    pixmap_fixture f(0xff000000u);
    const lh_ui_color_t c = color_of(200, 100, 0, 128);
    lh_ui_color_t black = color_of(0, 0, 0, 255);

    lh_ui_pixmap_fill_span(lh_addr_of(f.pixmap), 0, 1, 0, lh_addr_of(c));
    const lh_ui_color_t want = lh_ui_color_over(lh_addr_of(black), lh_addr_of(c));
    const lh_ui_color_t got = lh_ui_pixmap_get_pixel(lh_addr_of(f.pixmap), 0, 0);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(got), lh_addr_of(want)));
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(got)), 255);
}

TEST(ui_pixmap, cover_pixel_scales_alpha_and_skips_zero)
{
    pixmap_fixture f(0u);
    const lh_ui_color_t c = color_of(9, 8, 7, 255);

    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(c), 0);
    EXPECT_EQ(f.words[0], 0u);
    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(c), 100);
    const lh_ui_color_t got = lh_ui_pixmap_get_pixel(lh_addr_of(f.pixmap), 0, 0);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(got)), 100);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(got)), 9);
}

TEST(ui_pixmap, fill_box_and_clear_stay_inside_the_width)
{
    pixmap_fixture f(0x12345678u);
    const lh_ui_color_t c = color_of(0, 0, 0, 255);

    lh_ui_pixmap_fill_box(lh_addr_of(f.pixmap), 1, 1, 3, 3, lh_addr_of(c));
    EXPECT_EQ(f.words[1 * 5 + 1], 0xff000000u);
    EXPECT_EQ(f.words[2 * 5 + 2], 0xff000000u);
    EXPECT_EQ(f.words[0 * 5 + 1], 0x12345678u);

    lh_ui_pixmap_clear(lh_addr_of(f.pixmap), lh_addr_of(c));
    EXPECT_EQ(f.words[2 * 5 + 3], 0xff000000u);
    /* The spare word past the width is not a pixel. */
    EXPECT_EQ(f.words[0 * 5 + 4], 0x12345678u);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(ui_pixmap_death, stride_shorter_than_the_width)
{
    lh_u32_t words[4];
    lh_ui_pixmap_t pixmap;

    LH_EXPECT_DEATH(lh_ui_pixmap_init(lh_addr_of(pixmap), words, 4, 1, 3));
}

#endif
