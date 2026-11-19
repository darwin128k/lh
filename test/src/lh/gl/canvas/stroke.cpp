#include <gtest/gtest.h>
#include <lh/gl/canvas/stroke.h>
#include <lh/gl/canvas/sw.h>
#include <lh/gl/entity.h>

TEST(ui_canvas_stroke, rounded_ring_matches_coverage_with_alpha_clipping_and_consumed_interior)
{
    lh_u32_t pixels[32 * 24];
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(&pixmap, reinterpret_cast<lh_byte_ptr>(pixels), 32, 24, 32 * 4,
                      lh_gl_pixmap_format_argb8888);
    lh_gl_canvas_sw_t sw;
    lh_gl_canvas_sw_init(&sw);
    lh_gl_canvas_sw_use_pixmap(&sw, &pixmap);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(&canvas, &lh_gl_canvas_backend_sw, &sw);
    lh_gl_size_t size;
    lh_gl_size_init(&size, 32, 24);
    lh_gl_canvas_set_size(&canvas, size);
    lh_gl_rect_t box;
    lh_gl_rect_init(&box, 5, 4, 20, 16);
    lh_gl_radii_t radii;
    lh_gl_radii_init(&radii, 5, 3, 0, 4);
    lh_gl_color_t background;
    lh_gl_color_init(&background, 17, 33, 49, 255);
    for (int fractional : {0, 1})
    {
#if !LH_LIBRARY_OPTION_MATH_FPU
        if (fractional)
        {
            continue;
        }
#endif
        lh_gl_rect_init(
            &box, lh_gl_scalar_of(5 + fractional * 0.25), lh_gl_scalar_of(4 + fractional * 0.75),
            lh_gl_scalar_of(20 + fractional * 0.5), lh_gl_scalar_of(16 + fractional * 0.5));
        for (int width : {0, 1, 3, 9})
        {
            for (int alpha : {143, 255})
            {
                for (bool clipped : {false, true})
                {
                    const lh_gl_scalar_t border_width =
                        lh_gl_scalar_of(width + ((fractional && width > 0) ? 0.25 : 0));
                    lh_gl_color_t color;
                    lh_gl_color_init(&color, 210, 110, 70, static_cast<lh_byte_t>(alpha));
                    lh_gl_pixmap_clear(&pixmap, &background);
                    lh_gl_canvas_begin(&canvas);
                    lh_gl_rect_t clip;
                    lh_gl_rect_init(&clip, 8, 6, 13, 9);
                    lh_gl_point_t zero;
                    lh_gl_point_init(&zero, 0, 0);
                    if (clipped)
                    {
                        lh_gl_canvas_push(&canvas, zero, &clip);
                    }
                    lh_gl_canvas_stroke_box(&canvas, &box, &radii, border_width, &color);
                    if (clipped)
                    {
                        lh_gl_canvas_pop(&canvas);
                    }
                    lh_gl_canvas_end(&canvas);
                    lh_gl_rect_t hole;
                    lh_gl_rect_inset(&box, border_width, border_width, &hole);
                    lh_gl_radii_t inner;
                    lh_gl_radii_init(&inner, lh_math_max(0, 5 - border_width),
                                     lh_math_max(0, 3 - border_width), 0,
                                     lh_math_max(0, 4 - border_width));
                    for (int y = 0; y < 24; ++y)
                    {
                        for (int x = 0; x < 32; ++x)
                        {
                            int coverage = 0;
                            if (width > 0 && (!clipped || (x >= 8 && x < 21 && y >= 6 && y < 15)))
                            {
                                coverage = lh_gl_radius_coverage(
                                    &box, lh_gl_radii_get_at(&radii, &box, x, y), x, y);
                                if (!lh_gl_rect_is_empty(&hole))
                                {
                                    coverage = lh_math_max(
                                        0, coverage - lh_gl_radius_coverage(
                                                          &hole,
                                                          lh_gl_radii_get_at(&inner, &hole, x, y),
                                                          x, y));
                                }
                            }
                            lh_gl_color_t edge;
                            lh_gl_color_with_coverage(&color, static_cast<lh_byte_t>(coverage),
                                                      &edge);
                            lh_gl_color_t expected;
                            lh_gl_color_over(&background, &edge, &expected);
                            ASSERT_EQ(pixels[y * 32 + x], lh_gl_color_get_argb(&expected))
                                << x << "," << y << " width=" << width << " clip=" << clipped;
                        }
                    }
                }
            }
        }
    }
    lh_gl_canvas_deinit(&canvas);
}

TEST(ui_canvas_stroke, native_entity_gradient_and_border_share_the_rounded_shape)
{
    lh_u32_t pixels[32 * 24];
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(&pixmap, reinterpret_cast<lh_byte_ptr>(pixels), 32, 24, 32 * 4,
                      lh_gl_pixmap_format_argb8888);
    lh_gl_canvas_sw_t sw;
    lh_gl_canvas_sw_init(&sw);
    lh_gl_canvas_sw_use_pixmap(&sw, &pixmap);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(&canvas, &lh_gl_canvas_backend_sw, &sw);
    lh_gl_size_t size;
    lh_gl_size_init(&size, 32, 24);
    lh_gl_canvas_set_size(&canvas, size);
    lh_gl_color_t black;
    lh_gl_color_init(&black, 0, 0, 0, 255);
    lh_gl_pixmap_clear(&pixmap, &black);
    lh_gl_color_t colors[2];
    lh_gl_color_init(&colors[0], 30, 60, 100, 255);
    lh_gl_color_init(&colors[1], 10, 20, 40, 255);
    lh_pixel_gradient_t gradient;
    lh_pixel_gradient_init(&gradient);
    lh_gl_gradient_init_stops(&gradient, colors, lh_null, 2);
    lh_gl_paint_t fill;
    lh_gl_paint_init_gradient(&fill, &gradient);
    lh_gl_style_t style;
    lh_gl_style_init(&style);
    lh_gl_style_assign_fill(&style, &fill);
    lh_gl_style_set_radius_v(&style, 5);
    lh_gl_insets_t border;
    lh_gl_insets_init_all(&border, 1);
    lh_gl_style_set_border_v(&style, &border);
    lh_gl_color_t white;
    lh_gl_color_init(&white, 255, 255, 255, 255);
    lh_gl_paint_init_color(&fill, &white);
    lh_gl_style_assign_border_fill(&style, &fill);
    lh_gl_rect_t rect;
    lh_gl_rect_init(&rect, 5, 4, 20, 16);
    lh_gl_entity_t entity;
    lh_gl_entity_init(&entity, rect);
    lh_gl_entity_set_style_v(&entity, &style);
    lh_gl_entity_draw(&entity, &canvas);
    EXPECT_EQ(pixels[4 * 32 + 5], 0xff000000U);
    EXPECT_EQ(pixels[4 * 32 + 15], 0xffffffffU);
    EXPECT_EQ(pixels[12 * 32 + 5], 0xffffffffU);
    EXPECT_NE(pixels[8 * 32 + 15], pixels[15 * 32 + 15]);
    EXPECT_NE(pixels[12 * 32 + 15], 0xffffffffU);
    lh_gl_canvas_deinit(&canvas);
}
