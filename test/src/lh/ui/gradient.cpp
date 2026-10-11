#include <gtest/gtest.h>

#include <lh/byte.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/ui/gradient/sampler.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

lh_ui_color_t
rgba(lh_ui_color_channel_t r, lh_ui_color_channel_t g, lh_ui_color_channel_t b,
     lh_ui_color_channel_t a)
{
    lh_ui_color_t color;
    lh_ui_color_init(lh_addr_of(color), r, g, b, a);
    return color;
}

TEST(ui_gradient, make_empty_has_no_stops)
{
    lh_ui_gradient_t gradient;

    lh_ui_gradient_init(lh_addr_of(gradient));
    EXPECT_EQ(lh_ui_gradient_get_stop_count(lh_addr_of(gradient)), 0);
}

TEST(ui_gradient, init_stops_even_fracs_default_capacity)
{
    const lh_ui_color_t colors[2] = {
        rgba(255, 0, 0, 255),
        rgba(0, 0, 255, 255),
    };
    lh_ui_gradient_t gradient;
    lh_ui_gradient_init_stops(lh_addr_of(gradient), colors, lh_null, 2);
    ASSERT_EQ(lh_ui_gradient_get_stop_count(lh_addr_of(gradient)), 2);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 0)),
              0);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 1)),
              255);
    EXPECT_EQ(lh_ui_color_get_r(lh_ui_gradient_stop_get_color_as_const(
                  lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 0))),
              255);
    EXPECT_EQ(lh_ui_color_get_b(lh_ui_gradient_stop_get_color_as_const(
                  lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 1))),
              255);
}

TEST(ui_gradient, init_stops_uses_explicit_fracs)
{
    const lh_ui_color_t colors[2] = {
        rgba(0, 0, 0, 255),
        rgba(255, 255, 255, 255),
    };
    const lh_byte_t fracs[2] = {40, 200};
    lh_ui_gradient_t gradient;
    lh_ui_gradient_init_stops(lh_addr_of(gradient), colors, fracs, 2);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 0)),
              40);
    EXPECT_EQ(lh_ui_gradient_stop_get_frac(lh_ui_gradient_get_stop_as_const(lh_addr_of(gradient), 1)),
              200);
}

TEST(ui_gradient, max_stops_option_is_at_least_one)
{
    EXPECT_GE(LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS, 1);
}

TEST(ui_gradient, a_sampled_row_is_every_pixel_asked_alone)
{
    static const int boxes[][4] = {{0, 0, 40, 30}, {-7, 5, 13, 51}, {3, -9, 64, 7}, {11, 2, 1, 1}, {5, 5, 2, 90}};
    /* Evenly, inset, on one step, and the wrong way round. */
    static const lh_byte_t stop_fracs[][2] = {{0, 255}, {30, 220}, {60, 60}, {200, 50}};
    lh_ui_color_t colors[2];
    lh_ui_gradient_t gradient;
    lh_ui_color_t row[120];

    /* The sampler carries the place on the stop axis from pixel to pixel and walks a
       radial one along fixed squared distances; every pixel has to come out as
       ::lh_ui_gradient_frac_at and ::lh_ui_gradient_color_at say on their own. */
    lh_ui_color_init(lh_addr_of(colors[0]), 10, 200, 30, 255);
    lh_ui_color_init(lh_addr_of(colors[1]), 250, 20, 140, 90);
    for (const auto &fracs : stop_fracs)
    {
        lh_ui_gradient_init(lh_addr_of(gradient));
        lh_ui_gradient_init_stops(lh_addr_of(gradient), colors, fracs, 2);
        for (int angle = -1; angle < 361; angle += 7)
        {
            if (angle < 0)
            {
                lh_ui_gradient_set_radial(lh_addr_of(gradient));
            }
            else
            {
                lh_ui_gradient_set_linear(lh_addr_of(gradient), angle);
            }
            for (const auto &b : boxes)
            {
                lh_ui_rect_t rect;
                lh_ui_gradient_sampler_t sampler;

                lh_ui_rect_init(lh_addr_of(rect), b[0], b[1], b[2], b[3]);
                lh_ui_gradient_sampler_init(lh_addr_of(sampler), lh_addr_of(gradient), lh_addr_of(rect));
                for (int y = b[1] - 3; y < b[1] + b[3] + 3; ++y)
                {
                    lh_ui_gradient_sampler_row(lh_addr_of(sampler), b[0] - 20, b[0] + 100, y, row);
                    for (int x = b[0] - 20; x < b[0] + 100; ++x)
                    {
                        const lh_ui_color_t want = lh_ui_gradient_color_at(
                            lh_addr_of(gradient), lh_ui_gradient_frac_at(lh_addr_of(gradient), lh_addr_of(rect), x, y));

                        ASSERT_TRUE(lh_ui_color_equals(row + (x - (b[0] - 20)), lh_addr_of(want)))
                            << "angle " << angle << " box " << b[0] << "," << b[1] << " " << b[2] << "x" << b[3]
                            << " at " << x << "," << y;
                    }
                }
            }
        }
    }
}

} // namespace
