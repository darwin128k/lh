/**
 * @file sw.cpp
 * @brief Tests for `lh/gx/canvas.h` and `lh/gx/backend/sw.h`.
 *
 * The canvas is a table and keeps nothing; the software backend fills it and
 * draws into a pixmap the test owns. What is checked: nothing is drawn outside
 * the clip or the target, a strip buffer with its origin moved down the screen
 * draws the same pixels in its own rows, and RGB565 — swapped or not — comes out
 * as the colour narrowed.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>

namespace
{

/* A pixmap over the test's own words. */
template <typename Word, int W, int H> struct Target
{
    Word words[W * H];
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;

    explicit Target(lh_gx_pixmap_format_t format)
    {
        for (auto &w : words)
        {
            w = 0;
        }
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, W, H, format);
    }

    Word
    at(int x, int y) const
    {
        return words[y * W + x];
    }
};

lh_gx_rect_t
rect(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w, lh_gx_scalar_t h)
{
    lh_gx_rect_t r;

    lh_gx_rect_init(&r, x, y, w, h);
    return r;
}

TEST(gx_canvas, a_new_table_is_empty_and_needs_fill_rect)
{
    lh_gx_canvas_t table;

    lh_gx_canvas_init(&table);
    EXPECT_TRUE(lh_gx_canvas_get_fill_rect(&table) == nullptr);
    EXPECT_TRUE(lh_gx_canvas_get_fill_mask(&table) == nullptr);
    EXPECT_EQ(lh_gx_canvas_is_complete(&table), lh_bool_false);

    lh_gx_backend_sw_canvas_init(&table);
    EXPECT_TRUE(lh_gx_canvas_get_fill_rect(&table) == lh_gx_backend_sw_fill_rect);
    EXPECT_TRUE(lh_gx_canvas_get_fill_mask(&table) == lh_gx_backend_sw_fill_mask);
    EXPECT_EQ(lh_gx_canvas_is_complete(&table), lh_bool_true);
}

TEST(gx_backend_sw, a_fill_stays_inside_the_clip_and_the_target)
{
    Target<lh_u32_t, 8, 4> target(lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;

    lh_gx_backend_sw_init(&sw, &target.pixmap, 0, 0);
    lh_gx_backend_sw_canvas_init(&canvas);
    const lh_gx_rect_t clip = rect(2, 0, 4, 4);
    const lh_gx_rect_t fill = rect(-5, 1, 100, 2);

    lh_gx_canvas_get_fill_rect (&canvas)(&sw, &clip, &fill, 0xFF112233U);

    EXPECT_EQ(target.at(1, 1), 0U) << "left of the clip";
    EXPECT_EQ(target.at(2, 1), 0xFF112233U);
    EXPECT_EQ(target.at(5, 2), 0xFF112233U);
    EXPECT_EQ(target.at(6, 2), 0U) << "right of the clip";
    EXPECT_EQ(target.at(3, 0), 0U) << "above the rectangle";
    EXPECT_EQ(target.at(3, 3), 0U) << "below the rectangle";
}

/* The renderer moves a strip down the screen by its origin; the canvas never knows. */
TEST(gx_backend_sw, a_strip_draws_its_own_rows_of_the_screen)
{
    Target<lh_u32_t, 8, 2> strip(lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_t sw;

    lh_gx_backend_sw_init(&sw, &strip.pixmap, 0, 10);
    const lh_gx_rect_t screen = rect(0, 0, 8, 100);
    const lh_gx_rect_t card = rect(1, 11, 2, 5);

    lh_gx_backend_sw_fill_rect(&sw, &screen, &card, 0xFFFFFFFFU);

    EXPECT_EQ(strip.at(1, 0), 0U) << "screen row 10 is above the card";
    EXPECT_EQ(strip.at(1, 1), 0xFFFFFFFFU) << "screen row 11";
    EXPECT_EQ(strip.at(3, 1), 0U);
}

TEST(gx_backend_sw, rgb565_is_the_colour_narrowed_swapped_or_not)
{
    Target<lh_u16_t, 4, 1> plain(lh_gx_pixmap_format_rgb565);
    Target<lh_u16_t, 4, 1> swapped(lh_gx_pixmap_format_rgb565_swapped);
    lh_gx_backend_sw_t sw;
    const lh_gx_rect_t all = rect(0, 0, 4, 1);

    lh_gx_backend_sw_init(&sw, &plain.pixmap, 0, 0);
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, 0xFFFF0000U);
    lh_gx_backend_sw_init(&sw, &swapped.pixmap, 0, 0);
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, 0xFFFF0000U);

    EXPECT_EQ(plain.at(0, 0), 0xF800U);
    EXPECT_EQ(swapped.at(3, 0), 0x00F8U);
}

TEST(gx_backend_sw, a_translucent_fill_is_laid_over)
{
    Target<lh_u32_t, 2, 1> target(lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_t sw;
    const lh_gx_rect_t all = rect(0, 0, 2, 1);

    lh_gx_backend_sw_init(&sw, &target.pixmap, 0, 0);
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, 0xFF000000U);
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, 0x80FFFFFFU);
    EXPECT_EQ(target.at(1, 0), 0xFF808080U);
}

TEST(gx_backend_sw, a_mask_is_laid_through_and_clipped)
{
    Target<lh_u32_t, 6, 3> target(lh_gx_pixmap_format_argb8888);
    Target<lh_byte_t, 3, 2> mask(lh_gx_pixmap_format_a8);
    lh_gx_backend_sw_t sw;
    lh_gx_point_t at;

    mask.words[0] = 255U;
    mask.words[1] = 128U;
    mask.words[2] = 255U;
    mask.words[3 + 0] = 255U;
    lh_gx_point_init(&at, 2, 1);
    lh_gx_backend_sw_init(&sw, &target.pixmap, 0, 0);
    const lh_gx_rect_t clip = rect(0, 0, 4, 3);

    lh_gx_backend_sw_fill_mask(&sw, &clip, &mask.pixmap, at, 0xFFFFFFFFU);

    EXPECT_EQ(target.at(2, 1), 0xFFFFFFFFU) << "mask (0, 0)";
    EXPECT_EQ(target.at(3, 1), 0x80FFFFFFU) << "mask (1, 0): half white over clear";
    EXPECT_EQ(target.at(4, 1), 0U) << "mask (2, 0) is outside the clip";
    EXPECT_EQ(target.at(2, 2), 0xFFFFFFFFU) << "mask (0, 1)";
    EXPECT_EQ(target.at(3, 2), 0U) << "no coverage";
}

} // namespace
