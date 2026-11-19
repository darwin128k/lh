/**
 * @file brush.cpp
 * @brief Tests for `lh/gx/brush.h`.
 *
 * A brush is one colour or a gradient over a box: it reads back what it was made
 * of, a clear colour draws nothing while a gradient always may, and the colour it
 * gives a pixel is the gradient's at that pixel — the same from its table as
 * worked out, and the table the brush is made ready with is its own or one
 * filled for it.
 */

#include <gtest/gtest.h>

#include <lh/gx/brush.h>

namespace
{

lh_gx_rect_t
rect(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w, lh_gx_scalar_t h)
{
    lh_gx_rect_t r;

    lh_gx_rect_init(&r, x, y, w, h);
    return r;
}

TEST(gx_brush, one_colour_is_the_colour_everywhere)
{
    lh_gx_brush_t brush;

    lh_gx_brush_init_color(&brush, 0xFF112233U);
    EXPECT_EQ(lh_gx_brush_get_color(&brush), 0xFF112233U);
    EXPECT_FALSE(lh_gx_brush_is_gradient(&brush));
    EXPECT_FALSE(lh_gx_brush_is_clear(&brush));
    EXPECT_TRUE(lh_gx_brush_get_gradient(&brush) == nullptr);
    EXPECT_TRUE(lh_gx_brush_get_table(&brush) == nullptr);
    EXPECT_EQ(lh_gx_brush_get_color_at(&brush, -40, 900), 0xFF112233U);
    lh_gx_brush_init_color(&brush, 0x00FFFFFFU);
    EXPECT_TRUE(lh_gx_brush_is_clear(&brush)) << "alpha 0 draws nothing";
}

TEST(gx_brush, a_gradient_gives_each_pixel_its_colour_over_the_box)
{
    const lh_u32_t stops[2] = {0x00000000U, 0xFFFFFFFFU};
    const lh_gx_rect_t box = rect(10, 20, 30, 40);
    lh_pixel_gradient_t gradient;
    lh_pixel_gradient_table_t table;
    lh_gx_brush_t worked;
    lh_gx_brush_t looked_up;
    lh_math_rect_t px;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, stops, lh_null, 2);
    lh_pixel_gradient_set_linear(&gradient, 60);
    lh_pixel_gradient_table_fill(&table, &gradient);
    lh_gx_brush_init_gradient(&worked, &gradient, nullptr, box);
    lh_gx_brush_init_gradient(&looked_up, &gradient, &table, box);
    lh_gx_rect_to_rect(&box, &px);
    EXPECT_TRUE(lh_gx_brush_is_gradient(&worked));
    EXPECT_FALSE(lh_gx_brush_is_clear(&worked)) << "its first colour is clear, it is not";
    EXPECT_EQ(lh_gx_brush_get_color(&worked), 0x00000000U) << "the colour is the first";
    EXPECT_TRUE(lh_gx_rect_eq(lh_gx_brush_get_box(&worked), &box));
    for (int y = 15; y < 65; y += 3)
    {
        for (int x = 5; x < 45; x += 2)
        {
            const lh_u32_t want = lh_pixel_gradient_get_color_at(
                &gradient, lh_pixel_gradient_get_frac_at(&gradient, &px, x, y));

            ASSERT_EQ(lh_gx_brush_get_color_at(&worked, x, y), want) << x << "," << y;
            ASSERT_EQ(lh_gx_brush_get_color_at(&looked_up, x, y), want) << x << "," << y;
        }
    }
}

TEST(gx_brush, made_ready_it_lends_its_table_or_fills_one)
{
    const lh_u32_t stops[2] = {0xFF0000FFU, 0xFFFF0000U};
    lh_pixel_gradient_t gradient;
    lh_pixel_gradient_table_t table;
    lh_pixel_gradient_table_t own;
    lh_pixel_gradient_sampler_t sampler;
    lh_gx_brush_t brush;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, stops, lh_null, 2);
    lh_pixel_gradient_table_fill(&table, &gradient);
    lh_gx_brush_init_gradient(&brush, &gradient, &table, rect(0, 0, 8, 8));
    EXPECT_TRUE(lh_gx_brush_prepare(&brush, &own, &sampler) == &table);
    EXPECT_EQ(lh_pixel_gradient_sampler_get_width(&sampler), 8);
    lh_gx_brush_set_table(&brush, nullptr);
    EXPECT_TRUE(lh_gx_brush_prepare(&brush, &own, &sampler) == &own);
    EXPECT_EQ(lh_pixel_gradient_table_get_colors(&own)[255], 0xFFFF0000U);
}

} // namespace
