#include <gtest/gtest.h>

#include <lh/ui/brush.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>

namespace
{

const lh_ui_color_t *
pixel(const lh_ui_canvas_t *canvas, lh_math_coord_t x, lh_math_coord_t y)
{
    return lh_ui_canvas_get_pixel(canvas, x, y);
}

TEST(ui_canvas, fill_paints_the_rect_and_stops_at_its_edge)
{
    lh_ui_color_t pixels[4 * 4] = {};
    lh_ui_canvas_t canvas;
    lh_ui_color_t red;
    lh_ui_brush_t brush;
    lh_ui_color_init(lh_addr_of(red), 255, 0, 0, 255);
    lh_ui_brush_init(lh_addr_of(brush), lh_addr_of(red));
    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 4, 4, 4);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_math_rect_make(1, 1, 2, 2), lh_addr_of(brush));

    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 1, 1)), 255);
    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 2, 2)), 255);
    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 0, 0)), 0);
    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 3, 1)), 0);
}

TEST(ui_canvas, clip_discards_the_part_outside_it)
{
    lh_ui_color_t pixels[4 * 4] = {};
    lh_ui_canvas_t canvas;
    lh_ui_color_t blue;
    lh_ui_brush_t brush;
    lh_ui_color_init(lh_addr_of(blue), 0, 0, 255, 255);
    lh_ui_brush_init(lh_addr_of(brush), lh_addr_of(blue));
    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 4, 4, 4);
    lh_ui_canvas_set_clip(lh_addr_of(canvas), lh_math_rect_make(0, 0, 2, 2));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_math_rect_make(0, 0, 4, 4), lh_addr_of(brush));

    EXPECT_EQ(lh_ui_color_get_b(pixel(lh_addr_of(canvas), 1, 1)), 255);
    EXPECT_EQ(lh_ui_color_get_b(pixel(lh_addr_of(canvas), 2, 2)), 0);
}

TEST(ui_canvas, stroke_paints_the_inside_border_only)
{
    lh_ui_color_t pixels[6 * 6] = {};
    lh_ui_canvas_t canvas;
    lh_ui_color_t green;
    lh_ui_pen_t pen;
    lh_ui_color_init(lh_addr_of(green), 0, 255, 0, 255);
    lh_ui_pen_init(lh_addr_of(pen), lh_addr_of(green), 1);
    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 6, 6, 6);
    lh_ui_canvas_stroke_rect(lh_addr_of(canvas), lh_math_rect_make(1, 1, 4, 4), lh_addr_of(pen));

    EXPECT_EQ(lh_ui_color_get_g(pixel(lh_addr_of(canvas), 1, 1)), 255);
    EXPECT_EQ(lh_ui_color_get_g(pixel(lh_addr_of(canvas), 4, 2)), 255);
    EXPECT_EQ(lh_ui_color_get_g(pixel(lh_addr_of(canvas), 2, 2)), 0);
    EXPECT_EQ(lh_ui_color_get_g(pixel(lh_addr_of(canvas), 0, 0)), 0);
}

TEST(ui_canvas, clear_color_changes_nothing)
{
    lh_ui_color_t pixels[2 * 2] = {};
    lh_ui_canvas_t canvas;
    lh_ui_color_t clear;
    lh_ui_brush_t brush;
    lh_ui_color_init(lh_addr_of(clear), 255, 0, 0, 0);
    lh_ui_brush_init(lh_addr_of(brush), lh_addr_of(clear));
    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 2, 2, 2);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_math_rect_make(0, 0, 2, 2), lh_addr_of(brush));

    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 0, 0)), 0);
    EXPECT_EQ(lh_ui_color_get_a(pixel(lh_addr_of(canvas), 0, 0)), 0);
}

TEST(ui_canvas, horizontal_gradient_runs_from_the_first_stop_to_the_second)
{
    lh_ui_color_t pixels[4] = {};
    lh_ui_canvas_t canvas;
    lh_ui_color_t from;
    lh_ui_color_t to;
    lh_ui_gradient_t gradient;
    lh_ui_brush_t brush;
    lh_ui_color_init(lh_addr_of(from), 0, 0, 0, 255);
    lh_ui_color_init(lh_addr_of(to), 255, 0, 0, 255);
    lh_ui_gradient_init_linear(lh_addr_of(gradient), lh_addr_of(from), lh_addr_of(to), lh_ui_gradient_horizontal);
    lh_ui_brush_init_gradient(lh_addr_of(brush), lh_addr_of(gradient));
    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 4, 1, 4);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_math_rect_make(0, 0, 4, 1), lh_addr_of(brush));

    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 0, 0)), 0);
    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 3, 0)), 255);
}

TEST(ui_canvas, radial_gradient_is_the_first_stop_at_the_center)
{
    lh_ui_color_t pixels[5 * 5] = {};
    lh_ui_canvas_t canvas;
    lh_ui_color_t from;
    lh_ui_color_t to;
    lh_ui_gradient_t gradient;
    lh_ui_brush_t brush;
    lh_ui_color_init(lh_addr_of(from), 0, 0, 0, 255);
    lh_ui_color_init(lh_addr_of(to), 255, 0, 0, 255);
    lh_ui_gradient_init_radial(lh_addr_of(gradient), lh_addr_of(from), lh_addr_of(to));
    lh_ui_brush_init_gradient(lh_addr_of(brush), lh_addr_of(gradient));
    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 5, 5, 5);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_math_rect_make(0, 0, 5, 5), lh_addr_of(brush));

    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 2, 2)), 0);
    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 0, 0)), 255);
    EXPECT_GT(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 3, 2)), 0);
    EXPECT_LT(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 3, 2)), 255);
}

TEST(ui_canvas, angular_gradient_runs_clockwise_from_the_right)
{
    lh_ui_color_t pixels[5 * 5] = {};
    lh_ui_canvas_t canvas;
    lh_ui_color_t from;
    lh_ui_color_t to;
    lh_ui_gradient_t gradient;
    lh_ui_brush_t brush;
    lh_ui_color_init(lh_addr_of(from), 0, 0, 0, 255);
    lh_ui_color_init(lh_addr_of(to), 255, 0, 0, 255);
    lh_ui_gradient_init_angular(lh_addr_of(gradient), lh_addr_of(from), lh_addr_of(to));
    lh_ui_brush_init_gradient(lh_addr_of(brush), lh_addr_of(gradient));
    lh_ui_canvas_init(lh_addr_of(canvas), pixels, 5, 5, 5);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_math_rect_make(0, 0, 5, 5), lh_addr_of(brush));

    EXPECT_EQ(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 4, 2)), 0);
    EXPECT_GT(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 2, 4)), lh_ui_color_get_r(pixel(lh_addr_of(canvas), 4, 2)));
    EXPECT_GT(lh_ui_color_get_r(pixel(lh_addr_of(canvas), 0, 2)), lh_ui_color_get_r(pixel(lh_addr_of(canvas), 2, 4)));
}

} /* namespace */
