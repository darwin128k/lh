/**
 * @file gradient.cpp
 * @brief Tests for `lh/gl/gradient.h` and `lh/gl/gradient/sampler.h`: gl's step
 *        from its own colours and boxes to the core's gradient.
 *
 * The gradient is the core's and is tested there (`test/src/lh/core/pixel/gradient.cpp`);
 * here gl's colours go in and come out the same, a fractional box is laid on the floor
 * of its pixels, and gl's sampler gives every pixel of a row and a column — a row
 * longer than one chunk among them — the colour the gradient says on its own.
 */

#include <gtest/gtest.h>

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

TEST(ui_gradient, gl_colours_go_in_as_argb_and_come_out_the_same)
{
    const lh_gl_color_t colors[2] = {rgba(255, 0, 0, 255), rgba(0, 0, 255, 128)};
    lh_pixel_gradient_t gradient;

    lh_pixel_gradient_init(&gradient);
    lh_gl_gradient_init_stops(&gradient, colors, lh_null, 2);
    ASSERT_EQ(lh_pixel_gradient_get_stops_count(&gradient), 2);
    EXPECT_EQ(lh_pixel_gradient_stop_get_color(lh_pixel_gradient_get_stop(&gradient, 0)),
              0xFFFF0000U);
    EXPECT_EQ(lh_pixel_gradient_stop_get_color(lh_pixel_gradient_get_stop(&gradient, 1)),
              0x800000FFU);
    const lh_gl_color_t last = lh_test::out(lh_gl_gradient_color_at, &gradient, 255);

    EXPECT_TRUE(lh_gl_color_equals(&last, &colors[1]));
}

TEST(ui_gradient, a_box_is_laid_on_the_floor_of_its_pixels)
{
    lh_gl_rect_t rect;
    lh_math_rect_t px;

    lh_gl_rect_init(&rect, 3, 4, 10, 20);
    lh_gl_gradient_get_box_px(&rect, &px);
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin(&px)), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin(&px)), 4);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size(&px)), 10);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size(&px)), 20);
}

TEST(ui_gradient, a_sampled_row_and_column_are_every_pixel_asked_alone)
{
    static const int boxes[][4] = {{0, 0, 40, 30}, {-7, 5, 13, 51}, {5, 5, 2, 90}};
    /* Evenly, inset, on one step, and the wrong way round. */
    static const lh_byte_t stop_fracs[][2] = {{0, 255}, {30, 220}, {60, 60}, {200, 50}};
    const lh_gl_color_t colors[2] = {rgba(10, 200, 30, 255), rgba(250, 20, 140, 90)};
    /* Longer than a chunk, so the row is done in pieces. */
    constexpr int span = LH_GL_GRADIENT_SAMPLER_CHUNK + 60;
    static lh_gl_color_t row[span];
    lh_pixel_gradient_t gradient;

    for (const auto &fracs : stop_fracs)
    {
        lh_pixel_gradient_init(&gradient);
        lh_gl_gradient_init_stops(&gradient, colors, fracs, 2);
        for (int angle = -1; angle < 361; angle += 23)
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
                lh_gl_rect_t rect;
                lh_gl_gradient_sampler_t sampler;

                lh_gl_rect_init(&rect, b[0], b[1], b[2], b[3]);
                lh_gl_gradient_sampler_init(&sampler, &gradient, &rect);
                for (int y = b[1] - 3; y < b[1] + b[3] + 3; y += 5)
                {
                    lh_gl_gradient_sampler_row(&sampler, b[0] - 20, b[0] - 20 + span, y, row);
                    for (int x = b[0] - 20; x < b[0] - 20 + span; ++x)
                    {
                        const lh_gl_color_t want =
                            lh_test::out(lh_gl_gradient_color_at, &gradient,
                                         lh_gl_gradient_frac_at(&gradient, &rect, x, y));

                        ASSERT_TRUE(lh_gl_color_equals(&row[x - (b[0] - 20)], &want))
                            << "angle " << angle << " at " << x << "," << y;
                    }
                }
                const int x = b[0] + b[2] / 2;

                lh_gl_gradient_sampler_column(&sampler, x, b[1] - 20, b[1] - 20 + span, row);
                for (int y = b[1] - 20; y < b[1] - 20 + span; ++y)
                {
                    const lh_gl_color_t want =
                        lh_test::out(lh_gl_gradient_color_at, &gradient,
                                     lh_gl_gradient_frac_at(&gradient, &rect, x, y));

                    ASSERT_TRUE(lh_gl_color_equals(&row[y - (b[1] - 20)], &want))
                        << "column, angle " << angle << " at " << x << "," << y;
                }
            }
        }
    }
}

} // namespace
