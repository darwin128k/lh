#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/fill_probe.h>

#include <lh/ui/canvas.h>
#include <lh/ui/canvas/box.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/radii.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/test/out.h>

namespace
{
using lh_test::rect_of;

/* A real software canvas over 120 x 120 ARGB pixels, so a test reads what was drawn. */
struct surface
{
    static const int side = 120;
    lh_u32_t pixels[side * side];
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;

    surface()
    {
        lh_ui_size_t size;

        for (lh_u32_t &p : pixels)
        {
            p = 0;
        }
        lh_ui_pixmap_init(&pixmap, lh_ptr_rcast(lh_byte_t, pixels), side, side, side * 4,
                          lh_ui_pixmap_format_argb8888);
        lh_ui_canvas_sw_init(&sw);
        lh_ui_canvas_sw_set_pixmap(&sw, &pixmap);
        lh_ui_canvas_init(&canvas, &lh_ui_canvas_backend_sw, &sw);
        lh_ui_size_init(&size, lh_ui_scalar(side), lh_ui_scalar(side));
        lh_ui_canvas_set_size(&canvas, size);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return pixels[y * side + x];
    }
};

lh_ui_color_t
color_of(int r, int g, int b)
{
    lh_ui_color_t c;
    lh_ui_color_init(&c, (lh_ui_color_channel_t)r, (lh_ui_color_channel_t)g, (lh_ui_color_channel_t)b, 255);
    return c;
}

int
red_of(lh_u32_t argb)
{
    return (int)((argb >> 16) & 0xFF);
}

int
blue_of(lh_u32_t argb)
{
    return (int)(argb & 0xFF);
}

} // namespace

/* -- Radii ----------------------------------------------------------------- */

/* Two corners on a side that add up to more than the side are scaled down together,
   by the factor of the side that is most over-full, as CSS does. 30 + 30 on a side of
   40 is a factor of two thirds. */
TEST(ui_radii, corners_that_do_not_fit_are_scaled_down_together)
{
    lh_ui_radii_t radii;
    lh_ui_radii_t fit;
    const lh_ui_rect_t rect = rect_of(0, 0, 100, 40);

    /* 30 + 0 fits the 40 of the left side, and CSS would draw 30. A corner here is no
       more than half the shorter side -- 20 -- because each quarter of the box is cut
       by its own corner only. */
    lh_ui_radii_init(&radii, lh_ui_scalar(30), lh_ui_scalar(30), lh_ui_scalar(0), lh_ui_scalar(0));
    fit = lh_test::out(lh_ui_radii_clamp, &radii, &rect);
    EXPECT_EQ(fit.top_left, lh_ui_scalar(20));
    EXPECT_EQ(fit.bottom_left, lh_ui_scalar(0));

    lh_ui_radii_init(&radii, lh_ui_scalar(30), lh_ui_scalar(30), lh_ui_scalar(30), lh_ui_scalar(30));
    fit = lh_test::out(lh_ui_radii_clamp, &radii, &rect);
    EXPECT_EQ(fit.top_left, lh_ui_scalar(20));
    EXPECT_EQ(fit.bottom_right, lh_ui_scalar(20));
}

TEST(ui_radii, a_pixel_takes_the_radius_of_the_corner_whose_quarter_it_is_in)
{
    lh_ui_radii_t radii;
    const lh_ui_rect_t rect = rect_of(10, 10, 40, 20);

    lh_ui_radii_init(&radii, lh_ui_scalar(1), lh_ui_scalar(2), lh_ui_scalar(3), lh_ui_scalar(4));
    EXPECT_EQ(lh_ui_radii_get_at(&radii, &rect, 10, 10), lh_ui_scalar(1));
    EXPECT_EQ(lh_ui_radii_get_at(&radii, &rect, 49, 10), lh_ui_scalar(2));
    EXPECT_EQ(lh_ui_radii_get_at(&radii, &rect, 49, 29), lh_ui_scalar(3));
    EXPECT_EQ(lh_ui_radii_get_at(&radii, &rect, 10, 29), lh_ui_scalar(4));
    EXPECT_EQ(lh_ui_radii_is_uniform(&radii), lh_bool_false);
    EXPECT_EQ(lh_ui_radii_get_min(&radii), lh_ui_scalar(1));
}

/* -- Gradient over a box ------------------------------------------------------ */

TEST(ui_gradient, a_linear_gradient_runs_the_way_its_angle_points)
{
    lh_ui_gradient_t g;
    const lh_ui_rect_t rect = rect_of(0, 0, 10, 100);

    lh_ui_gradient_init(&g);
    EXPECT_EQ(lh_ui_gradient_get_angle(&g), 180) << "CSS's default is top to bottom";
    EXPECT_LT(lh_ui_gradient_frac_at(&g, &rect, 5, 0), 5);
    EXPECT_GT(lh_ui_gradient_frac_at(&g, &rect, 5, 99), 250);
    EXPECT_NEAR(lh_ui_gradient_frac_at(&g, &rect, 5, 50), 128, 3);

    lh_ui_gradient_set_linear(&g, 90);
    const lh_ui_rect_t wide = rect_of(0, 0, 100, 10);
    EXPECT_LT(lh_ui_gradient_frac_at(&g, &wide, 0, 5), 5) << "90 degrees runs left to right";
    EXPECT_GT(lh_ui_gradient_frac_at(&g, &wide, 99, 5), 250);

    lh_ui_gradient_set_linear(&g, 0);
    EXPECT_GT(lh_ui_gradient_frac_at(&g, &rect, 5, 0), 250) << "0 degrees runs bottom to top";
}

TEST(ui_gradient, a_radial_gradient_runs_from_the_middle_to_the_corners)
{
    lh_ui_gradient_t g;
    const lh_ui_rect_t rect = rect_of(0, 0, 100, 50);

    lh_ui_gradient_init(&g);
    lh_ui_gradient_set_radial(&g);
    EXPECT_LT(lh_ui_gradient_frac_at(&g, &rect, 50, 25), 8);
    EXPECT_GT(lh_ui_gradient_frac_at(&g, &rect, 0, 0), 245);
    EXPECT_GT(lh_ui_gradient_frac_at(&g, &rect, 99, 49), 245);
}

TEST(ui_gradient, a_colour_between_two_stops_is_the_two_mixed_in_proportion)
{
    lh_ui_gradient_t g;
    const lh_ui_color_t stops[2] = {color_of(200, 0, 0), color_of(0, 0, 100)};
    lh_ui_color_t mid;

    lh_ui_gradient_init(&g);
    lh_ui_gradient_init_stops(&g, stops, lh_null, 2);
    mid = lh_test::out(lh_ui_gradient_color_at, &g, 51);
    EXPECT_EQ(lh_ui_color_get_r(&mid), 160);
    EXPECT_EQ(lh_ui_color_get_b(&mid), 20);
}

/* -- Filling a box ------------------------------------------------------------ */

/* One colour and one radius are the backend's own rounded fill, as they always were. */
TEST(ui_canvas_box, a_solid_box_with_one_radius_is_the_backends_rounded_fill)
{
    lh_test::fill_probe probe{};
    lh_ui_canvas_backend_t backend{};
    lh_ui_canvas_t canvas;
    lh_ui_radii_t radii;
    lh_ui_paint_t paint;
    lh_ui_size_t size;

    probe.rect = rect_of(0, 0, 40, 40);
    probe.color = color_of(1, 2, 3);
    backend.fill_rect = lh_test::fill_probe_fill_rect;
    backend.fill_round_rect = lh_test::fill_probe_fill_round_rect;
    lh_ui_canvas_init(&canvas, &backend, &probe);
    lh_ui_size_init(&size, lh_ui_scalar(100), lh_ui_scalar(100));
    lh_ui_canvas_set_size(&canvas, size);
    lh_ui_radii_init_all(&radii, lh_ui_scalar(6));
    lh_ui_paint_init_color(&paint, &probe.color);

    lh_ui_canvas_fill_box(&canvas, &probe.rect, &radii, &paint);
    EXPECT_EQ(probe.round_matches, 1);
    EXPECT_EQ(probe.radius, lh_ui_scalar(6));
}

/* `border-radius: 10px 0 0 0`: the top-left corner is cut and the other three are
   square to their last pixel. */
TEST(ui_canvas_box, each_corner_is_rounded_by_its_own_radius)
{
    surface s;
    lh_ui_radii_t radii;
    lh_ui_paint_t paint;
    const lh_ui_color_t red = color_of(255, 0, 0);
    const lh_ui_rect_t rect = rect_of(10, 10, 40, 40);

    lh_ui_radii_init(&radii, lh_ui_scalar(10), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_paint_init_color(&paint, &red);
    lh_ui_canvas_fill_box(&s.canvas, &rect, &radii, &paint);

    EXPECT_EQ(s.at(10, 10) >> 24, 0u) << "the rounded corner's outermost pixel is cut away";
    EXPECT_EQ(s.at(49, 10), 0xFFFF0000u) << "top right is square";
    EXPECT_EQ(s.at(49, 49), 0xFFFF0000u) << "bottom right is square";
    EXPECT_EQ(s.at(10, 49), 0xFFFF0000u) << "bottom left is square";
    EXPECT_EQ(s.at(30, 30), 0xFFFF0000u);
    EXPECT_EQ(s.at(9, 30), 0u) << "nothing outside the box";
}

/* A gradient from red to blue, top to bottom: red at the top, blue at the bottom, and
   the two mixed half way. */
TEST(ui_canvas_box, a_gradient_fill_runs_from_its_first_stop_to_its_last)
{
    surface s;
    lh_ui_radii_t radii;
    lh_ui_gradient_t g;
    lh_ui_paint_t paint;
    const lh_ui_color_t stops[2] = {color_of(255, 0, 0), color_of(0, 0, 255)};
    const lh_ui_rect_t rect = rect_of(0, 0, 20, 100);

    lh_ui_gradient_init(&g);
    lh_ui_gradient_init_stops(&g, stops, lh_null, 2);
    lh_ui_paint_init_gradient(&paint, &g);
    lh_ui_radii_init_all(&radii, lh_ui_scalar(0));
    lh_ui_canvas_fill_box(&s.canvas, &rect, &radii, &paint);

    EXPECT_GT(red_of(s.at(10, 0)), 245);
    EXPECT_LT(blue_of(s.at(10, 0)), 10);
    EXPECT_GT(blue_of(s.at(10, 99)), 245);
    EXPECT_NEAR(red_of(s.at(10, 50)), 127, 4);
    EXPECT_EQ(s.at(0, 50), s.at(19, 50)) << "a row of a top-to-bottom gradient is one colour";
}
