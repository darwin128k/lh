#include <gtest/gtest.h>
#include <lh/ui/canvas/stroke.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/entity.h>

TEST(ui_canvas_stroke, rounded_ring_matches_coverage_with_alpha_clipping_and_consumed_interior)
{
    lh_u32_t pixels[32 * 24];
    lh_ui_pixmap_t pixmap;
    lh_ui_pixmap_init(&pixmap, reinterpret_cast<lh_byte_ptr>(pixels), 32, 24, 32 * 4,
                      lh_ui_pixmap_format_argb8888);
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_sw_init(&sw);
    lh_ui_canvas_sw_use_pixmap(&sw, &pixmap);
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, &lh_ui_canvas_backend_sw, &sw);
    lh_ui_size_t size;
    lh_ui_size_init(&size, 32, 24);
    lh_ui_canvas_set_size(&canvas, size);
    lh_ui_rect_t box;
    lh_ui_rect_init(&box, 5, 4, 20, 16);
    lh_ui_radii_t radii;
    lh_ui_radii_init(&radii, 5, 3, 0, 4);
    lh_ui_color_t background;
    lh_ui_color_init(&background, 17, 33, 49, 255);
    for (int fractional : {0, 1})
    {
#if !LH_LIBRARY_OPTION_MATH_FPU
        if (fractional)
        {
            continue;
        }
#endif
        lh_ui_rect_init(
            &box, lh_ui_scalar_of(5 + fractional * 0.25), lh_ui_scalar_of(4 + fractional * 0.75),
            lh_ui_scalar_of(20 + fractional * 0.5), lh_ui_scalar_of(16 + fractional * 0.5));
        for (int width : {0, 1, 3, 9})
        {
            for (int alpha : {143, 255})
            {
                for (bool clipped : {false, true})
                {
                    const lh_ui_scalar_t border_width =
                        lh_ui_scalar_of(width + ((fractional && width > 0) ? 0.25 : 0));
                    lh_ui_color_t color;
                    lh_ui_color_init(&color, 210, 110, 70, static_cast<lh_byte_t>(alpha));
                    lh_ui_pixmap_clear(&pixmap, &background);
                    lh_ui_canvas_begin(&canvas);
                    lh_ui_rect_t clip;
                    lh_ui_rect_init(&clip, 8, 6, 13, 9);
                    lh_ui_point_t zero;
                    lh_ui_point_init(&zero, 0, 0);
                    if (clipped)
                    {
                        lh_ui_canvas_push(&canvas, zero, &clip);
                    }
                    lh_ui_canvas_stroke_box(&canvas, &box, &radii, border_width, &color);
                    if (clipped)
                    {
                        lh_ui_canvas_pop(&canvas);
                    }
                    lh_ui_canvas_end(&canvas);
                    lh_ui_rect_t hole;
                    lh_ui_rect_inset(&box, border_width, border_width, &hole);
                    lh_ui_radii_t inner;
                    lh_ui_radii_init(&inner, lh_math_max(0, 5 - border_width),
                                     lh_math_max(0, 3 - border_width), 0,
                                     lh_math_max(0, 4 - border_width));
                    for (int y = 0; y < 24; ++y)
                    {
                        for (int x = 0; x < 32; ++x)
                        {
                            int coverage = 0;
                            if (width > 0 && (!clipped || (x >= 8 && x < 21 && y >= 6 && y < 15)))
                            {
                                coverage = lh_ui_radius_coverage(
                                    &box, lh_ui_radii_get_at(&radii, &box, x, y), x, y);
                                if (!lh_ui_rect_is_empty(&hole))
                                {
                                    coverage = lh_math_max(
                                        0, coverage - lh_ui_radius_coverage(
                                                          &hole,
                                                          lh_ui_radii_get_at(&inner, &hole, x, y),
                                                          x, y));
                                }
                            }
                            lh_ui_color_t edge;
                            lh_ui_color_with_coverage(&color, static_cast<lh_byte_t>(coverage),
                                                      &edge);
                            lh_ui_color_t expected;
                            lh_ui_color_over(&background, &edge, &expected);
                            ASSERT_EQ(pixels[y * 32 + x], lh_ui_color_get_argb(&expected))
                                << x << "," << y << " width=" << width << " clip=" << clipped;
                        }
                    }
                }
            }
        }
    }
    lh_ui_canvas_deinit(&canvas);
}

TEST(ui_canvas_stroke, native_entity_gradient_and_border_share_the_rounded_shape)
{
    lh_u32_t pixels[32 * 24];
    lh_ui_pixmap_t pixmap;
    lh_ui_pixmap_init(&pixmap, reinterpret_cast<lh_byte_ptr>(pixels), 32, 24, 32 * 4,
                      lh_ui_pixmap_format_argb8888);
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_sw_init(&sw);
    lh_ui_canvas_sw_use_pixmap(&sw, &pixmap);
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, &lh_ui_canvas_backend_sw, &sw);
    lh_ui_size_t size;
    lh_ui_size_init(&size, 32, 24);
    lh_ui_canvas_set_size(&canvas, size);
    lh_ui_color_t black;
    lh_ui_color_init(&black, 0, 0, 0, 255);
    lh_ui_pixmap_clear(&pixmap, &black);
    lh_ui_color_t colors[2];
    lh_ui_color_init(&colors[0], 30, 60, 100, 255);
    lh_ui_color_init(&colors[1], 10, 20, 40, 255);
    lh_ui_gradient_t gradient;
    lh_ui_gradient_init(&gradient);
    lh_ui_gradient_init_stops(&gradient, colors, lh_null, 2);
    lh_ui_paint_t fill;
    lh_ui_paint_init_gradient(&fill, &gradient);
    lh_ui_style_t style;
    lh_ui_style_init(&style);
    lh_ui_style_assign_fill(&style, &fill);
    lh_ui_style_set_radius_v(&style, 5);
    lh_ui_insets_t border;
    lh_ui_insets_init_all(&border, 1);
    lh_ui_style_set_border_v(&style, &border);
    lh_ui_color_t white;
    lh_ui_color_init(&white, 255, 255, 255, 255);
    lh_ui_paint_init_color(&fill, &white);
    lh_ui_style_assign_border_fill(&style, &fill);
    lh_ui_rect_t rect;
    lh_ui_rect_init(&rect, 5, 4, 20, 16);
    lh_ui_entity_t entity;
    lh_ui_entity_init(&entity, rect);
    lh_ui_entity_set_style_v(&entity, &style);
    lh_ui_entity_draw(&entity, &canvas);
    EXPECT_EQ(pixels[4 * 32 + 5], 0xff000000U);
    EXPECT_EQ(pixels[4 * 32 + 15], 0xffffffffU);
    EXPECT_EQ(pixels[12 * 32 + 5], 0xffffffffU);
    EXPECT_NE(pixels[8 * 32 + 15], pixels[15 * 32 + 15]);
    EXPECT_NE(pixels[12 * 32 + 15], 0xffffffffU);
    lh_ui_canvas_deinit(&canvas);
}
