#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/gl/gradient/sampler.h>
#include <lh/test/out.h>

namespace
{

lh_gl_color_t
rgba(lh_gl_color_channel_t r, lh_gl_color_channel_t g, lh_gl_color_channel_t b,
     lh_gl_color_channel_t a)
{
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), r, g, b, a);
    return color;
}

TEST(ui_gradient, make_empty_has_no_stops)
{
    lh_gl_gradient_t gradient;

    lh_gl_gradient_init(lh_addr_of(gradient));
    EXPECT_EQ(lh_gl_gradient_get_stop_count(lh_addr_of(gradient)), 0);
}

TEST(ui_gradient, init_stops_even_fracs_default_capacity)
{
    const lh_gl_color_t colors[2] = {
        rgba(255, 0, 0, 255),
        rgba(0, 0, 255, 255),
    };
    lh_gl_gradient_t gradient;
    lh_gl_gradient_init_stops(lh_addr_of(gradient), colors, lh_null, 2);
    ASSERT_EQ(lh_gl_gradient_get_stop_count(lh_addr_of(gradient)), 2);
    EXPECT_EQ(lh_gl_gradient_stop_get_frac(lh_gl_gradient_get_stop(lh_addr_of(gradient), 0)), 0);
    EXPECT_EQ(lh_gl_gradient_stop_get_frac(lh_gl_gradient_get_stop(lh_addr_of(gradient), 1)), 255);
    EXPECT_EQ(lh_gl_color_get_r(
                  lh_gl_gradient_stop_get_color(lh_gl_gradient_get_stop(lh_addr_of(gradient), 0))),
              255);
    EXPECT_EQ(lh_gl_color_get_b(
                  lh_gl_gradient_stop_get_color(lh_gl_gradient_get_stop(lh_addr_of(gradient), 1))),
              255);
}

TEST(ui_gradient, init_stops_uses_explicit_fracs)
{
    const lh_gl_color_t colors[2] = {
        rgba(0, 0, 0, 255),
        rgba(255, 255, 255, 255),
    };
    const lh_byte_t fracs[2] = {40, 200};
    lh_gl_gradient_t gradient;
    lh_gl_gradient_init_stops(lh_addr_of(gradient), colors, fracs, 2);
    EXPECT_EQ(lh_gl_gradient_stop_get_frac(lh_gl_gradient_get_stop(lh_addr_of(gradient), 0)), 40);
    EXPECT_EQ(lh_gl_gradient_stop_get_frac(lh_gl_gradient_get_stop(lh_addr_of(gradient), 1)), 200);
}

TEST(ui_gradient, max_stops_option_is_at_least_one)
{
    EXPECT_GE(LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS, 1);
}

TEST(ui_gradient, a_sampled_row_is_every_pixel_asked_alone)
{
    static const int boxes[][4] = {{0, 0, 40, 30}, {-7, 5, 13, 51}, {3, -9, 64, 7}, {11, 2, 1, 1}, {5, 5, 2, 90}};
    /* Evenly, inset, on one step, and the wrong way round. */
    static const lh_byte_t stop_fracs[][2] = {{0, 255}, {30, 220}, {60, 60}, {200, 50}};
    lh_gl_color_t colors[2];

    /* The sampler carries the place on the stop axis from pixel to pixel and walks a
       radial one along fixed squared distances; every pixel has to come out as
       ::lh_gl_gradient_frac_at and ::lh_gl_gradient_color_at say on their own. */
    lh_gl_color_init(lh_addr_of(colors[0]), 10, 200, 30, 255);
    lh_gl_color_init(lh_addr_of(colors[1]), 250, 20, 140, 90);
    lh_gl_gradient_t gradient;
    lh_gl_color_t row[120];
    for (const auto &fracs : stop_fracs)
    {
        lh_gl_gradient_init(lh_addr_of(gradient));
        lh_gl_gradient_init_stops(lh_addr_of(gradient), colors, fracs, 2);
        for (int angle = -1; angle < 361; angle += 7)
        {
            if (angle < 0)
            {
                lh_gl_gradient_set_radial(lh_addr_of(gradient));
            }
            else
            {
                lh_gl_gradient_set_linear(lh_addr_of(gradient), angle);
            }
            for (const auto &b : boxes)
            {
                lh_gl_rect_t rect;
                lh_gl_gradient_sampler_t sampler;

                lh_gl_rect_init(lh_addr_of(rect), b[0], b[1], b[2], b[3]);
                lh_gl_gradient_sampler_init(lh_addr_of(sampler), lh_addr_of(gradient), lh_addr_of(rect));
                for (int y = b[1] - 3; y < b[1] + b[3] + 3; ++y)
                {
                    lh_gl_gradient_sampler_row(lh_addr_of(sampler), b[0] - 20, b[0] + 100, y, row);
                    for (int x = b[0] - 20; x < b[0] + 100; ++x)
                    {
                        const lh_gl_color_t want = lh_test::out(lh_gl_gradient_color_at, 
                            lh_addr_of(gradient), lh_gl_gradient_frac_at(lh_addr_of(gradient), lh_addr_of(rect), x, y));

                        ASSERT_TRUE(lh_gl_color_equals(row + (x - (b[0] - 20)), lh_addr_of(want)))
                            << "angle " << angle << " box " << b[0] << "," << b[1] << " " << b[2] << "x" << b[3]
                            << " at " << x << "," << y;
                    }
                }
            }
        }
    }
}

} // namespace

/* -- Sampler bricks -------------------------------------------------------- */

TEST(ui_gradient_sampler_bricks, prepared_colors_match_each_stop_arrangement)
{
    const lh_byte_t positions[][4] = {
        {0, 80, 160, 255}, {20, 20, 220, 255}, {200, 50, 30, 10}, {0, 255, 255, 255}};
    lh_gl_color_t colors[4];
    for (int i = 0; i < 4; ++i)
    {
        lh_gl_color_init(&colors[i], 20 + 50 * i, 200 - 30 * i, 15 + 20 * i, 255 - 30 * i);
    }
    lh_gl_rect_t rect;
    lh_gl_rect_init(&rect, 0, 0, 16, 16);
    for (const auto &fracs : positions)
    {
        for (int count = 1; count <= 4 && count <= LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS; ++count)
        {
            lh_gl_gradient_t gradient;
            lh_gl_gradient_init(&gradient);
            lh_gl_gradient_init_stops(&gradient, colors, fracs,
                                      lh_cast_static(lh_gl_gradient_stop_count_t, count));
            lh_gl_gradient_sampler_t sampler;
            lh_gl_gradient_sampler_init(&sampler, &gradient, &rect);
            for (int frac = 0; frac < 256; ++frac)
            {
                const lh_gl_color_t expected = lh_test::out(lh_gl_gradient_color_at, &gradient,
                                                            lh_cast_static(lh_byte_t, frac));
                EXPECT_TRUE(lh_gl_color_equals(&lh_gl_gradient_sampler_get_colors(&sampler)[frac],
                                               &expected))
                    << "stops " << count << " fraction " << frac;
            }
        }
    }
}

TEST(ui_gradient_sampler_bricks, lerp_walks_what_dividing_again_gives)
{
    const lh_gl_color_t from = rgba(0, 255, 10, 255);
    const lh_gl_color_t to = rgba(255, 0, 200, 0);
    lh_gl_gradient_sampler_lerp_t lerp;

    lh_gl_gradient_sampler_lerp_init(&lerp, &from, &to, 7, 2);
    for (lh_s32_t at = 2; at <= 7; ++at)
    {
        const lh_gl_color_t got = lh_test::out(lh_gl_gradient_sampler_lerp_color, &lerp);

        EXPECT_EQ(lh_gl_color_get_r(&got), (0 * (7 - at) + 255 * at) / 7);
        EXPECT_EQ(lh_gl_color_get_g(&got), (255 * (7 - at) + 0 * at) / 7);
        EXPECT_EQ(lh_gl_color_get_b(&got), (10 * (7 - at) + 200 * at) / 7);
        EXPECT_EQ(lh_gl_color_get_a(&got), (255 * (7 - at) + 0 * at) / 7);
        lh_gl_gradient_sampler_lerp_next(&lerp);
    }
}

TEST(ui_gradient_sampler_bricks, flat_sampler_repeats_its_first_colour)
{
    const lh_gl_color_t colors[2] = {rgba(1, 2, 3, 255), rgba(200, 100, 50, 255)};
    lh_gl_gradient_t gradient;

    lh_gl_gradient_init(&gradient);
    lh_gl_gradient_init_stops(&gradient, colors, lh_null, 2);
    lh_gl_rect_t empty;
    lh_gl_rect_init(&empty, lh_gl_scalar(0), lh_gl_scalar(0), lh_gl_scalar(0), lh_gl_scalar(4));
    lh_gl_gradient_sampler_t sampler;
    lh_gl_gradient_sampler_init(&sampler, &gradient, &empty);
    EXPECT_TRUE(lh_gl_gradient_sampler_is_flat(&sampler));
    lh_gl_color_t row[3];
    lh_gl_gradient_sampler_flat_run(&sampler, 3, row);
    for (const lh_gl_color_t &c : row)
    {
        EXPECT_EQ(lh_gl_color_get_r(&c), lh_gl_color_get_r(&sampler.colors[0]));
    }
    EXPECT_EQ(lh_gl_gradient_sampler_stop_frac(&gradient, 0), 0);
    EXPECT_EQ(lh_gl_gradient_sampler_stop_frac(&gradient, 1), 255);
    EXPECT_EQ(lh_gl_gradient_sampler_segment_at(&gradient, 1, 100), 1);
}

TEST(ui_gradient_sampler_bricks, linear_run_matches_row_and_column)
{
    const lh_gl_color_t colors[2] = {rgba(0, 0, 0, 255), rgba(255, 255, 255, 255)};
    lh_gl_gradient_t gradient;

    lh_gl_gradient_init(&gradient);
    lh_gl_gradient_init_stops(&gradient, colors, lh_null, 2);
    lh_gl_gradient_set_linear(&gradient, 30);
    lh_gl_rect_t box;
    lh_gl_rect_init(&box, lh_gl_scalar(0), lh_gl_scalar(0), lh_gl_scalar(20), lh_gl_scalar(20));
    lh_gl_gradient_sampler_t sampler;
    lh_gl_gradient_sampler_init(&sampler, &gradient, &box);
    EXPECT_FALSE(lh_gl_gradient_sampler_is_flat(&sampler));
    lh_gl_color_t a[20];
    lh_gl_gradient_sampler_row(&sampler, 0, 20, 5, a);
    lh_gl_color_t b[20];
    lh_gl_gradient_sampler_linear_run(&sampler, 1 - sampler.middle_x, 11 - sampler.middle_y, 4 * sampler.sine * 255,
                                      20, b);
    for (int i = 0; i < 20; ++i)
    {
        EXPECT_EQ(lh_gl_color_get_r(&a[i]), lh_gl_color_get_r(&b[i])) << i;
    }
}
