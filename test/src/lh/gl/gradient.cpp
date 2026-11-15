#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/gradient/fill.h>
#include <lh/gl/gradient/sine.h>
#include <lh/gl/layer.h>
#include <lh/gl/pixmap.h>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

lh_bool_t
is_color(const lh_gl_color_t *color, lh_gl_color_channel_t r, lh_gl_color_channel_t g,
         lh_gl_color_channel_t b, lh_gl_color_channel_t a)
{
    return lh_gl_color_get_r(color) == r && lh_gl_color_get_g(color) == g &&
                   lh_gl_color_get_b(color) == b && lh_gl_color_get_a(color) == a
               ? lh_bool_true
               : lh_bool_false;
}

TEST(gl_gradient, fraction_zero_is_the_first_color_and_255_is_the_second)
{
    lh_gl_color_t from;
    lh_gl_color_init(lh_addr_of(from), 0, 0, 0, 255);
    lh_gl_color_t to;
    lh_gl_color_init(lh_addr_of(to), 255, 0, 0, 255);
    lh_gl_gradient_t gradient;
    lh_gl_gradient_init(lh_addr_of(gradient), lh_addr_of(from), lh_addr_of(to));
    lh_gl_color_t out;
    lh_gl_gradient_color_at(lh_addr_of(gradient), 0, lh_addr_of(out));
    EXPECT_TRUE(is_color(lh_addr_of(out), 0, 0, 0, 255));
    lh_gl_gradient_color_at(lh_addr_of(gradient), 255, lh_addr_of(out));
    EXPECT_TRUE(is_color(lh_addr_of(out), 255, 0, 0, 255));
    lh_gl_gradient_color_at(lh_addr_of(gradient), 128, lh_addr_of(out));
    EXPECT_TRUE(is_color(lh_addr_of(out), 128, 0, 0, 255));
    EXPECT_EQ(lh_gl_gradient_sine(90), 4096);
    EXPECT_EQ(lh_gl_gradient_sine(180), 0);
    EXPECT_EQ(lh_gl_gradient_sine(45), 2896);
    EXPECT_EQ(lh_gl_gradient_linear_frac(180, 2, 2, 0, 0), 63);
    EXPECT_EQ(lh_gl_gradient_linear_frac(180, 2, 2, 0, 1), 191);
    EXPECT_EQ(lh_gl_gradient_linear_frac(135, 2, 2, 0, 0), 63);
    EXPECT_EQ(lh_gl_gradient_linear_frac(135, 2, 2, 1, 0), 127);
    EXPECT_EQ(lh_gl_gradient_linear_frac(135, 2, 2, 0, 1), 127);
    EXPECT_EQ(lh_gl_gradient_linear_frac(135, 2, 2, 1, 1), 191);
    EXPECT_EQ(lh_gl_gradient_radial_frac(1, 1, 0, 0), 0);
    EXPECT_EQ(lh_gl_gradient_radial_frac(4, 4, 1, 1), 63);
    EXPECT_EQ(lh_gl_gradient_radial_frac(4, 4, 2, 2), 63);
    EXPECT_EQ(lh_gl_gradient_radial_frac(4, 4, 0, 0), 191);
    EXPECT_EQ(lh_gl_gradient_radial_frac(4, 4, 3, 3), 191);
}

TEST(gl_gradient_fill, paints_the_top_row_from_the_first_color)
{
    lh_gl_color_t pixels[4];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 2, 2);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 2, 2));
    lh_gl_color_t from;
    lh_gl_color_init(lh_addr_of(from), 10, 20, 30, 255);
    lh_gl_color_t to;
    lh_gl_color_init(lh_addr_of(to), 40, 50, 60, 255);
    lh_gl_gradient_t gradient;
    lh_gl_gradient_init(lh_addr_of(gradient), lh_addr_of(from), lh_addr_of(to));
    lh_gl_gradient_fill_t fill;
    lh_gl_gradient_fill_init(lh_addr_of(fill), rect_of(0, 0, 2, 2), lh_addr_of(gradient));
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_gradient_fill_get_entity_mut(lh_addr_of(fill)));
    lh_gl_rect_t clip = rect_of(0, 0, 2, 2);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    lh_gl_color_t top;
    lh_gl_gradient_color_at(lh_addr_of(gradient), lh_gl_gradient_linear_frac(180, 2, 2, 0, 0),
                            lh_addr_of(top));
    lh_gl_color_t bottom;
    lh_gl_gradient_color_at(lh_addr_of(gradient), lh_gl_gradient_linear_frac(180, 2, 2, 0, 1),
                            lh_addr_of(bottom));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0),
                         lh_gl_color_get_r(lh_addr_of(top)),
                         lh_gl_color_get_g(lh_addr_of(top)),
                         lh_gl_color_get_b(lh_addr_of(top)), 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 0),
                         lh_gl_color_get_r(lh_addr_of(top)),
                         lh_gl_color_get_g(lh_addr_of(top)),
                         lh_gl_color_get_b(lh_addr_of(top)), 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 1),
                         lh_gl_color_get_r(lh_addr_of(bottom)),
                         lh_gl_color_get_g(lh_addr_of(bottom)),
                         lh_gl_color_get_b(lh_addr_of(bottom)), 255));
    EXPECT_NE(lh_gl_color_get_r(lh_addr_of(top)), lh_gl_color_get_r(lh_addr_of(bottom)));
    EXPECT_FALSE(lh_gl_entity_class_get_opaque(lh_gl_gradient_fill_get_class()));
    lh_gl_color_init(lh_addr_of(to), 1, 2, 3, 255);
    lh_gl_gradient_set_to(lh_addr_of(gradient), lh_addr_of(to));
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    lh_gl_gradient_color_at(lh_addr_of(gradient), lh_gl_gradient_linear_frac(180, 2, 2, 0, 1),
                            lh_addr_of(bottom));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 1),
                         lh_gl_color_get_r(lh_addr_of(bottom)),
                         lh_gl_color_get_g(lh_addr_of(bottom)),
                         lh_gl_color_get_b(lh_addr_of(bottom)), 255));
    lh_gl_gradient_fill_set_angle(lh_addr_of(fill), 135);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    lh_gl_color_t down_right;
    lh_gl_gradient_color_at(lh_addr_of(gradient), lh_gl_gradient_linear_frac(135, 2, 2, 1, 1),
                            lh_addr_of(down_right));
    EXPECT_NE(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0)),
              lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1)));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1),
                         lh_gl_color_get_r(lh_addr_of(down_right)),
                         lh_gl_color_get_g(lh_addr_of(down_right)),
                         lh_gl_color_get_b(lh_addr_of(down_right)), 255));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 0)),
              lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 1)));
    lh_gl_gradient_fill_set_gradient(lh_addr_of(fill), nullptr);
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 0, 0, 0, 0));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_gradient_fill, a_radius_leaves_the_corner_clear)
{
    lh_gl_color_t pixels[16];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 4, 4);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 4, 4));
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 200, 10, 10, 255);
    lh_gl_gradient_t gradient;
    lh_gl_gradient_init(lh_addr_of(gradient), lh_addr_of(color), lh_addr_of(color));
    lh_gl_gradient_fill_t fill;
    lh_gl_gradient_fill_init(lh_addr_of(fill), rect_of(0, 0, 4, 4), lh_addr_of(gradient));
    lh_gl_gradient_fill_set_radius(lh_addr_of(fill), 2);
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_gradient_fill_get_entity_mut(lh_addr_of(fill)));
    lh_gl_rect_t clip = rect_of(0, 0, 4, 4);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 0, 0, 0, 0));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 3, 3), 0, 0, 0, 0));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1), 200, 10, 10, 255));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_gradient_fill, radial_is_the_first_color_near_the_middle)
{
    lh_gl_color_t pixels[16];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 4, 4);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 4, 4));
    lh_gl_color_t from;
    lh_gl_color_init(lh_addr_of(from), 0, 0, 0, 255);
    lh_gl_color_t to;
    lh_gl_color_init(lh_addr_of(to), 255, 0, 0, 255);
    lh_gl_gradient_t gradient;
    lh_gl_gradient_init(lh_addr_of(gradient), lh_addr_of(from), lh_addr_of(to));
    lh_gl_gradient_fill_t fill;
    lh_gl_gradient_fill_init(lh_addr_of(fill), rect_of(0, 0, 4, 4), lh_addr_of(gradient));
    lh_gl_gradient_fill_set_radial(lh_addr_of(fill), lh_bool_true);
    lh_gl_layer_add(lh_addr_of(layer), lh_gl_gradient_fill_get_entity_mut(lh_addr_of(fill)));
    lh_gl_rect_t clip = rect_of(0, 0, 4, 4);
    lh_gl_layer_draw(lh_addr_of(layer), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1), 63, 0, 0, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 191, 0, 0, 255));
    EXPECT_EQ(lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1)),
              lh_gl_color_get_r(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 2)));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

} // namespace
