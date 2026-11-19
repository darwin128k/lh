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

#include <lh/core/pixel/span.h>
#include <lh/gx/backend/sw.h>
#include <lh/test/gx/brush.h>

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

    lh_gx_canvas_get_fill_rect (&canvas)(&sw, &clip, &fill, lh_test::gx_brush(0xFF112233U).get());

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

    lh_gx_backend_sw_fill_rect(&sw, &screen, &card, lh_test::gx_brush(0xFFFFFFFFU).get());

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
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, lh_test::gx_brush(0xFFFF0000U).get());
    lh_gx_backend_sw_init(&sw, &swapped.pixmap, 0, 0);
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, lh_test::gx_brush(0xFFFF0000U).get());

    EXPECT_EQ(plain.at(0, 0), 0xF800U);
    EXPECT_EQ(swapped.at(3, 0), 0x00F8U);
}

TEST(gx_backend_sw, a_translucent_fill_is_laid_over)
{
    Target<lh_u32_t, 2, 1> target(lh_gx_pixmap_format_argb8888);
    lh_gx_backend_sw_t sw;
    const lh_gx_rect_t all = rect(0, 0, 2, 1);

    lh_gx_backend_sw_init(&sw, &target.pixmap, 0, 0);
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, lh_test::gx_brush(0xFF000000U).get());
    lh_gx_backend_sw_fill_rect(&sw, &all, &all, lh_test::gx_brush(0x80FFFFFFU).get());
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

    lh_gx_backend_sw_fill_mask(&sw, &clip, &mask.pixmap, at, lh_test::gx_brush(0xFFFFFFFFU).get());

    EXPECT_EQ(target.at(2, 1), 0xFFFFFFFFU) << "mask (0, 0)";
    EXPECT_EQ(target.at(3, 1), 0x80FFFFFFU) << "mask (1, 0): half white over clear";
    EXPECT_EQ(target.at(4, 1), 0U) << "mask (2, 0) is outside the clip";
    EXPECT_EQ(target.at(2, 2), 0xFFFFFFFFU) << "mask (0, 1)";
    EXPECT_EQ(target.at(3, 2), 0U) << "no coverage";
}

/* A gradient brush colours every pixel of a rectangle, and of a mask, as the brush says
   of that pixel on its own (lh_gx_brush_get_color_at): diagonal, straight down and
   radial, in ARGB and in RGB565, through a target that is a band moved down the
   screen, with the brush's table and without one. */
TEST(gx_backend_sw, a_gradient_is_every_pixel_the_brush_gives_it)
{
    const lh_u32_t stops[2] = {0xFF2050A0U, 0xC0F0E010U};
    lh_pixel_gradient_t gradient;
    lh_pixel_gradient_table_t table;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, stops, lh_null, 2);
    for (int shape = 0; shape < 3; ++shape)
    {
        if (shape == 2)
        {
            lh_pixel_gradient_set_radial(&gradient);
        }
        else
        {
            lh_pixel_gradient_set_linear(&gradient, shape == 0 ? 135 : 180);
        }
        lh_pixel_gradient_table_fill(&table, &gradient);
        for (int own = 0; own < 2; ++own)
        {
            /* The band is rows 10 .. 17 of the screen; the card spans 3 .. 32 x 6 .. 25. */
            Target<lh_u32_t, 40, 8> target(lh_gx_pixmap_format_argb8888);
            Target<lh_u16_t, 40, 8> target16(lh_gx_pixmap_format_rgb565);
            Target<lh_byte_t, 12, 5> mask(lh_gx_pixmap_format_a8);
            lh_gx_backend_sw_t sw;
            lh_gx_backend_sw_t sw16;
            lh_gx_brush_t brush;
            lh_gx_point_t at;

            for (int i = 0; i < 12 * 5; ++i)
            {
                mask.words[i] = static_cast<lh_byte_t>(i * 37);
            }
            lh_gx_brush_init_gradient(&brush, &gradient, own == 0 ? &table : nullptr,
                                      rect(3, 6, 30, 20));
            lh_gx_backend_sw_init(&sw, &target.pixmap, 0, 10);
            lh_gx_backend_sw_init(&sw16, &target16.pixmap, 0, 10);
            const lh_gx_rect_t clip = rect(0, 0, 40, 40);
            const lh_gx_rect_t card = rect(3, 6, 30, 20);

            lh_gx_backend_sw_fill_rect(&sw, &clip, &card, &brush);
            lh_gx_backend_sw_fill_rect(&sw16, &clip, &card, &brush);
            for (int y = 0; y < 8; ++y)
            {
                for (int x = 0; x < 40; ++x)
                {
                    const bool in = x >= 3 && x < 33;
                    const lh_u32_t want = in ? lh_gx_brush_get_color_at(&brush, x, y + 10) : 0U;
                    lh_u16_t want16 = 0U;

                    lh_pixel_span_blend_rgb565(&want16, 1U, want, want >> 24, lh_bool_false);
                    ASSERT_EQ(target.at(x, y), want)
                        << "shape " << shape << " own " << own << " at " << x << "," << y;
                    ASSERT_EQ(target16.at(x, y), want16)
                        << "565, shape " << shape << " at " << x << "," << y;
                }
            }
            Target<lh_u32_t, 40, 8> masked(lh_gx_pixmap_format_argb8888);

            lh_gx_backend_sw_init(&sw, &masked.pixmap, 0, 10);
            lh_gx_point_init(&at, 20, 8);
            lh_gx_backend_sw_fill_mask(&sw, &clip, &mask.pixmap, at, &brush);
            for (int my = 0; my < 5; ++my)
            {
                for (int mx = 0; mx < 12; ++mx)
                {
                    const int x = 20 + mx;
                    const int y = 8 + my - 10;

                    if (y < 0)
                    {
                        continue;
                    }
                    lh_u32_t want = 0U;
                    const lh_u32_t color = lh_gx_brush_get_color_at(&brush, x, y + 10);

                    lh_pixel_span_cover_argb(&want, &mask.words[my * 12 + mx], 1U, color,
                                             color >> 24);
                    ASSERT_EQ(masked.at(x, y), want)
                        << "mask, shape " << shape << " at " << x << "," << y;
                }
            }
        }
    }
}

TEST(gx_backend_sw, the_solid_canvas_draws_a_gradient_as_its_first_colour)
{
    const lh_u32_t stops[2] = {0xFF2050A0U, 0xFFF0E010U};
    Target<lh_u32_t, 8, 4> target(lh_gx_pixmap_format_argb8888);
    Target<lh_byte_t, 2, 1> mask(lh_gx_pixmap_format_a8);
    lh_pixel_gradient_t gradient;
    lh_gx_canvas_t canvas;
    lh_gx_backend_sw_t sw;
    lh_gx_brush_t brush;
    lh_gx_point_t at;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, stops, lh_null, 2);
    lh_gx_brush_init_gradient(&brush, &gradient, nullptr, rect(0, 0, 8, 4));
    lh_gx_backend_sw_canvas_init_solid(&canvas);
    EXPECT_TRUE(lh_gx_canvas_get_fill_rect(&canvas) == lh_gx_backend_sw_fill_rect_solid);
    EXPECT_TRUE(lh_gx_canvas_get_fill_mask(&canvas) == lh_gx_backend_sw_fill_mask_solid);
    lh_gx_backend_sw_init(&sw, &target.pixmap, 0, 0);
    const lh_gx_rect_t all = rect(0, 0, 8, 4);
    const lh_gx_rect_t top = rect(0, 0, 8, 2);

    lh_gx_canvas_get_fill_rect (&canvas)(&sw, &all, &top, &brush);
    EXPECT_EQ(target.at(0, 0), 0xFF2050A0U);
    EXPECT_EQ(target.at(7, 1), 0xFF2050A0U) << "the first colour everywhere";
    EXPECT_EQ(target.at(0, 2), 0U);
    mask.words[0] = 255U;
    lh_gx_point_init(&at, 3, 3);
    lh_gx_canvas_get_fill_mask (&canvas)(&sw, &all, &mask.pixmap, at, &brush);
    EXPECT_EQ(target.at(3, 3), 0xFF2050A0U);
    EXPECT_EQ(target.at(4, 3), 0U) << "no coverage";
}

} // namespace
