/**
 * @file gradient.cpp
 * @brief Tests for `lh/core/pixel/gradient.h` and its bricks: the stop, the sine,
 *        the lerp, the colour table and the sampler.
 *
 * The gradient says what one pixel is (::lh_pixel_gradient_get_frac_at,
 * ::lh_pixel_gradient_get_color_at); the table and the sampler say the same for a
 * whole box with additions only. The tests hold the fast bricks to the slow ones
 * pixel by pixel, over boxes, angles and stop arrangements that reach every branch.
 */

#include <gtest/gtest.h>

#include <lh/core/pixel/argb.h>
#include <lh/core/pixel/gradient/lerp.h>
#include <lh/core/pixel/gradient/sampler.h>
#include <lh/core/pixel/gradient/sine.h>
#include <lh/core/pixel/gradient/stop/initializer.h>
#include <lh/core/pixel/gradient/table.h>

namespace
{

lh_math_rect_t
box(int x, int y, int w, int h)
{
    lh_math_rect_t r;

    lh_math_rect_init(&r, x, y, w, h);
    return r;
}

TEST(pixel_gradient_stop, a_stop_is_a_colour_and_a_position)
{
    static const lh_pixel_gradient_stop_t constant =
        lh_pixel_gradient_stop_initializer(0xFF102030U, 77);
    lh_pixel_gradient_stop_t stop;

    lh_pixel_gradient_stop_init(&stop, 0x80FF0000U, 128);
    EXPECT_EQ(lh_pixel_gradient_stop_get_color(&stop), 0x80FF0000U);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(&stop), 128);
    lh_pixel_gradient_stop_set_frac(&stop, 3);
    lh_pixel_gradient_stop_set_color(&stop, 1U);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(&stop), 3);
    EXPECT_EQ(lh_pixel_gradient_stop_get_color(&stop), 1U);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(&constant), 77);
}

TEST(pixel_gradient_sine, whole_degrees_fold_onto_the_first_quadrant)
{
    EXPECT_EQ(lh_pixel_gradient_sine(0), 0);
    EXPECT_EQ(lh_pixel_gradient_sine(90), 4096);
    EXPECT_EQ(lh_pixel_gradient_sine(180), 0);
    EXPECT_EQ(lh_pixel_gradient_sine(270), -4096);
    EXPECT_EQ(lh_pixel_gradient_sine(30), 2048);
    EXPECT_EQ(lh_pixel_gradient_sine(-90), -4096) << "negative folds";
    EXPECT_EQ(lh_pixel_gradient_sine(450), 4096) << "past 360 folds";
    EXPECT_EQ(lh_pixel_gradient_sine(150), lh_pixel_gradient_sine(30));
}

TEST(pixel_gradient, a_new_gradient_has_no_stops_and_runs_down)
{
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    EXPECT_EQ(lh_pixel_gradient_get_stops_count(&gradient), 0);
    EXPECT_EQ(lh_pixel_gradient_get_shape(&gradient), lh_pixel_gradient_shape_linear);
    EXPECT_EQ(lh_pixel_gradient_get_angle(&gradient), 180);
    EXPECT_EQ(lh_pixel_gradient_get_color_at(&gradient, 100), 0U) << "no stops: clear";
    EXPECT_GE(LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS, 1);
}

TEST(pixel_gradient, stops_spread_evenly_or_stand_where_they_are_told)
{
    const lh_u32_t colors[2] = {0xFFFF0000U, 0xFF0000FFU};
    const lh_byte_t fracs[2] = {40, 200};
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, colors, lh_null, 2);
    ASSERT_EQ(lh_pixel_gradient_get_stops_count(&gradient), 2);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(&gradient, 0)), 0);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(&gradient, 1)), 255);
    EXPECT_EQ(lh_pixel_gradient_stop_get_color(lh_pixel_gradient_get_stop(&gradient, 1)),
              0xFF0000FFU);
    lh_pixel_gradient_init_stops(&gradient, colors, fracs, 2);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(&gradient, 0)), 40);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(&gradient, 1)), 200);
    lh_pixel_gradient_init_stops(&gradient, colors, lh_null, 1);
    EXPECT_EQ(lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(&gradient, 0)), 0)
        << "one stop stands at the start";
}

TEST(pixel_gradient, the_angle_folds_and_the_shape_follows_the_setter)
{
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_set_linear(&gradient, -90);
    EXPECT_EQ(lh_pixel_gradient_get_angle(&gradient), 270);
    lh_pixel_gradient_set_linear(&gradient, 725);
    EXPECT_EQ(lh_pixel_gradient_get_angle(&gradient), 5);
    lh_pixel_gradient_set_radial(&gradient);
    EXPECT_EQ(lh_pixel_gradient_get_shape(&gradient), lh_pixel_gradient_shape_radial);
    lh_pixel_gradient_set_linear(&gradient, 90);
    EXPECT_EQ(lh_pixel_gradient_get_shape(&gradient), lh_pixel_gradient_shape_linear);
}

TEST(pixel_gradient, a_pixel_falls_where_the_angle_points)
{
    const lh_math_rect_t tall = box(0, 0, 10, 100);
    const lh_math_rect_t wide = box(0, 0, 100, 10);
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    EXPECT_LT(lh_pixel_gradient_get_frac_at(&gradient, &tall, 5, 0), 5) << "180: the top first";
    EXPECT_GT(lh_pixel_gradient_get_frac_at(&gradient, &tall, 5, 99), 250);
    EXPECT_NEAR(lh_pixel_gradient_get_frac_at(&gradient, &tall, 5, 50), 128, 3);
    lh_pixel_gradient_set_linear(&gradient, 90);
    EXPECT_LT(lh_pixel_gradient_get_frac_at(&gradient, &wide, 0, 5), 5) << "90: left to right";
    EXPECT_GT(lh_pixel_gradient_get_frac_at(&gradient, &wide, 99, 5), 250);
    lh_pixel_gradient_set_radial(&gradient);
    EXPECT_LT(lh_pixel_gradient_get_frac_at(&gradient, &wide, 50, 5), 30) << "radial: the middle";
    EXPECT_GT(lh_pixel_gradient_get_frac_at(&gradient, &wide, 0, 0), 235) << "and the corner";
    const lh_math_rect_t empty = box(0, 0, 0, 10);

    EXPECT_EQ(lh_pixel_gradient_get_frac_at(&gradient, &empty, 0, 0), 0);
}

TEST(pixel_gradient, a_colour_between_two_stops_is_the_two_in_proportion)
{
    const lh_u32_t colors[2] = {lh_pixel_argb_make(0, 255, 10, 255),
                                lh_pixel_argb_make(255, 0, 200, 0)};
    const lh_byte_t fracs[2] = {50, 150};
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, colors, fracs, 2);
    EXPECT_EQ(lh_pixel_gradient_get_color_at(&gradient, 0), colors[0]) << "before the first";
    EXPECT_EQ(lh_pixel_gradient_get_color_at(&gradient, 255), colors[1]) << "after the last";
    const lh_u32_t mid = lh_pixel_gradient_get_color_at(&gradient, 100);

    EXPECT_EQ(lh_pixel_argb_get_channel(mid, 16), 127U);
    EXPECT_EQ(lh_pixel_argb_get_channel(mid, 8), 127U);
    EXPECT_EQ(lh_pixel_argb_get_channel(mid, 0), 105U);
    EXPECT_EQ(lh_pixel_argb_get_channel(mid, 24), 127U) << "alpha mixes too";
}

TEST(pixel_gradient, the_segment_is_the_first_stop_at_or_past_a_position)
{
    const lh_u32_t colors[2] = {1U, 2U};
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, colors, lh_null, 2);
    EXPECT_EQ(lh_pixel_gradient_get_segment_at(&gradient, 1, 100), 1);
    EXPECT_EQ(lh_pixel_gradient_get_segment_at(&gradient, 0, 0), 0);
    EXPECT_EQ(lh_pixel_gradient_get_segment_at(&gradient, 0, 1), 1);
}

TEST(pixel_gradient_lerp, it_walks_what_dividing_again_gives)
{
    lh_pixel_gradient_lerp_t lerp;

    lh_pixel_gradient_lerp_init(&lerp, lh_pixel_argb_make(0, 255, 10, 255),
                                lh_pixel_argb_make(255, 0, 200, 0), 7, 2);
    for (lh_s32_t at = 2; at <= 7; ++at)
    {
        const lh_u32_t got = lh_pixel_gradient_lerp_get_color(&lerp);

        EXPECT_EQ(lh_pixel_argb_get_channel(got, 16), lh_u32_t((0 * (7 - at) + 255 * at) / 7));
        EXPECT_EQ(lh_pixel_argb_get_channel(got, 8), lh_u32_t((255 * (7 - at) + 0 * at) / 7));
        EXPECT_EQ(lh_pixel_argb_get_channel(got, 0), lh_u32_t((10 * (7 - at) + 200 * at) / 7));
        EXPECT_EQ(lh_pixel_argb_get_channel(got, 24), lh_u32_t((255 * (7 - at) + 0 * at) / 7));
        lh_pixel_gradient_lerp_next(&lerp);
    }
}

TEST(pixel_gradient_table, every_entry_is_the_colour_at_its_position)
{
    /* In order, two on one place, the wrong way round, and the last three together. */
    const lh_byte_t positions[][4] = {
        {0, 80, 160, 255}, {20, 20, 220, 255}, {200, 50, 30, 10}, {0, 255, 255, 255}};
    lh_u32_t colors[4];

    for (lh_u32_t i = 0U; i < 4U; ++i)
    {
        colors[i] =
            lh_pixel_argb_make(20U + 50U * i, 200U - 30U * i, 15U + 20U * i, 255U - 30U * i);
    }
    for (const auto &fracs : positions)
    {
        for (int count = 1; count <= 4 && count <= LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS; ++count)
        {
            lh_pixel_gradient_t gradient;
            lh_pixel_gradient_table_t table;

            lh_pixel_gradient_init(&gradient);
            lh_pixel_gradient_init_stops(&gradient, colors, fracs,
                                         static_cast<lh_pixel_gradient_stop_count_t>(count));
            lh_pixel_gradient_table_fill(&table, &gradient);
            for (int frac = 0; frac < 256; ++frac)
            {
                ASSERT_EQ(lh_pixel_gradient_table_get_colors(&table)[frac],
                          lh_pixel_gradient_get_color_at(&gradient, static_cast<lh_byte_t>(frac)))
                    << "stops " << count << " position " << frac;
            }
        }
    }
}

TEST(pixel_gradient_table, map_looks_positions_up)
{
    const lh_u32_t colors[2] = {0xFF000000U, 0xFFFFFFFFU};
    const lh_byte_t levels[3] = {0, 255, 0};
    lh_pixel_gradient_t gradient;
    lh_pixel_gradient_table_t table;
    lh_u32_t out[3];

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, colors, lh_null, 2);
    lh_pixel_gradient_table_fill(&table, &gradient);
    lh_pixel_gradient_table_map(&table, levels, 3, out);
    EXPECT_EQ(out[0], 0xFF000000U);
    EXPECT_EQ(out[1], 0xFFFFFFFFU);
    EXPECT_EQ(out[2], 0xFF000000U);
}

/* The sampler carries the position from pixel to pixel and walks a radial one along
   fixed squared distances; every pixel of a row and of a column, inside the box and
   past it, has to be what ::lh_pixel_gradient_get_frac_at says on its own. */
TEST(pixel_gradient_sampler, a_row_and_a_column_are_every_pixel_asked_alone)
{
    static const int boxes[][4] = {
        {0, 0, 40, 30}, {-7, 5, 13, 51}, {3, -9, 64, 7}, {11, 2, 1, 1}, {5, 5, 2, 90}};
    lh_pixel_gradient_t gradient;
    lh_byte_t row[120];
    lh_byte_t column[160];

    lh_pixel_gradient_init(&gradient);
    for (int angle = -1; angle < 361; angle += 7)
    {
        if (angle < 0)
        {
            lh_pixel_gradient_set_radial(&gradient);
        }
        else
        {
            lh_pixel_gradient_set_linear(&gradient, angle);
        }
        for (const auto &b : boxes)
        {
            const lh_math_rect_t rect = box(b[0], b[1], b[2], b[3]);
            lh_pixel_gradient_sampler_t sampler;

            lh_pixel_gradient_sampler_init(&sampler, &gradient, &rect);
            for (int y = b[1] - 3; y < b[1] + b[3] + 3; ++y)
            {
                lh_pixel_gradient_sampler_row(&sampler, b[0] - 20, b[0] + 100, y, row);
                for (int x = b[0] - 20; x < b[0] + 100; ++x)
                {
                    ASSERT_EQ(row[x - (b[0] - 20)],
                              lh_pixel_gradient_get_frac_at(&gradient, &rect, x, y))
                        << "angle " << angle << " box " << b[0] << "," << b[1] << " " << b[2] << "x"
                        << b[3] << " at " << x << "," << y;
                }
            }
            for (int x = b[0] - 2; x < b[0] + b[2] + 2; ++x)
            {
                lh_pixel_gradient_sampler_column(&sampler, x, b[1] - 20, b[1] + 140, column);
                for (int y = b[1] - 20; y < b[1] + 140; ++y)
                {
                    ASSERT_EQ(column[y - (b[1] - 20)],
                              lh_pixel_gradient_get_frac_at(&gradient, &rect, x, y))
                        << "column, angle " << angle << " at " << x << "," << y;
                }
            }
        }
    }
}

TEST(pixel_gradient_sampler, an_empty_box_is_flat_at_the_first_position)
{
    const lh_math_rect_t empty = box(0, 0, 0, 4);
    lh_pixel_gradient_t gradient;
    lh_pixel_gradient_sampler_t sampler;
    lh_byte_t row[3] = {9, 9, 9};

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_sampler_init(&sampler, &gradient, &empty);
    EXPECT_TRUE(lh_pixel_gradient_sampler_is_flat(&sampler));
    lh_pixel_gradient_sampler_row(&sampler, 0, 3, 0, row);
    EXPECT_EQ(row[0], 0);
    EXPECT_EQ(row[2], 0);
}

TEST(pixel_gradient_sampler, a_linear_run_is_the_row_it_starts)
{
    const lh_math_rect_t square = box(0, 0, 20, 20);
    lh_pixel_gradient_t gradient;
    lh_pixel_gradient_sampler_t sampler;
    lh_byte_t a[20];
    lh_byte_t b[20];

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_set_linear(&gradient, 30);
    lh_pixel_gradient_sampler_init(&sampler, &gradient, &square);
    EXPECT_FALSE(lh_pixel_gradient_sampler_is_flat(&sampler));
    lh_pixel_gradient_sampler_row(&sampler, 0, 20, 5, a);
    lh_pixel_gradient_sampler_linear_run(
        &sampler, 1 - lh_pixel_gradient_sampler_get_middle_x(&sampler),
        11 - lh_pixel_gradient_sampler_get_middle_y(&sampler),
        4 * lh_pixel_gradient_sampler_get_sine(&sampler) * 255, 20, b);
    for (int i = 0; i < 20; ++i)
    {
        EXPECT_EQ(a[i], b[i]) << i;
    }
}

} // namespace
