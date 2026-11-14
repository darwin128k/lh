#include <gtest/gtest.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
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

TEST(gl_pixmap, canvas_fill_replaces_pixels_inside_the_clip)
{
    lh_gl_color_t pixels[12];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 4, 3);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 1, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 4, 3));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 10, 20, 30, 255);
    lh_gl_rect_t rect = rect_of(0, 1, 2, 1);
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1), 10, 20, 30, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 1), 10, 20, 30, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 0, 0, 0, 0));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 3, 1), 0, 0, 0, 0));
    lh_gl_canvas_set_clip(lh_addr_of(canvas), rect_of(1, 1, 1, 1));
    lh_gl_color_init(lh_addr_of(color), 255, 0, 0, 128);
    lh_gl_rect_t wide = rect_of(0, 1, 3, 1);
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(wide), lh_addr_of(color));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1), 133, 10, 15, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 1), 10, 20, 30, 255));
    EXPECT_EQ(lh_gl_canvas_backend_get_stroke_rect(lh_gl_pixmap_get_canvas_backend()), nullptr);
}

TEST(gl_pixmap, a_rect_outside_the_grid_writes_nothing)
{
    lh_gl_color_t pixels[4];
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 2, 2);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 9, 9, 9, 9);
    lh_gl_rect_t rect = rect_of(10, 10, 2, 2);
    lh_gl_rect_t clip = rect_of(0, 0, 2, 2);
    lh_gl_pixmap_fill_rect(lh_addr_of(pixmap), lh_addr_of(clip), lh_addr_of(rect), lh_addr_of(color));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 0, 0, 0, 0));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1), 0, 0, 0, 0));
}

TEST(gl_pixmap, a_circle_a_round_rect_and_a_triangle_leave_the_outside)
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
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 4, 4));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 200, 10, 10, 255);
    lh_gl_point_t center;
    lh_gl_point_init(lh_addr_of(center), 2, 2);
    lh_gl_canvas_fill_circle(lh_addr_of(canvas), center, 2, lh_addr_of(color));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 0, 0, 0, 0));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 2, 2), 200, 10, 10, 255));
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_rect_t box = rect_of(0, 0, 4, 4);
    lh_gl_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(box), 2, lh_addr_of(color));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 0, 0, 0, 0));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 1, 1), 200, 10, 10, 255));
    lh_memory_std_set(pixels, 0, sizeof pixels);
    lh_gl_point_t a;

    lh_gl_point_init(lh_addr_of(a), 0, 0);
    lh_gl_point_t b;

    lh_gl_point_init(lh_addr_of(b), 4, 0);
    lh_gl_point_t c;

    lh_gl_point_init(lh_addr_of(c), 0, 4);
    lh_gl_canvas_fill_triangle(lh_addr_of(canvas), lh_addr_of(a), lh_addr_of(b), lh_addr_of(c),
                               lh_addr_of(color));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 0, 0), 200, 10, 10, 255));
    EXPECT_TRUE(is_color(lh_gl_pixmap_get_pixel(lh_addr_of(pixmap), 3, 1), 0, 0, 0, 0));
}

} // namespace
